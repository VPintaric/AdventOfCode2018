#include <iostream>
#include <string>
#include <map>

int main( void ) {
	int withTwoCount = 0;
	int withThreeCount = 0;

	std::string id;
	while( std::cin >> id ) {
		std::map< char, int > charCount {};
		for( const auto &c : id ) {
			++charCount[ c ];
		}

		bool hasTwo = false, hasThree = false;
		for( const auto &p : charCount ) {
			switch( p.second ) {
				case 2:
					hasTwo = true;
					break;
				case 3:
					hasThree = true;
					break;	
			}
		}

		if( hasTwo ) {
			++withTwoCount;
		}
		if( hasThree ) {
			++withThreeCount;
		}
	}

	auto checksum = withTwoCount * withThreeCount;
	std::cout << checksum << '\n';

	return 0;
}