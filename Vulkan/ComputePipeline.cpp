#include "ComputePipeline.h"
#include "LogicDevice.h"
#include "Shader.h"

using namespace QZL;

ComputePipeline::ComputePipeline(const LogicDevice* logicDevice, VkPipelineLayoutCreateInfo layoutInfo, const std::string& computeShader)
	: logicDevice_(logicDevice)
{
	Shader module = { *logicDevice_, computeShader };

	VkPipelineShaderStageCreateInfo stageInfo;
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageInfo.module = module.getModule();
	stageInfo.pName = "main";

	CHECK_VKRESULT(vkCreatePipelineLayout(*logicDevice_, &layoutInfo, nullptr, &layout_));

	VkComputePipelineCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.stage = stageInfo;
	createInfo.layout = layout_;

	CHECK_VKRESULT(vkCreateComputePipelines(*logicDevice_, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline_));
}

VkPipelineLayoutCreateInfo ComputePipeline::makeLayoutInfo(const uint32_t layoutCount, const VkDescriptorSetLayout* layouts)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = layoutCount;
	pipelineLayoutInfo.pSetLayouts = layouts;
	return pipelineLayoutInfo;
}