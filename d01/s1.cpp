#include <iostream>

int main( void ) {
	int delta, runningSum = 0;
	while( std::cin >> delta ) {
		runningSum += delta;
	}

	std::cout << runningSum << "\n";

	return 0;
}