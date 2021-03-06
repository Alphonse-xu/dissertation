/// Author: Ralph Ridley
/// Date: 31/01/19

#include "BasicRenderer.h"
#include "VaoWrapper.h"
#include "RendererStorage.h"

using namespace QZL;
using namespace QZL::AZDO;

BasicRenderer::BasicRenderer(ShaderPipeline* pipeline, VaoWrapper* vao, const glm::mat4* viewMatrix)
	: Base(pipeline, vao), viewMatrix_(viewMatrix)
{
}

void BasicRenderer::initialise()
{
	setupInstanceDataBuffer();
	auto instPtr = renderStorage_->instanceData();
	for (size_t i = 0; i < renderStorage_->instanceCount(); ++i) {
		glm::mat4 model = (instPtr + i)->transform.toModelMatrix();
		instanceDataBufPtr_[i] = {
			model, Shared::kProjectionMatrix * *viewMatrix_ * model
		};
	}
}

void BasicRenderer::doFrame(const glm::mat4& viewMatrix)
{
	bindInstanceDataBuffer();
	pipeline_->use();
	renderStorage_->vao()->bind();
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer_);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, renderStorage_->meshCount() * sizeof(DrawElementsCommand), renderStorage_->meshData(), GL_DYNAMIC_DRAW);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, nullptr, renderStorage_->meshCount(), 0);
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	renderStorage_->vao()->unbind();
	pipeline_->unuse();
}
