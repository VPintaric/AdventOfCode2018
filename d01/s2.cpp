#include <iostream>
#include <set>
#include <vector>

int main( void ) {
	int delta, runningSum = 0;
	std::set< int > seenSums;
	std::vector< int > deltas;

	while( std::cin >> delta ) {
		deltas.push_back( delta );
	}

	int deltaIdx = 0;
	do {
		seenSums.insert( runningSum );
		runningSum += deltas[ deltaIdx ];
		deltaIdx = ( deltaIdx + 1 ) % deltas.size();
	} while( seenSums.find( runningSum ) == seenSums.end() );

	std::cout << runningSum << "\n";

	return 0;
}