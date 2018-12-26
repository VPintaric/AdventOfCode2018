#include <iostream>
#include <cstdio>
#include <set>
#include <vector>
#include <tuple>
#include <list>

struct Point {
    int x, y, z, w;

    bool operator<( const Point &other ) const {
        return std::tie( x, y, z, w ) < 
                std::tie( other.x, other.y, other.z, other.w );
    }
};

int dist( const Point &p1, const Point &p2 ) {
    return std::abs( p1.x - p2.x ) + std::abs( p1.y - p2.y ) + 
            std::abs( p1.z - p2.z ) + std::abs( p1.w - p2.w );
}

std::vector< Point > getPoints() {
    std::vector< Point > points;

    Point p;
    while( std::scanf( "%d,%d,%d,%d\n", &p.x, &p.y, &p.z, &p.w ) == 4) {
        points.push_back( p );
    }

    return points;
}

using Group = std::set< Point >;
using Groups = std::list< Group >;

int main( void ) {
    auto points = getPoints();

    Groups groups;
    for( const auto &p : points ) {
        Groups::iterator belongsTo = groups.end();

        auto iter = groups.begin();
        std::vector< Groups::iterator > shouldMerge;
        while( iter != groups.end()) {
            for( const auto &pInGroup : *iter ) {
                if( dist( p, pInGroup ) <= 3 ) {
                    if( belongsTo == groups.end()) {
                        belongsTo = iter;
                    } else {
                        shouldMerge.push_back( iter );
                    }
                    break;
                }
            }

            iter = std::next( iter );
        }

        if( belongsTo == groups.end()) {
            Group newGroup;
            newGroup.insert( p );
            groups.push_back( newGroup ); 
        } else {
            belongsTo->insert( p );
            for( auto &iter : shouldMerge ) {
                belongsTo->insert( iter->begin(), iter->end());
                groups.erase( iter );
            }
        }
    }

    std::cout << groups.size() << std::endl;

    return 0;
}