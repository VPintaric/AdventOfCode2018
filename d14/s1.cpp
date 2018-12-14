#include <iostream>
#include <vector>

int main( void ) {
    int nRecipes;
    std::cin >> nRecipes;

    std::vector< int > recipes { 3, 7 };
    int firstElf = 0;
    int secondElf = 1;

    while( recipes.size() < nRecipes + 10 ) {
        int sum = recipes[ firstElf ] + recipes[ secondElf ];
        if( sum < 10 ) {
            recipes.push_back( sum );
        } else {
            recipes.push_back( sum / 10 );
            recipes.push_back( sum % 10 );
        }

        firstElf = (firstElf + recipes[ firstElf ] + 1) % recipes.size();
        secondElf = (secondElf + recipes[ secondElf ] + 1) % recipes.size();
    }

    for( int i = nRecipes; i < nRecipes + 10; ++i ) {
        std::cout << recipes[ i ];
    }
    std::cout << std::endl;

    return 0;
}