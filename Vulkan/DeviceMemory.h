#pragma once
#include "vk_mem_alloc.h"
#include "MemoryAllocation.h"

namespace QZL
{
	class LogicDevice;
	class PhysicalDevice;
	struct SystemDetails;

	/// Interface for https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html
	class DeviceMemory {
		friend class LogicDevice;
	public:
		const MemoryAllocationDetails createBuffer(MemoryAllocationPattern pattern, VkBufferUsageFlags bufferUsage, VkDeviceSize size);
		const MemoryAllocationDetails createImage(MemoryAllocationPattern pattern, VkImageCreateInfo imageCreateInfo);
		void deleteAllocation(AllocationID id, VkBuffer buffer);
		void deleteAllocation(AllocationID id, VkImage image);
		void* mapMemory(const AllocationID& id);
		void unmapMemory(const AllocationID& id);
		void transferMemory(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);
		void transferMemory(const VkBuffer& srcBuffer, const VkImage& dstImage, VkDeviceSize srcOffset, uint32_t width, uint32_t height);
		void changeImageLayout(const VkImage& image, const VkImageLayout srcLayout, const VkImageLayout dstLayout, const VkFormat& format, uint32_t mipLevels);
	private:
		DeviceMemory(PhysicalDevice* physicalDevice, LogicDevice* logicDevice, VkCommandBuffer transferCmdBuffer, VkQueue queue);
		~DeviceMemory();

		// Ensure mapped access is possible if requested
		void fixAccessType(MemoryAccessType& access, VmaAllocationInfo allocInfo, VkMemoryPropertyFlags memFlags);
		// Choose VmaCreateInfo based on the selected pattern
		VmaAllocationCreateInfo makeVmaCreateInfo(MemoryAllocationPattern pattern, MemoryAccessType& access);

		VmaAllocator allocator_;
		AllocationID availableId_; // 0 reserved for invalid id
		std::map<AllocationID, VmaAllocation> allocations_;

		VkQueue queue_;
		VkCommandBuffer transferCmdBuffer_;
		LogicDevice* logicDevice_;
	};
}
