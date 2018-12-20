#include <iostream>
#include <string>
#include <vector>

#include <thread>
#include <chrono>

constexpr int gridSideLength = 50;
constexpr int totalTime = 1000000000;

struct Grid {
    Grid() = default;

    Grid( const Grid &other )
        : data( other.data ), width( other.width ), height( other.height ) {

    }

    Grid( Grid &&other ) 
        : data( std::move( other.data )), width( other.width ), height( other.height ) {
        other.width = other.height = 0;
    }

    Grid &operator=( Grid &&other ) {
        if( this != &other ) {
            data = std::move( other.data );
            width = other.width;
            height = other.height;
            other.width = other.height = 0;
        }
        return *this;
    }

    char &operator()( int x, int y ) {
        return data[ y ][ x ];
    }

    bool operator==( const Grid &other ) {
        if( width != other.width || height != other.height ) {
            return false;
        }

        for( int row = 0; row < height; ++row ) {
            if( data[ row ].compare( other.data[ row ] ) != 0  ) {
                return false;
            }
        }

        return true;
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
    std::vector< Grid > gridHistory;

    auto grid = getGrid();
    int cycleIndexStart = -1;
    bool foundCycle = false;
    while( true ) {
        for( int i = 0; i < gridHistory.size(); ++i ) {
            if( gridHistory[ i ] == grid ) {
                cycleIndexStart = i;
                foundCycle = true;
                break;
            }
        }
        if( foundCycle ) {
            break;
        }

        auto nextGrid = advanceTime( grid );

        std::cout << "Time: " << gridHistory.size() << '\n';
        grid.print();
        std::this_thread::sleep_for (std::chrono::milliseconds(16));

        gridHistory.push_back( std::move( grid ));
        grid = std::move( nextGrid );
    }

    int cycleLength = gridHistory.size() - cycleIndexStart;
    int remainingTime = totalTime - cycleIndexStart;
    int finalStateIndex = cycleIndexStart + remainingTime % cycleLength;

    std::cout << cycleLength << ' ' << remainingTime << ' ' << cycleIndexStart << ' ' << (remainingTime % cycleLength) << ' ' << finalStateIndex << '\n';

    grid = std::move( gridHistory[ finalStateIndex ] );

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