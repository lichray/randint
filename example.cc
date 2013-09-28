#include "random.h"
#include <iostream>

using stdex::randint;
using stdex::randreal;

int main() {
	std::cout << randint(0, 6) << std::endl;
	std::cout << randint('A', 'G') << std::endl;
	std::cout << randreal(0.0, 6.0) << std::endl;
	std::cout << randreal(0.0f, 6.0f) << std::endl;
}
