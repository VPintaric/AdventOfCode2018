#include <iostream>
#include <list>
#include <vector>

using List = std::list< int >;
using Iterator = std::list< int >::iterator;

Iterator moveClockwise( List &list, Iterator iter, int n ) {
    for( int i = 0; i < n; ++i ) {
        ++iter;
        if( iter == list.end()) {
            iter = list.begin();
        }
    }

    return iter;
}

Iterator moveCClockwise( List &list, Iterator iter, int n ) {
    for( int i = 0; i < n; ++i ) {
        if( iter == list.begin()) {
            iter = list.end();
        }
        --iter;
    }

    return iter;
}

int main( void ) {
    int nPlayers, finalMarble;

    std::cin >> nPlayers >> finalMarble;

    std::list< int > marbles;
    marbles.push_front( 0 );
    auto current = marbles.begin();

    int player = 0;
    std::vector< int > playerScores( nPlayers, 0 );

    for( int marble = 1; marble <= finalMarble; ++marble ) {
        if( marble % 23 == 0 ) {
            playerScores[ player ] += marble;
            current = moveCClockwise( marbles, current, 7 );

            playerScores[ player ] += *current;
            auto toBeRemoved = current;
            current = moveClockwise( marbles, current, 1 );
        
            marbles.erase( toBeRemoved );
        } else {
            current = moveClockwise( marbles, current, 2 );
            marbles.insert( current, marble );
            current = moveCClockwise( marbles, current, 1 );
        }

        player = ( player + 1 ) % nPlayers;
    }

    int maxScore = playerScores[ 0 ];
    for( int i = 1; i < playerScores.size(); ++i ) {
        if( maxScore < playerScores[ i ] ) {
            maxScore = playerScores[ i ];
        }
    }

    std::cout << maxScore << std::endl;

    return 0;
}