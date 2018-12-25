#include <iostream>
#include <cstdio>
#include <memory>
#include <queue>
#include <set>
#include <vector>

using LL = long long;

struct Grid {
    int w, h;
    std::unique_ptr< LL[] > data;

    Grid( int w, int h ) : w( w ), h( h ) {
        data = std::unique_ptr< LL[] >( new LL[ w * h ] );
    }

    LL &operator()( int x, int y ) {
        return data[ x + y * w ];
    }
};

enum RegionType {
    Rocky  = 0,
    Wet    = 1,
    Narrow = 2
};

LL erosionLevelForGeoIndex( LL geoIndex, int depth ) {
    return ( geoIndex + depth ) % 20183;
}

Grid getGrid( int w, int h, int tx, int ty, int depth ) {
    Grid grid( w, h );

    for( int x = 0; x < grid.w; ++x ) {
        for( int y = 0; y < grid.h; ++y ) {
            if(( x == 0 && y == 0 ) || ( x == tx && y == ty )) {
                grid( x, y ) = erosionLevelForGeoIndex( 0, depth );
            } else if( y == 0 ) {
                grid( x, y ) = erosionLevelForGeoIndex( x * 16807, depth );
            } else if( x == 0 ) {
                grid( x, y ) = erosionLevelForGeoIndex( y * 48271, depth );
            } else {
                grid( x, y ) = erosionLevelForGeoIndex( grid( x - 1, y ) * grid( x, y - 1), depth );
            }
        }
    }

    for( int x = 0; x < grid.w; ++x ) {
        for( int y = 0; y < grid.h; ++y ) {
            grid( x, y ) %= 3;
        }
    }

    return grid;
}

struct State {
    enum Tool {
        None, Climbing, Torch
    };

    int x, y;
    Tool tool;

    State() : x( 0 ), y( 0 ), tool( Tool::Torch ) { }
    State( int x, int y, Tool tool ) : x( x ), y( y ), tool( tool ) { }

    bool operator<( const State &other ) const {
        if( x != other.x ) {
            return x < other.x;
        } else if( y != other.y ) {
            return y < other.y;
        } else {
            return tool < other.tool;
        }
    }
};

struct Node {
    State state;
    int fscore;
    int distance;

    bool operator<( const Node &other ) const {
        if( fscore != other.fscore ) {
            return fscore > other.fscore;
        }
        if( distance != other.distance ) {
            return distance > other.distance;
        } else {
            return state < other.state;
        }
    }
};

State getInitialState() {
    return State( 0, 0, State::Tool::Torch);
}

State::Tool getOtherEquipableTool( State::Tool currentTool, RegionType type ) {
    if( type == RegionType::Rocky ) {
        if( currentTool == State::Tool::Climbing ) {
            return State::Tool::Torch;
        } else {
            return State::Tool::Climbing;
        }
    } else if( type == RegionType::Wet ) {
        if( currentTool == State::Tool::Climbing ) {
            return State::Tool::None;
        } else {
            return State::Tool::Climbing;
        }
    } else {
        if( currentTool == State::Tool::Torch ) {
            return State::Tool::None;
        } else {
            return State::Tool::Torch;
        }
    }
}

bool canMoveTo( Grid &grid, State::Tool tool, int x, int y ) {
    if( x < 0 || y < 0 ) {
        return false;
    }

    auto type = grid( x, y );

    return !(( tool == State::Tool::None && type == RegionType::Rocky      ) ||
             ( tool == State::Tool::Torch && type == RegionType::Wet       ) ||
             ( tool == State::Tool::Climbing && type == RegionType::Narrow ));
}

std::vector< std::pair< State, int >> getNeighbours( Grid &grid, State s ) {
    std::vector< std::pair< State, int >> neighbours;

    auto otherTool = getOtherEquipableTool( s.tool, static_cast< RegionType >(grid( s.x, s.y )));
    neighbours.push_back( std::make_pair( State( s.x, s.y, otherTool ), 7 ));

    if( canMoveTo( grid, s.tool, s.x - 1, s.y )) {
        neighbours.push_back( std::make_pair( State( s.x - 1, s.y, s.tool ), 1 ));
    }
    if( canMoveTo( grid, s.tool, s.x + 1, s.y )) {
        neighbours.push_back( std::make_pair( State( s.x + 1, s.y, s.tool ), 1 ));
    }
    if( canMoveTo( grid, s.tool, s.x, s.y - 1 )) {
        neighbours.push_back( std::make_pair( State( s.x, s.y - 1, s.tool ), 1 ));
    }
    if( canMoveTo( grid, s.tool, s.x, s.y + 1 )) {
        neighbours.push_back( std::make_pair( State( s.x, s.y + 1, s.tool ), 1 ));
    }

    return neighbours;
}

bool isGoal( const State &s, int tx, int ty ) {
    return s.x == tx && s.y == ty && s.tool == State::Tool::Torch;
}

int heuristic( const State &s, int tx, int ty ) {
    int toolchange = (s.tool == State::Tool::Torch) ? 0 : 7;
    return std::abs( s.x - tx ) + std::abs( s.y - ty ) + toolchange;
}

int search( Grid &grid, int tx, int ty ) {
    Node initialNode;
    initialNode.state = getInitialState();
    initialNode.distance = 0;

    std::priority_queue< Node > open;
    std::set< State > closed;

    open.push( initialNode );

    while( !open.empty()) {
        auto node = open.top();
        open.pop();

        if( closed.find( node.state) != closed.end()) {
            continue;
        }

        if( isGoal( node.state, tx, ty )) {
            return node.distance;
        }

        auto neighbours = getNeighbours( grid, node.state );
        for( const auto &n : neighbours ) {
            if( closed.find( n.first ) == closed.end()) {
                Node newNode;
                newNode.state = n.first;
                newNode.distance = node.distance + n.second;
                newNode.fscore = newNode.distance + heuristic( newNode.state, tx, ty );
                open.push( newNode );
            }
        }

        closed.insert( node.state );
    }

    return -1;
}

int main( void ) {
    int depth;
    int tx, ty;

    scanf( "depth: %d\ntarget: %d,%d", &depth, &tx, &ty );

    auto grid = getGrid( 2000, 2000, tx, ty, depth );

    std::cout << search( grid, tx, ty ) << std::endl;

    return 0;
}