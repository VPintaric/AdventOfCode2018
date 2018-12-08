#include <iostream>
#include <vector>

int sumMetadata( const std::vector< int > &tree, int &index ) {
    int nChildren = tree[ index++ ];
    int nMetadata = tree[ index++ ];
    std::vector< int > childrenValues;

    for( int child = 0; child < nChildren; ++child ) {
        childrenValues.push_back( sumMetadata( tree, index ));
    }

    int sum = 0;
    if( childrenValues.empty()) {
        for( int metaEntry = 0; metaEntry < nMetadata; ++metaEntry ) {
            sum += tree[ index++ ];
        }
    } else {
        for( int metaEntry = 0; metaEntry < nMetadata; ++metaEntry ) {
            int childIndex = tree[ index++ ] - 1;
            if( childIndex >= 0 && childIndex < childrenValues.size()) {
                sum += childrenValues[ childIndex ];
            }
        }
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