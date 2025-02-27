#include "Buffer.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "../VulkanContext.hpp"

namespace Engine {
namespace vk {

	Buffer::~Buffer() {
		if (buffer != VK_NULL_HANDLE) {
			//std::fprintf(stdout, "Destroyed buffer: %s\n", name.c_str());
			assert(mAllocator != VK_NULL_HANDLE);
			assert(allocation != VK_NULL_HANDLE);
			vmaDestroyBuffer(mAllocator, buffer, allocation);
		}
	}

	Buffer::Buffer(VmaAllocator aAllocator, std::string name, VkBuffer aBuffer, VmaAllocation aAllocation) noexcept
		: buffer(aBuffer)
		, name(name)
		, allocation(aAllocation)
		, mAllocator(aAllocator)
	{
		//std::fprintf(stdout, "Created buffer: %s - %p\n", name.c_str(), aBuffer);
	}

	Buffer::Buffer(Buffer&& aOther) noexcept
		: buffer(std::exchange(aOther.buffer, VK_NULL_HANDLE))
		, name(std::exchange(aOther.name, ""))
		, allocation(std::exchange(aOther.allocation, VK_NULL_HANDLE))
		, mAllocator(std::exchange(aOther.mAllocator, VK_NULL_HANDLE))
	{}

	Buffer& Buffer::operator=(Buffer&& aOther) noexcept {
		std::swap(buffer, aOther.buffer);
		std::swap(name, aOther.name);
		std::swap(allocation, aOther.allocation);
		std::swap(mAllocator, aOther.mAllocator);
		return *this;
	}


	Buffer createBuffer(std::string name, const VulkanAllocator& aAllocator, VkDeviceSize aDeviceSize, VkBufferUsageFlags aUsageFlags, VmaAllocationCreateFlags aCreateFlags, VmaMemoryUsage aMemoryUsage) {		
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

		return Buffer(aAllocator.allocator, name, buffer, allocation);
	}

}
}