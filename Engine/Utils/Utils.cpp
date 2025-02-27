#include "Utils.hpp"

#include <corecrt_malloc.h>

namespace Utils {

	void* allocAligned(std::size_t size, std::size_t alignment) {
		void* data = nullptr;

#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(size, alignment);
#else
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
#endif
		return data;
	}

	void freeAligned(void* data) {
#if defined(_MSC_VER) || defined(__MINGW32__)
		_aligned_free(data);
#else
		free(data);
#endif
	}

}