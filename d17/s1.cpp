#include <iostream>
#include <cstdio>
#include <vector>
#include <memory>
#include <cstring>

constexpr int sourceX = 500;

constexpr char flowingWater = 'F';
constexpr char stillWater   = 'S';
constexpr char clayTile     = '#';
constexpr char sandTile     = '.';

struct Rect {
    int minx, maxx, miny, maxy;
};

struct Grid {
    std::unique_ptr< char[] > data;
    Rect bounds;
    int width, height;
    int sourceX;

    char &operator()( int x, int y ) {
        return data[ x + y * width ];
    }

    void print() const {
        for( int y = 0; y < height; ++y ) {
            for( int x = 0; x < width; ++x ) {
                std::cout << data[ x + y * width];
            }
            std::cout << '\n';
        }
    }
};

Grid getGrid() {
    Grid grid;
    // Screw it
    grid.bounds.minx = 99999;
    grid.bounds.miny = 99999;
    grid.bounds.maxx = -99999;
    grid.bounds.maxy = -99999;

    std::vector< Rect > veins;
    char c;
    while( scanf( "%c", &c ) == 1 ) {
        Rect vein;
        if( c == 'x' ) {
            scanf( "=%d, y=%d..%d\n", &vein.minx, &vein.miny, &vein.maxy );
            vein.maxx = vein.minx;
        } else {
            scanf( "=%d, x=%d..%d\n", &vein.miny, &vein.minx, &vein.maxx );
            vein.maxy = vein.miny;
        }
        veins.push_back( vein );
        grid.bounds.minx = std::min( grid.bounds.minx, vein.minx );
        grid.bounds.miny = std::min( grid.bounds.miny, vein.miny );
        grid.bounds.maxx = std::max( grid.bounds.maxx, vein.maxx );
        grid.bounds.maxy = std::max( grid.bounds.maxy, vein.maxy );
    }

    grid.width = grid.bounds.maxx - grid.bounds.minx + 3;
    grid.height = grid.bounds.maxy - grid.bounds.miny + 2;
    grid.data = std::unique_ptr< char[] >( new char[ grid.width * grid.height ] );

    grid.sourceX = sourceX - grid.bounds.minx + 1;

    std::memset( grid.data.get(), '.', grid.width * grid.height );
    for( const auto &vein : veins ) {
        for( int y = vein.miny; y <= vein.maxy; ++y ) {
            for( int x = vein.minx; x <= vein.maxx; ++x ) {
                grid( x - grid.bounds.minx + 1, y - grid.bounds.miny + 1 ) = clayTile;
            }
        }
    }

    return grid;
}

bool isStaticTile( char c ) {
    return c == clayTile || c == stillWater;
}

bool isDynamicTile( char c ) {
    return c == sandTile || c == flowingWater;
}

bool isWater( char c ) {
    return c == stillWater || c == flowingWater;
}

void flowDown( Grid &grid, int x, int y );
bool flowHorizontal( Grid &grid, int x, int y, int vx ) {
    while( isDynamicTile( grid( x + vx, y )) && isStaticTile( grid( x, y + 1 ))) {
        x += vx;
        grid( x, y ) = flowingWater;
    }

    if( !isStaticTile( grid( x, y + 1 ))) {
        flowDown( grid, x, y );
        return false;
    } else {
        return true;
    }
}

void markAsStillWater( Grid &grid, int x, int y, int vx ) {
    grid( x, y ) = stillWater;
    while( isDynamicTile( grid( x + vx, y ))) {
        x += vx;
        grid( x, y ) = stillWater;
    }
}

void flowDown( Grid &grid, int x, int y ) {
    grid( x, y ) = flowingWater;
    while( y + 1 < grid.height && isDynamicTile( grid( x, y + 1 ))) {
        ++y;
        grid( x, y ) = flowingWater;
    }

    if( y + 1 == grid.height ) {
        return;
    }

    bool flowStoppedToTheRight = flowHorizontal( grid, x, y, 1 );
    bool flowStoppedToTheLeft = flowHorizontal( grid, x, y, -1 );
    if( flowStoppedToTheLeft && flowStoppedToTheRight ) {
        markAsStillWater( grid, x, y,  1 );
        markAsStillWater( grid, x, y, -1 );
        flowDown( grid, x, y - 1 );
    }
}

int main( void ) {
    auto grid = getGrid();

    flowDown( grid, grid.sourceX, 0 );
    grid.print();

    int flowingWaterCounter = 0;
    int stillWaterCounter = 0;
    for( int y = 1; y < grid.height; ++y ) {
        for( int x = 0; x < grid.width; ++x ) {
            switch( grid( x, y )) {
                case flowingWater:
                    ++flowingWaterCounter;
                    break;
                case stillWater:
                    ++stillWaterCounter;
                    break; 
            }
        }
    }

    std::cout << "\nStill water tiles: " << stillWaterCounter << '\n';
    std::cout << "Total water tiles: " << (stillWaterCounter + flowingWaterCounter) << std::endl;

    return 0;
}