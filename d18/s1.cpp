#include <iostream>
#include <string>
#include <vector>

constexpr int gridSideLength = 50;

struct Grid {
    Grid() = default;

    Grid( const Grid &other )
        : data( other.data ), width( other.width ), height( other.height ) {

    }

    char &operator()( int x, int y ) {
        return data[ y ][ x ];
    }

    void print() {
        for( const auto &row : data ) {
            std::cout << row << '\n';
        }
        std::cout << std::endl;
    }

    std::vector< std::string > data;
    int width, height;
};

constexpr char openTile = '.';
constexpr char treeTile = '|';
constexpr char yardTile = '#';

struct AdjacentCounter {
    int nOpenTiles { 0 };
    int nTreeTiles { 0 };
    int nYardTiles { 0 };
};

Grid getGrid() {
    Grid grid;

    grid.data.resize( gridSideLength );

    for( int i = 0; i < gridSideLength; ++i ) {
        std::getline( std::cin, grid.data[ i ] );
    }

    grid.width = grid.height = gridSideLength;

    return grid;
}

void addToCounter( Grid &grid, int x, int y, AdjacentCounter &counter ) {
    if( x < 0 || x >= grid.width || y < 0 || y >= grid.height ) {
        return;
    }

    auto tile = grid( x, y );
    if( tile == openTile ) {
        ++counter.nOpenTiles;
    } else if( tile == treeTile ) {
        ++counter.nTreeTiles;
    } else if( tile == yardTile ) {
        ++counter.nYardTiles;
    }
}

AdjacentCounter countAdjacentTiles( Grid &grid, int x, int y ) {
    AdjacentCounter counter;

    addToCounter( grid, x - 1, y    , counter );
    addToCounter( grid, x - 1, y - 1, counter );
    addToCounter( grid, x    , y - 1, counter );
    addToCounter( grid, x + 1, y - 1, counter );
    addToCounter( grid, x + 1, y    , counter );
    addToCounter( grid, x + 1, y + 1, counter );
    addToCounter( grid, x    , y + 1, counter );
    addToCounter( grid, x - 1, y + 1, counter );

    return counter;
}

Grid advanceTime( Grid &grid ) {
    Grid newGrid( grid );

    for( int y = 0; y < grid.height; ++y ) {
        for( int x = 0; x < grid.width; ++x ) {
            auto tile = grid( x, y );
            auto adjacentTiles = countAdjacentTiles( grid, x, y );

            if( tile == openTile ) {
                if( adjacentTiles.nTreeTiles >= 3 ) {
                    newGrid( x, y ) = treeTile;
                }
            } else if( tile == treeTile ) {
                if( adjacentTiles.nYardTiles >= 3 ) {
                    newGrid( x, y ) = yardTile;
                }
            } else if( tile == yardTile ) {
                if( adjacentTiles.nYardTiles >= 1 && adjacentTiles.nTreeTiles >= 1 ) {
                    newGrid( x, y ) = yardTile;
                } else {
                    newGrid( x, y ) = openTile;
                }
            }
        }
    }

    return newGrid;
}

int main( void ) {
    auto grid = getGrid();

    for( int i = 0; i < 10; ++i ) {
        grid = advanceTime( grid );
    }

    int nYards = 0;
    int nTrees = 0;
    for( int y = 0; y < grid.height; ++y ) {
        for( int x = 0; x < grid.width; ++x ) {
            if( grid(x, y) == treeTile ) {
                ++nTrees;
            } else if( grid(x, y) == yardTile ) {
                ++nYards;
            }
        }
    }

    std::cout << (nTrees * nYards) << std::endl;

    return 0;
}