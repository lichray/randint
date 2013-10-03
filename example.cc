#include "random.h"
#include <iostream>

using stdex::randint;

int main()
{
	stdex::seed_init();
	std::cout << randint(0, 6) << std::endl;
	// std::cout << randint('A', 'G') << std::endl;

	std::cout << randint(0, 6) << std::endl;
	std::cout << randint(-3L, 3L) << std::endl;
}
