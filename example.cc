#include "random.h"
#include <iostream>

using stdex::randint;
using stdex::reseed;

int main()
{
	std::cout << randint(0, 6) << std::endl;
	// std::cout << randint('A', 'G') << std::endl;

	reseed();
	std::cout << randint(0, 6) << std::endl;

	reseed(3);
	std::cout << randint(0, 6) << std::endl;
}
