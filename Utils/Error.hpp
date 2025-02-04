#pragma once

// Credit: COMP5892M (Advanced Rendering)

#include <string>
#include <exception>

namespace Utils {

    class Error : public std::exception {
    public:
        explicit Error(const char*, ...);

        const char* what() const noexcept override;

    private:
        std::string msg;
    };

}