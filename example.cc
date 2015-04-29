#include "random.h"
#include <iostream>

using stdex::randint;

int main()
{
	std::cout << randint(0, 6) << std::endl;
	std::cout << randint(0, 6) << std::endl;
	std::cout << randint<size_t>(0, 10) << std::endl;

	stdex::reseed(0);
	std::cout << randint(3L, 100L) << std::endl;

	stdex::reseed();
	std::cout << randint(3L, 100L) << std::endl;
}
