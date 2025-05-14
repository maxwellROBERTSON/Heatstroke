#pragma once
#include <iostream>

#ifdef _DEBUG
#define DLOG(x) std::cout << x << std::endl
#else
#define DLOG(x)
#endif
