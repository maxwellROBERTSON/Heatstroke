#include "Error.hpp"

// Credit: COMP5892M (Advanced Rendering)

#include <cstdarg>

namespace Utils {

    Error::Error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buff[1024]{};
        vsnprintf(buff, 1023, fmt, args);

        va_end(args);

        msg = buff;
    }

    const char* Error::what() const noexcept {
        return msg.c_str();
    }

}