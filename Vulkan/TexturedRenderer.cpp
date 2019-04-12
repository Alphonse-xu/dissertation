#include "TexturedRenderer.h"
#include "ElementBuffer.h"
#include "UniformBuffer.h"
#include "LogicDevice.h"
#include "Descriptor.h"
#include "TextureSampler.h"
#include "TextureLoader.h"
#include "DeviceMemory.h"
#include "RendererPipeline.h"

using namespace QZL;

TexturedRenderer::TexturedRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
	const std::string& vertexShader, const std::string& fragmentShader)
	: RendererBase(), textureLoader_(new TextureLoader(logicDevice, logicDevice->getDeviceMemory()))
{
	textureSampler_ = new TextureSampler(logicDevice, textureLoader_->loadTexture("Mandelbrot"), VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 8, 1);

	// Setup uniform buffers
	UniformBuffer* mvpBuf = new UniformBuffer(logicDevice, MemoryAllocationPattern::kDynamicResource, 0, 0,
		sizeof(ElementData) * 10, VK_SHADER_STAGE_VERTEX_BIT);
	auto layout = descriptor->makeLayout({ mvpBuf->getBinding(), textureSampler_->getBinding() });
	uniformBuffers_.push_back(mvpBuf);
	size_t idx = descriptor->createSets({ layout, layout, layout });
	std::vector<VkWriteDescriptorSet> descWrites;
	for (int i = 0; i < 3; ++i) {
		descriptorSets_.push_back(descriptor->getSet(idx + i));
		descWrites.push_back(mvpBuf->descriptorWrite(descriptor->getSet(idx + i)));
		descWrites.push_back(textureSampler_->descriptorWrite(descriptor->getSet(idx + i)));
	}
	descriptor->updateDescriptorSets(descWrites);

	// Pipeline
	createPipeline(logicDevice, renderPass, swapChainExtent, RendererPipeline::makeLayoutInfo(uniformBuffers_.size(), &layout), vertexShader, fragmentShader);
}

TexturedRenderer::~TexturedRenderer()
{
	SAFE_DELETE(textureSampler_);
	SAFE_DELETE(textureLoader_);
}

void TexturedRenderer::initialise(const glm::mat4& viewMatrix)
{
	if (Shared::kProjectionMatrix[1][1] >= 0)
		Shared::kProjectionMatrix[1][1] *= -1;
	ElementData* eleDataPtr = static_cast<ElementData*>(uniformBuffers_[0]->bindUniformRange());
	for (auto& it : meshes_) {
		uint32_t instanceCount = 0;
		for (auto& it2 : it.second) {
			for (int i = 0; i < it2.second.size(); ++i) {
				auto inst = it2.second[i];
				glm::mat4 model = inst->transform.toModelMatrix();
				eleDataPtr[instanceCount + i].modelMatrix = model;
				eleDataPtr[instanceCount + i].mvpMatrix = Shared::kProjectionMatrix * viewMatrix * model;
			}
			instanceCount += it2.second.size();
		}
	}
	uniformBuffers_[0]->unbindUniformRange();
}

void TexturedRenderer::recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer)
{
	beginFrame(cmdBuffer);

	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 0, 1, &descriptorSets_[idx], 0, nullptr);
	for (auto& it : meshes_) {
		it.first->bind(cmdBuffer);

		uint32_t instanceCount = 0;
		for (auto& it2 : it.second) {
			const BasicMesh* mesh = (*it.first)[it2.first];
			vkCmdDrawIndexed(cmdBuffer, mesh->indexCount, it2.second.size(), mesh->indexOffset, mesh->vertexOffset, instanceCount);
			instanceCount += it2.second.size();
		}
	}
}
