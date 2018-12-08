#include <iostream>
#include <vector>

int sumMetadata( const std::vector< int > &tree, int &index ) {
    int nChildren = tree[ index++ ];
    int nMetadata = tree[ index++ ];
    int sum = 0;

    for( int child = 0; child < nChildren; ++child ) {
        sum += sumMetadata( tree, index );
    }

    for( int metaEntry = 0; metaEntry < nMetadata; ++metaEntry ) {
        sum += tree[ index++ ];
    }

    return sum;
}

int main( void ) {
    std::vector< int > tree;

    int in;
    while( std::cin >> in ) {
        tree.push_back( in );
    }

    int index = 0;
    auto metadataSum = sumMetadata( tree, index );

    std::cout << metadataSum << std::endl;

    return 0;
}