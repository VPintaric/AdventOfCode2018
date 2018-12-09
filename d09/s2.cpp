#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

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

std::string bigIntAdd( const std::string &x, int n ) {
    auto y = std::to_string( n );
    std::reverse( y.begin(), y.end());

    std::stringstream result;

    int carry = 0;
    int until = std::max( x.size(), y.size());
    for( int i = 0; i < until; ++i ) {
        int fromX = static_cast< int >( i < x.size() ? x.at( i ) - '0' : 0 );
        int fromY = static_cast< int >( i < y.size() ? y.at( i ) - '0' : 0 );

        int s = fromX + fromY + carry;
        carry = s / 10;
        s = s % 10;

        result << s;
    }

    if( carry > 0 ) {
        result << carry;
    }

    return result.str();
}

bool bigIntLessThan( const std::string &x, const std::string &y ) {
    if( x.size() < y.size()) {
        return true;
    } else if( x.size() > y.size()) {
        return false;
    } else {
        auto iterx = x.rbegin();
        auto itery = y.rbegin();

        while( iterx != x.rend()) {
            if( *iterx < *itery ) {
                return true;
            } else if( *iterx > *itery ) {
                return false;
            }
            ++iterx;
            ++itery;
        }

        return false;
    }
}

int main( void ) {
    int nPlayers, finalMarble;

    std::cin >> nPlayers >> finalMarble;
    finalMarble *= 100;

    std::list< int > marbles;
    marbles.push_front( 0 );
    auto current = marbles.begin();

    int player = 0;
    std::vector< std::string > playerScores( nPlayers, "0" );

    for( int marble = 1; marble <= finalMarble; ++marble ) {
        if( marble % 23 == 0 ) {
            int addToScore = marble;
            current = moveCClockwise( marbles, current, 7 );

            addToScore += *current;
            auto toBeRemoved = current;
            current = moveClockwise( marbles, current, 1 );
        
            marbles.erase( toBeRemoved );

            playerScores[ player ] = bigIntAdd( playerScores[ player ], addToScore );
        } else {
            current = moveClockwise( marbles, current, 2 );
            marbles.insert( current, marble );
            current = moveCClockwise( marbles, current, 1 );
        }

        player = ( player + 1 ) % nPlayers;
    }

    std::string maxScore = playerScores[ 0 ];
    for( int i = 1; i < playerScores.size(); ++i ) {
        if( bigIntLessThan( maxScore, playerScores[ i ] ) ) {
            maxScore = playerScores[ i ];
        }
    }

    std::reverse( maxScore.begin(), maxScore.end());
    std::cout << maxScore << std::endl;

    return 0;
}