#include <iostream>
#include <cstdio>
#include <vector>

using LL = long long;

struct Nanobot {
    LL x, y, z, r;
};

std::vector< Nanobot > getNanobots() {
    std::vector< Nanobot > nanobots;

    Nanobot n;
    while( scanf( "pos=<%lld,%lld,%lld>, r=%lld\n", &n.x, &n.y, &n.z, &n.r ) == 4 ) {
        nanobots.push_back( n );
    }

    return nanobots;
}

int dist( const Nanobot &n1, const Nanobot &n2 ) {
    return std::abs( n1.x - n2.x ) + std::abs( n1.y - n2.y ) + std::abs( n1.z - n2.z );
}

int main( void ) {
    auto nanobots = getNanobots();

    auto strongest = &nanobots[ 0 ];
    for( auto &n : nanobots ) {
        if( strongest->r < n.r ) {
            strongest = &n;
        }
    }

    int inRange = 0;
    for( auto &n : nanobots ) {
        if( dist( *strongest, n ) <= strongest->r ) {
            ++inRange;
        }
    }

    std::cout << inRange << std::endl;

    return 0;
}