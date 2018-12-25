#include <iostream>
#include <cstdio>
#include <memory>

using LL = long long;

struct Grid {
    int w, h;
    std::unique_ptr< LL[] > data;

    Grid( int w, int h ) : w( w ), h( h ) {
        data = std::unique_ptr< LL[] >( new LL[ w * h ] );
    }

    LL &operator()( int x, int y ) {
        return data[ x + y * w ];
    }
};

LL erosionLevelForGeoIndex( LL geoIndex, int depth ) {
    return ( geoIndex + depth ) % 20183;
}

Grid getErosionLevelsGrid( int tx, int ty, int depth ) {
    Grid grid( tx + 1, ty + 1 );

    for( int x = 0; x < grid.w; ++x ) {
        for( int y = 0; y < grid.h; ++y ) {
            if(( x == 0 && y == 0 ) || ( x == tx && y == ty )) {
                grid( x, y ) = erosionLevelForGeoIndex( 0, depth );
            } else if( y == 0 ) {
                grid( x, y ) = erosionLevelForGeoIndex( x * 16807, depth );
            } else if( x == 0 ) {
                grid( x, y ) = erosionLevelForGeoIndex( y * 48271, depth );
            } else {
                grid( x, y ) = erosionLevelForGeoIndex( grid( x - 1, y ) * grid( x, y - 1), depth );
            }
        }
    }

    return grid;
}

int main( void ) {
    int depth;
    int tx, ty;

    scanf( "depth: %d\ntarget: %d,%d", &depth, &tx, &ty );

    auto grid = getErosionLevelsGrid( tx, ty, depth );

    LL riskLevel = 0;
    for( int x = 0; x < grid.w; ++x ) {
        for( int y = 0; y < grid.h; ++y ) {
            riskLevel += grid( x, y ) % 3;
        }
    }

    std::cout << riskLevel << std::endl;

    return 0;
}