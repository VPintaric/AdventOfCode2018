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

    constexpr int distanceSumLimit = 10000;
    int nTilesInRegion = 0;
    for( int x = min.x; x <= max.x; ++x ) {
        for( int y = min.y; y <= max.y; ++y ) {
            Vector2D tile { x, y };
            int distanceSum = 0;

            for( const auto &p : points ) {
                distanceSum += distance( tile, p );
            }

            if( distanceSum < distanceSumLimit ) {
                ++nTilesInRegion;
            }
        }
    }

    std::cout << nTilesInRegion << std::endl;

    return 0;
}