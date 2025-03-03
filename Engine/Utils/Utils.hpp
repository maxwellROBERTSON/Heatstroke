#pragma once

#include <cstddef>

namespace Utils {

	void* allocAligned(std::size_t size, std::size_t alignment);
	void freeAligned(void* data);

}