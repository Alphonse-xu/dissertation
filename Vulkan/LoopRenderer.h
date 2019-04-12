#pragma once
#include "RendererBase.h"

namespace QZL
{
	class LoopRenderer : public RendererBase<MeshInstance> {
	public:
		LoopRenderer(const LogicDevice* logicDevice, VkRenderPass renderPass, VkExtent2D swapChainExtent, Descriptor* descriptor,
			const std::string& vertexShader, const std::string& fragmentShader);
		void recordFrame(const glm::mat4& viewMatrix, const uint32_t idx, VkCommandBuffer cmdBuffer) override;
	};
}
