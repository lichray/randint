#include "random.h"
#include <iostream>

using stdex::randint;

int main()
{
	std::cout << randint<false>(0, 6) << std::endl;
	// std::cout << randint('A', 'G') << std::endl;

	std::cout << randint(0, 6) << std::endl;
	std::cout << randint(-3L, 3L) << std::endl;
}
