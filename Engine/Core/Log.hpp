#pragma once
#include <iostream>

#ifdef _DEBUG
#define LOG(x) std::cout << x
#else
struct NullBuffer : std::streambuf {
    int overflow(int c) override { return c; }
};
static NullBuffer nullBuffer;
static std::ostream nullStream(&nullBuffer);
#define LOG(x) nullStream
#endif
