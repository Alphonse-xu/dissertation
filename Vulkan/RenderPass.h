#pragma once
#include "VkUtil.h"

namespace QZL
{
	class Image2D;
	class LogicDevice;
	class RendererPipeline;
	struct SwapChainDetails;

	class RenderPass {
		friend class SwapChain;
	public:
		void doFrame(const uint32_t idx, VkCommandBuffer cmdBuffer);
	private:
		RenderPass(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		~RenderPass();

		void createFramebuffers(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		void createBackBuffer(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);
		VkFormat createDepthBuffer(LogicDevice* logicDevice, const SwapChainDetails& swapChainDetails);

		VkRenderPass renderPass_;
		std::vector<VkFramebuffer> framebuffers_;
		const SwapChainDetails& swapChainDetails_;
		const LogicDevice* logicDevice_;

		std::vector<RendererPipeline*> renderers_;

		Image2D* backBuffer_;
		Image2D* depthBuffer_;
	};
}
