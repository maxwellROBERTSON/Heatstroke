#include "test.h"
#include "../third_party/TestLib/testLib.h"

#include <iostream>

test::test()
{
	num = 5;
	num *= PI;
	std::cout << "IT WORKED " << addLOL(5, num) << std::endl;
}