#include <iostream>
#include <string>
#include <map>
#include <vector>

struct Vector2D {
    Vector2D() : x( 0 ), y( 0 ) { }

    Vector2D( int x, int y ) : x( x ), y( y ) { }

    bool operator<( const Vector2D &other ) const {
        if( x != other.x ) {
            return x < other.x;
        } else {
            return y < other.y;
        }
    }

    int x, y;
};

struct BranchInfo {
    int openIndex;
    std::vector< int > branchStartIndices;
    int closeIndex;
};

std::map< int, BranchInfo > gatherBranchingInfo( const std::string &directions ) {
    std::map< int, BranchInfo > branchingInfos;

    std::vector< int > currentOpen;
    for( int i = 0; i < directions.size(); ++i ) {
        switch( directions[ i ] ) {
            case '(': {
                branchingInfos[ i ].openIndex = i;
                branchingInfos[ i ].branchStartIndices.push_back( i + 1 );
                currentOpen.push_back( i );
                break;
            }
            case ')': {
                auto openingIndex = currentOpen.back();
                currentOpen.pop_back();
                branchingInfos[ openingIndex ].closeIndex = i;
                break;
            }
            case '|': {
                auto openingIndex = currentOpen.back();
                branchingInfos[ openingIndex ].branchStartIndices.push_back( i + 1 );
                break;
            }
            default:
                break;
        }
    }

    return branchingInfos;
}

struct PosDistance {
    Vector2D p;
    int distance;
};

void move( Vector2D &p, char direction ) {
    switch( direction ) {
        case 'N': --p.y; break;
        case 'S': ++p.y; break;
        case 'W': --p.x; break;
        case 'E': ++p.x; break;
        default: break;
    }
}

struct PosIndex {
    Vector2D p;
    int idx;

    bool operator<( const PosIndex &other ) const {
        if( idx != other.idx ) {
            return idx < other.idx;
        } else {
            return p < other.p;
        }
    }
};

using MemMap = std::map< PosIndex, int >;

std::vector< PosDistance > process( const std::map< int, BranchInfo > &branchingInfos, MemMap &memMap,
                                std::map< Vector2D, int > &pathLengths, const std::string &directions,
                                Vector2D p, int distance, int idx ) {
    if( idx >= directions.size() || directions[ idx ] == '$' ) {
        return {};
    }

    auto iter = memMap.find( {p, idx } );
    if( iter != memMap.end()) {
        if( distance < iter->second ) {
            iter->second = distance;
        } else {
            return {};
        }
    }
    memMap[{ p, idx }] = distance;

    while( idx < directions.size() && directions[ idx ] != '$' ) {
        auto symbol = directions[ idx ];
        if( symbol == '(' ) {
            std::vector< PosDistance > possibilities;
            auto &branchInfo = branchingInfos.find( idx )->second;

            for( const auto &branchStartIdx : branchInfo.branchStartIndices ) {
                auto returnedPossibilities = process( branchingInfos, memMap, pathLengths, directions,
                                                         p, distance, branchStartIdx ); 
                possibilities.insert( possibilities.end(), 
                                        returnedPossibilities.begin(),
                                        returnedPossibilities.end());
            }

            for( const auto &possibility : possibilities ) {
                process( branchingInfos, memMap, pathLengths, directions,
                            possibility.p, possibility.distance, branchInfo.closeIndex + 1 );
            }
        } else if( symbol == ')' || symbol == '|' ) {
            return { { p, distance } };
        } else {
            move( p, symbol );
            ++distance;

            auto iter = pathLengths.find( p );
            if( iter == pathLengths.end()) {
                pathLengths[ p ] = distance;
            } else {
                iter->second = std::min( iter->second, distance );
            }
        }
        ++idx;
    }

    return {};
}

int main( void ) {
    std::string directions;
    std::getline( std::cin, directions );


    auto branchingInfos = gatherBranchingInfo( directions );

    std::map< Vector2D, int > pathLengths;
    MemMap memMap;
    process( branchingInfos, memMap, pathLengths, directions, Vector2D( 0, 0 ), 0, 1 );
    pathLengths[ Vector2D() ] = 0;

    int maxDistance = 0;
    for( const auto &kv : pathLengths ) {
        maxDistance = std::max( maxDistance, kv.second );
    }

    std::cout << maxDistance << std::endl;

    return 0;
}