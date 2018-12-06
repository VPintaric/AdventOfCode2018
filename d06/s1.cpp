#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

struct Vector2D {
    int x, y;

    bool operator<( const Vector2D &other ) const {
        return x < other.x;
    }
};

std::vector< Vector2D > getInput() {
    std::vector< Vector2D > points;
    Vector2D p;

    std::string line;
    while( std::getline( std::cin, line )) {
        std::stringstream ss( line );

        ss >> p.x;
        ss.ignore( 1 ); // ignore comma
        ss >> p.y;

        points.push_back( p );
    }

    return points;
}

bool isOnEdge( const Vector2D &p, const Vector2D &boundFirstPoint, const Vector2D &boundSecondPoint ) {
    return p.x == boundFirstPoint.x || p.y == boundFirstPoint.y || 
            p.x == boundSecondPoint.x || p.y == boundSecondPoint.y;
}

int distance( const Vector2D &p1, const Vector2D &p2 ) {
    return std::abs( p1.x - p2.x ) + std::abs( p1.y - p2.y );
}

int main( void ) {
    auto points = getInput();
    Vector2D max, min;

    max = points.front();
    min = points.front();
    for( const auto &p : points ) {
        max.x = std::max( max.x, p.x );
        max.y = std::max( max.y, p.y );
        min.x = std::min( min.x, p.x );
        min.y = std::min( min.y, p.y );
    }

    std::map< Vector2D, int > tileCounter;
    std::set< Vector2D > pointsOnEdge;
    for( int x = min.x; x <= max.x; ++x ) {
        for( int y = min.y; y <= max.y; ++y ) {
            Vector2D tile { x, y };
            std::vector< Vector2D > nearestPoints { points[ 0 ] };
            auto nearestDistance = distance( tile, points[ 0 ] );

            for( int i = 1; i < points.size(); ++i ) {
                auto dist = distance( tile, points[ i ] );
                if( dist < nearestDistance ) {
                    nearestDistance = dist;
                    nearestPoints.clear();
                    nearestPoints.push_back( points[ i ]);
                } else if( dist == nearestDistance ) {
                    nearestPoints.push_back( points[ i ]); 
                }
            }

            if( nearestPoints.size() == 1 ) {
                ++tileCounter[ nearestPoints[ 0 ]];

                if( isOnEdge( tile, max, min )) {
                    pointsOnEdge.insert( nearestPoints[ 0 ]);
                }
            }
        }
    }

    for( const auto &toRemove : pointsOnEdge ) {
        tileCounter.erase( toRemove );
    }

    int maxTileCount = 0;
    for( const auto &tileCount : tileCounter ) {
        maxTileCount = std::max( maxTileCount, tileCount.second );
    }

    std::cout << maxTileCount << std::endl;

    return 0;
}