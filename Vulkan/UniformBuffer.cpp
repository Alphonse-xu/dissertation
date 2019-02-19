#include "UniformBuffer.h"

using namespace QZL;

UniformBuffer::UniformBuffer(const LogicDevice* logicDevice, MemoryAllocationPattern pattern,
	uint32_t bindingIdx_, VkBufferUsageFlags flags, VkDeviceSize maxSize, VkShaderStageFlags stageFlags)
	: logicDevice_(logicDevice), size_(maxSize)
{
	bufferDetails_ = logicDevice_->getDeviceMemory()->createBuffer(pattern, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, maxSize);
	// TODO create staging buffer transfer alternative
	ENSURES(bufferDetails_.access == MemoryAccessType::kDirect || bufferDetails_.access == MemoryAccessType::kPersistant);

	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = bindingIdx_;
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = stageFlags;
	binding_ = layoutBinding;
}

UniformBuffer::~UniformBuffer()
{
	logicDevice_->getDeviceMemory()->deleteAllocation(bufferDetails_.id, bufferDetails_.buffer);
}

const VkDescriptorSetLayoutBinding& UniformBuffer::getBinding()
{
	return binding_;
}

VkWriteDescriptorSet UniformBuffer::descriptorWrite(VkDescriptorSet set)
{
	bufferInfo_ = {};
	bufferInfo_.buffer = bufferDetails_.buffer;
	bufferInfo_.offset = 0;
	bufferInfo_.range = size_;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = set;
	descriptorWrite.dstBinding = bindingIdx_;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo_;

	return descriptorWrite;
}

void* UniformBuffer::bindUniformRange()
{
	return logicDevice_->getDeviceMemory()->mapMemory(bufferDetails_.id);
}

void UniformBuffer::unbindUniformRange()
{
	logicDevice_->getDeviceMemory()->unmapMemory(bufferDetails_.id);
}

