#include <iostream>
#include <array>
#include <utility>

constexpr int gridWidth = 300;
constexpr int gridHeight = 300;

constexpr int blockWidth = 3;
constexpr int blockHeight = 3;

using Grid = std::array< int, gridWidth * gridHeight >;

void initialiseGrid( Grid &grid, int gridSerialNumber ) {
    for( int x = 0; x < gridWidth; ++x ) {
        for( int y = 0; y < gridHeight; ++y ) {
            int rackId = (x + 1) + 10;
            int powerLevel = rackId * (y + 1);
            powerLevel += gridSerialNumber;
            powerLevel *= rackId;
            powerLevel = (powerLevel / 100) % 10;
            powerLevel -= 5;

            grid[ x + gridWidth * y ] = powerLevel;
        }
    }
}

int movedRight( int x, int y, int currentValue, const Grid &grid ) {
    for( int i = y; i < y + blockHeight; ++i ) {
        currentValue -= grid[ (x - 1) + i * gridWidth ];
    }

    for( int i = y; i < y + blockHeight; ++i ) {
        currentValue += grid[ (x + blockWidth - 1) + i * gridWidth ];
    }

    return currentValue;
}

int movedDown( int x, int y, int currentValue, const Grid &grid ) {
    for( int i = x; i < x + blockWidth; ++i ) {
        currentValue -= grid[ i + (y - 1) * gridWidth ];
    }

    for( int i = x; i < x + blockWidth; ++i ) {
        currentValue += grid[ i + (y + blockHeight - 1) * gridWidth ];
    }

    return currentValue;
}

int movedUp( int x, int y, int currentValue, const Grid &grid ) {
    for( int i = x; i < x + blockWidth; ++i ) {
        currentValue -= grid[ i + (y + blockHeight) * gridWidth ];
    }

    for( int i = x; i < x + blockWidth; ++i ) {
        currentValue += grid[ i + y * gridWidth ];
    }

    return currentValue;
}

std::pair< int, int > findBestBlock( Grid &grid ) {
    int bestX, bestY;
    int bestValue;

    int currentValue = 0;
    int x = 0, y = 0;
    for( int i = x; i < x + blockWidth; ++i ) {
        for( int j = y; j < y + blockHeight; ++j) {
            currentValue += grid[ i + j * gridWidth ];
        }
    }

    bestValue = currentValue;
    bestX = x;
    bestY = y;

    bool goingDown = true;
    while( true ) {
        if( goingDown ) {
            ++y;
        } else {
            --y;
        }

        if( (y == gridHeight - blockHeight) || (y == -1) ) {
            if( goingDown ) {
                y = gridHeight - blockHeight - 1;
            } else {
                y = 0;
            }

            goingDown = !goingDown;
            ++x;

            if( x == gridWidth - blockWidth ) {
                break;
            }

            currentValue = movedRight( x, y, currentValue, grid );
        } else if( goingDown ) {
            currentValue = movedDown( x, y, currentValue, grid );
        } else {
            currentValue = movedUp( x, y, currentValue, grid );
        }

        if( bestValue < currentValue ) {
            bestValue = currentValue;
            bestX = x;
            bestY = y;
        }
    }

    return std::make_pair( bestX + 1, bestY + 1 );

}

int main( void ) {
    Grid grid;
    int gridSerialNumber;

    std::cin >> gridSerialNumber;

    initialiseGrid( grid, gridSerialNumber );
    auto bestBlockCoordinates = findBestBlock( grid );

    std::cout << bestBlockCoordinates.first << "," << bestBlockCoordinates.second << std::endl;

    return 0;
}