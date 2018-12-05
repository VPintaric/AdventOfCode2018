#include <iostream>
#include <vector>

bool isReaction( char c1, char c2 ) {
    return ( std::islower( c1 ) && std::toupper( c1 ) == c2 ) ||
            ( std::isupper( c1 ) && std::tolower( c1 ) == c2); 
}

int polymerLengthWhenRemoving( const std::vector< char > &polymer, char removed ) {
    std::vector< char > result;

    for( auto c : polymer ) {
        if( !result.empty() && isReaction( c, result.back())) {
            result.pop_back();
        } else if( c != removed && std::tolower( c ) != removed ) {
            result.push_back( c );
        }
    }

    return result.size();
}

int main( void ) {
    std::vector< char > polymer;
    char c;

    while( std::cin >> c ) {
        if( !polymer.empty() && isReaction( c, polymer.back())) {
            polymer.pop_back();
        } else {
            polymer.push_back( c );
        }
    }

    auto minPolymerLength = polymerLengthWhenRemoving( polymer, 'a' );
    for( char removed = 'b'; removed <= 'z'; ++removed ) {
        minPolymerLength = std::min( minPolymerLength, polymerLengthWhenRemoving( polymer, removed ));
    }

    std::cout << minPolymerLength << std::endl;

    return 0;
}