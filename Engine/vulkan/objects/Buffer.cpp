#include "Buffer.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "../VulkanContext.hpp"

namespace Engine {
namespace vk {

	Buffer::~Buffer() {
		if (buffer != VK_NULL_HANDLE) {
			assert(mAllocator != VK_NULL_HANDLE);
			assert(allocation != VK_NULL_HANDLE);
			vmaDestroyBuffer(mAllocator, buffer, allocation);
		}
	}

	Buffer::Buffer(VmaAllocator aAllocator, VkBuffer aBuffer, VmaAllocation aAllocation) noexcept
		: buffer(aBuffer)
		, allocation(aAllocation)
		, mAllocator(aAllocator)
	{}

	Buffer::Buffer(Buffer&& aOther) noexcept
		: buffer(std::exchange(aOther.buffer, VK_NULL_HANDLE))
		, allocation(std::exchange(aOther.allocation, VK_NULL_HANDLE))
		, mAllocator(std::exchange(aOther.mAllocator, VK_NULL_HANDLE))
	{}

	Buffer& Buffer::operator=(Buffer&& aOther) noexcept {
		std::swap(buffer, aOther.buffer);
		std::swap(allocation, aOther.allocation);
		std::swap(mAllocator, aOther.mAllocator);
		return *this;
	}

	Buffer createBuffer(const VulkanAllocator& aAllocator, VkDeviceSize aDeviceSize, VkBufferUsageFlags aUsageFlags, VmaAllocationCreateFlags aCreateFlags, VmaMemoryUsage aMemoryUsage) {		
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = aDeviceSize;
		bufferInfo.usage = aUsageFlags;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.flags = aCreateFlags;
		allocInfo.usage = aMemoryUsage;

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		if (const auto res = vmaCreateBuffer(aAllocator.allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr); VK_SUCCESS != res)
			throw Utils::Error("Unable to allocate buffer\n vmaCreateBuffer() returned %s", Utils::toString(res).c_str());

		return Buffer(aAllocator.allocator, buffer, allocation);
	}

}
}