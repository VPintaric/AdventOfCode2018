#include <iostream>
#include <array>
#include <utility>

constexpr int gridWidth = 300;
constexpr int gridHeight = 300;

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

int movedRight( int x, int y, int blockSideLength, int currentValue, const Grid &grid ) {
    for( int i = y; i < y + blockSideLength; ++i ) {
        currentValue -= grid[ (x - 1) + i * gridWidth ];
    }

    for( int i = y; i < y + blockSideLength; ++i ) {
        currentValue += grid[ (x + blockSideLength - 1) + i * gridWidth ];
    }

    return currentValue;
}

int movedDown( int x, int y, int blockSideLength, int currentValue, const Grid &grid ) {
    for( int i = x; i < x + blockSideLength; ++i ) {
        currentValue -= grid[ i + (y - 1) * gridWidth ];
    }

    for( int i = x; i < x + blockSideLength; ++i ) {
        currentValue += grid[ i + (y + blockSideLength - 1) * gridWidth ];
    }

    return currentValue;
}

int movedUp( int x, int y, int blockSideLength, int currentValue, const Grid &grid ) {
    for( int i = x; i < x + blockSideLength; ++i ) {
        currentValue -= grid[ i + (y + blockSideLength) * gridWidth ];
    }

    for( int i = x; i < x + blockSideLength; ++i ) {
        currentValue += grid[ i + y * gridWidth ];
    }

    return currentValue;
}

struct BlockInfo {
    int x, y, sideLength, value;
};

BlockInfo findBestBlock( Grid &grid, int blockSideLength ) {
    int bestX, bestY;
    int bestValue;

    int currentValue = 0;
    int x = 0, y = 0;
    for( int i = x; i < x + blockSideLength; ++i ) {
        for( int j = y; j < y + blockSideLength; ++j) {
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

        if( (y == gridHeight - blockSideLength) || (y == -1) ) {
            if( goingDown ) {
                y = gridHeight - blockSideLength - 1;
            } else {
                y = 0;
            }

            goingDown = !goingDown;
            ++x;

            if( x == gridWidth - blockSideLength ) {
                break;
            }

            currentValue = movedRight( x, y, blockSideLength, currentValue, grid );
        } else if( goingDown ) {
            currentValue = movedDown( x, y, blockSideLength, currentValue, grid );
        } else {
            currentValue = movedUp( x, y, blockSideLength, currentValue, grid );
        }

        if( bestValue < currentValue ) {
            bestValue = currentValue;
            bestX = x;
            bestY = y;
        }
    }

    BlockInfo bbi;
    bbi.x = bestX + 1;
    bbi.y = bestY + 1;
    bbi.sideLength = blockSideLength;
    bbi.value = bestValue;
    return bbi;
}

int main( void ) {
    Grid grid;
    int gridSerialNumber;

    std::cin >> gridSerialNumber;

    initialiseGrid( grid, gridSerialNumber );

    auto bestBlockInfo = findBestBlock( grid, 1 );
    for( int blockSideLength = 2; blockSideLength < gridWidth; ++blockSideLength ) {
        auto blockInfo = findBestBlock( grid, blockSideLength );
        if( bestBlockInfo.value < blockInfo.value ) {
            bestBlockInfo = blockInfo;
        }
    }

    std::cout << bestBlockInfo.x << "," << bestBlockInfo.y << "," << bestBlockInfo.sideLength << std::endl;

    return 0;
}