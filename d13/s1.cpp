#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

using Grid = std::vector< std::string >;

struct Position {
    int x, y;

    bool operator<( const Position &other ) const {
        if( y == other.y ) {
            return x < other.x;
        } else {
            return y < other.y;
        }
    }
};

struct Cart {
    enum class Turn {
        Left,
        Straight,
        Right,
    };

    Position pos;
    int vx, vy;
    Turn nextIntersectionTurn;

    bool operator<( const Cart &other ) const {
        return pos < other.pos;
    }
};

Grid getGrid() {
    std::string line;

    Grid grid;

    while( std::getline( std::cin, line )) {
        grid.push_back( line );
    }

    return grid;
}

std::vector< Cart > findCarts( const Grid &grid ) {
    std::vector< Cart > carts;

    for( int y = 0; y < grid.size(); ++y ) {
        for( int x = 0; x < grid[ y ].size(); ++x ) {
            auto tile = grid[ y ][ x ];
            if( tile == '^' || tile == 'v' || tile == '<' || tile == '>' ) {
                Cart cart;
                cart.pos.x = x;
                cart.pos.y = y;
                cart.nextIntersectionTurn = Cart::Turn::Left;

                if( tile == '^' ) {
                    cart.vx = 0;
                    cart.vy = -1;
                } else if( tile == 'v' ) {
                    cart.vx = 0;
                    cart.vy = 1;
                } else if( tile == '<' ) {
                    cart.vx = -1;
                    cart.vy = 0;
                } else if( tile == '>' ) {
                    cart.vx = 1;
                    cart.vy = 0;
                }

                carts.push_back( cart );
            }
        }
    }

    return carts;
}

void turnRight( Cart &cart ) {
    if( cart.vx == -1 ) {
        cart.vx = 0;
        cart.vy = -1;
    } else if( cart.vx == 1 ) {
        cart.vx = 0;
        cart.vy = 1;
    } else if( cart.vy == -1 ) {
        cart.vx = 1;
        cart.vy = 0;
    } else if( cart.vy == 1 ) {
        cart.vx = -1;
        cart.vy = 0;
    }
}

void turnLeft( Cart &cart ) {
    if( cart.vx == -1 ) {
        cart.vx = 0;
        cart.vy = 1;
    } else if( cart.vx == 1 ) {
        cart.vx = 0;
        cart.vy = -1;
    } else if( cart.vy == -1 ) {
        cart.vx = -1;
        cart.vy = 0;
    } else if( cart.vy == 1 ) {
        cart.vx = 1;
        cart.vy = 0;
    }
}

int main( void ) {
    auto grid = getGrid();
    auto carts = findCarts( grid );

    std::set< Position > cartPositions;
    for( const auto &cart : carts ) {
        cartPositions.insert( cart.pos );
    }

    Position collisionPos;
    bool foundCollision = false;
    while( !foundCollision ) {
        std::sort( carts.begin(), carts.end());

        for( auto &cart : carts ) {
            cartPositions.erase( cart.pos );

            cart.pos.x += cart.vx;
            cart.pos.y += cart.vy;

            if( cartPositions.find( cart.pos ) != cartPositions.end()) {
                collisionPos = cart.pos;
                foundCollision = true;
                break;
            }

            cartPositions.insert( cart.pos );

            auto tile = grid[ cart.pos.y ][ cart.pos.x ];

            if( tile == '\\' ) {
                if( cart.vx != 0 ) {
                    turnRight( cart );
                } else {
                    turnLeft( cart );
                }
            } else if( tile == '/' ) {
                if( cart.vx != 0 ) {
                    turnLeft( cart );
                } else {
                    turnRight( cart );
                }
            } else if( tile == '+' ) {
                switch( cart.nextIntersectionTurn ) {
                    case Cart::Turn::Left:
                        turnLeft( cart );
                        cart.nextIntersectionTurn = Cart::Turn::Straight;
                        break;
                    case Cart::Turn::Straight:
                        cart.nextIntersectionTurn = Cart::Turn::Right;
                        break;
                    case Cart::Turn::Right:
                        turnRight( cart );
                        cart.nextIntersectionTurn = Cart::Turn::Left;
                        break;
                }
            }
        }
    }

    std::cout << collisionPos.x << "," << collisionPos.y << '\n';

    return 0;
}