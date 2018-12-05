#include <iostream>
#include <stack>

bool isReaction( char c1, char c2 ) {
    return ( std::islower( c1 ) && std::toupper( c1 ) == c2 ) ||
            ( std::isupper( c1 ) && std::tolower( c1 ) == c2); 
}

int main( void ) {
    std::stack< char > polymer;
    char c;

    while( std::cin >> c ) {
        if( !polymer.empty() && isReaction( c, polymer.top())) {
            polymer.pop();
        } else {
            polymer.push( c );
        }
    }

    std::cout << polymer.size() << std::endl;

    return 0;
}