#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

bool checkMatch( const std::vector< int > &recipes, int index, const std::vector< int > &pattern ) {
    if( recipes.size() < pattern.size()) {
        return false;
    }

    for( int i = 0; i < pattern.size(); ++i ) {
        if( recipes[ index + i ] != pattern[ i ] ) {
            return false;
        }
    }
    return true;
}

int main( void ) {
    std::string patternString;
    std::cin >> patternString;

    std::vector< int > pattern;
    for( auto c : patternString ) {
        pattern.push_back( c - '0' );
    }

    std::vector< int > recipes { 3, 7 };
    int firstElf = 0;
    int secondElf = 1;
    int sizeOfLeftPart;

    while( true ) {
        int sum = recipes[ firstElf ] + recipes[ secondElf ];
        if( sum < 10 ) {
            recipes.push_back( sum );
            if( checkMatch( recipes, recipes.size() - pattern.size(), pattern )) {
                sizeOfLeftPart = recipes.size() - pattern.size();
                break;
            }
        } else {
            recipes.push_back( sum / 10 );
            if( checkMatch( recipes, recipes.size() - pattern.size(), pattern )) {
                sizeOfLeftPart = recipes.size() - pattern.size();
                break;
            }
            recipes.push_back( sum % 10 );
            if( checkMatch( recipes, recipes.size() - pattern.size(), pattern )) {
                sizeOfLeftPart = recipes.size() - pattern.size();
                break;
            }
        }

        firstElf = (firstElf + recipes[ firstElf ] + 1) % recipes.size();
        secondElf = (secondElf + recipes[ secondElf ] + 1) % recipes.size();
    }

    std::cout << sizeOfLeftPart << std::endl;

    return 0;
}