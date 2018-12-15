#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>

constexpr int initialHealth = 200;
constexpr int initialAttack = 3;

struct Grid {
    std::vector< std::string > data;
    int width, height;

    char getAt( int x, int y ) const {
        if( x < 0 || x >= width || y < 0 || y >= height ) {
            return '#';
        } else {
            return data[ y ][ x ];
        }
    }

    void setAt( int x, int y, char c ) {
        if( x < 0 || x >= width || y < 0 || y >= height ) {
            return;
        } else {
            data[ y ][ x ] = c;
        }   
    }

    bool isAdjacentTo( int x, int y, char c ) const {
        return getAt( x, y - 1 ) == c || getAt( x - 1, y ) == c || getAt( x + 1, y ) == c || getAt( x, y + 1 ) == c;
    }

    void print() const {
        for( const auto &row : data ) {
            std::cout << row << '\n';
        }
    }
};

struct Position {
    int x, y;

    bool operator<( const Position &other ) const {
        if( y == other.y ) {
            return x < other.x;
        } else {
            return y < other.y;
        }
    }

    bool operator==( const Position &other ) const {
        return x == other.x && y == other.y;
    }

    bool operator!=( const Position &other ) const {
        return x != other.x || y != other.y;
    }
};

struct Entity {
    enum class Type {
        Elf, Goblin
    };

    Position position;
    int health;
    int attack;
    Type type;

    bool operator<( const Entity &other ) const {
        return position < other.position;
    }
};

char entityTypeToChar( Entity::Type type ) {
    return type == Entity::Type::Elf ? 'E' : 'G';
}

Entity::Type getTargetTypeFor( Entity::Type type ) {
    return type == Entity::Type::Elf ? Entity::Type::Goblin : Entity::Type::Elf;
}

using EntityVector = std::vector< std::unique_ptr< Entity >>;
using PositionEntityMap = std::map< Position, Entity* >;

Grid getGrid() {
    std::string line;
    Grid grid;

    while( std::getline( std::cin, line )) {
        grid.data.push_back( std::move( line ));
    }

    grid.height = grid.data.size();
    grid.width = grid.height > 0 ? grid.data[0].size() : 0;

    return grid;
}

EntityVector findEntities( const Grid &grid, int &nElves, int &nGoblins ) {
    nElves = nGoblins = 0;
    const auto &data = grid.data;
    EntityVector entities;

    for( int y = 0; y < grid.height; ++y ) {
        for( int x = 0; x < grid.width; ++x ) {
            if( data[ y ][ x ] == 'E' || data[ y ][ x ] == 'G' ) {
                auto entity = std::make_unique< Entity >();
                entity->position.x = x;
                entity->position.y = y;
                entity->health = initialHealth;
                entity->attack = initialAttack;
                if( data[ y ][ x ] == 'E' ) {
                    entity->type = Entity::Type::Elf;
                    ++nElves;
                } else {
                    entity->type = Entity::Type::Goblin;
                    ++nGoblins;
                }

                entities.push_back( std::move( entity ));
            }
        }
    }

    return entities;
}

Entity* getAttackTarget( const Grid &grid, PositionEntityMap &positionToEntity, Entity &attacker ) {
    char targetType = entityTypeToChar( getTargetTypeFor( attacker.type ));

    int minHealth = initialHealth + 1;
    Position posToCheck = attacker.position;
    Entity *target = nullptr;

    --posToCheck.y;
    if( grid.getAt( posToCheck.x, posToCheck.y ) == targetType ) {
        auto possibleTarget = positionToEntity[ posToCheck ];
        if( possibleTarget->health < minHealth ) {
            minHealth = possibleTarget->health;
            target = possibleTarget;
        }
    }

    ++posToCheck.y;
    --posToCheck.x;
    if( grid.getAt( posToCheck.x, posToCheck.y ) == targetType ) {
        auto possibleTarget = positionToEntity[ posToCheck ];
        if( possibleTarget->health < minHealth ) {
            minHealth = possibleTarget->health;
            target = possibleTarget;
        }
    }

    posToCheck.x += 2;
    if( grid.getAt( posToCheck.x, posToCheck.y ) == targetType ) {
        auto possibleTarget = positionToEntity[ posToCheck ];
        if( possibleTarget->health < minHealth ) {
            minHealth = possibleTarget->health;
            target = possibleTarget;
        }
    }

    --posToCheck.x;
    ++posToCheck.y;
    if( grid.getAt( posToCheck.x, posToCheck.y ) == targetType ) {
        auto possibleTarget = positionToEntity[ posToCheck ];
        if( possibleTarget->health < minHealth ) {
            minHealth = possibleTarget->health;
            target = possibleTarget;
        }
    }

    return target;
}

void attackTarget( Grid &grid, PositionEntityMap &positionToEntity, Entity &attacker, Entity &target, int &nElves, int &nGoblins ) {
    target.health -= attacker.attack;
    if( target.health <= 0 ) {
        grid.setAt( target.position.x, target.position.y, '.' );
        positionToEntity.erase( target.position );
        if( target.type == Entity::Type::Elf ) {
            --nElves;
        } else {
            --nGoblins;
        }
    }
}

struct Node {
    Position position;
    Position firstMove;

    bool operator<( const Node &other ) const {
        if( position != other.position ) {
            return !(position < other.position);
        } else {
            return !(firstMove < other.firstMove);
        }
    }
};

struct SearchNode {
    int cost;
    Node node;

    bool operator<( const SearchNode &other ) const {
        if( cost != other.cost ) {
            return cost > other.cost;
        } else {
            return node < other.node;
        }
    }
};

std::vector< Position > getNeighbours( const Grid &grid, const Position &position ) {
    auto neighbour = position;
    std::vector< Position > neighbours;

    --neighbour.y;
    if( grid.getAt( neighbour.x, neighbour.y ) == '.' ) {
        neighbours.push_back( neighbour );
    }

    ++neighbour.y;
    --neighbour.x;
    if( grid.getAt( neighbour.x, neighbour.y ) == '.' ) {
        neighbours.push_back( neighbour );
    }

    neighbour.x += 2;
    if( grid.getAt( neighbour.x, neighbour.y ) == '.' ) {
        neighbours.push_back( neighbour );
    }

    --neighbour.x;
    ++neighbour.y;
    if( grid.getAt( neighbour.x, neighbour.y ) == '.' ) {
        neighbours.push_back( neighbour );
    }

    return neighbours;
}

bool isFactionCompletelySurrounded( Grid &grid, PositionEntityMap &positionToEntity, char factionType ) {
    for( const auto &kv : positionToEntity ) {
        if( entityTypeToChar( kv.second->type ) == factionType && grid.isAdjacentTo( kv.first.x, kv.first.y, '.' )) {
            return false;
        }
    }
    return true;
}

void moveEntity( Grid &grid, PositionEntityMap &positionToEntity, Entity &entity ) {
    char entityType = entityTypeToChar( entity.type );
    char targetType = entityTypeToChar( getTargetTypeFor( entity.type ));

    if( grid.isAdjacentTo( entity.position.x, entity.position.y, targetType )) {
        return;
    }

    if( isFactionCompletelySurrounded( grid, positionToEntity, targetType )) {
        return;
    }

    std::priority_queue< SearchNode > nodes;
    std::set< Node > closed;

    auto neighbours = getNeighbours( grid, entity.position );
    for( const auto &neighbour : neighbours ) {
        SearchNode node;
        node.node.position = neighbour;
        node.node.firstMove = neighbour;
        node.cost = 1;

        nodes.push( node );
    }
    Node firstClosed;
    firstClosed.position = entity.position;
    firstClosed.firstMove = entity.position;
    closed.insert( firstClosed );

    Position positionToMoveTo = entity.position;
    while( !nodes.empty()) {
        std::cout << nodes.size() << ':' << closed.size() << ' ';

        auto node = nodes.top();
        nodes.pop();

        if( grid.isAdjacentTo( node.node.position.x, node.node.position.y, targetType )) {
            positionToMoveTo = node.node.firstMove;
            break;
        }

        neighbours = getNeighbours( grid, node.node.position );
        for( const auto &neighbour : neighbours ) {
            Node newNode;
            newNode.position = neighbour;
            newNode.firstMove = node.node.firstMove;
            if( closed.find( newNode ) == closed.end()) {
                SearchNode newSearchNode;
                newSearchNode.node = newNode;
                newSearchNode.cost = node.cost + 1;

                nodes.push( newSearchNode );
            }
        }

        closed.insert( node.node );
    }

    grid.setAt( entity.position.x, entity.position.y, '.' );
    positionToEntity.erase( entity.position );
    entity.position = positionToMoveTo;
    grid.setAt( entity.position.x, entity.position.y, entityType );
    positionToEntity[ entity.position ] = &entity;
}

void diagnosticPrint( Grid &grid, EntityVector &entities, PositionEntityMap &positionToEntity ) {
    std::cout << "================\n";
    grid.print();
    // for( const auto &entity : entities ) {
    //     std::cout << entity.get() << " (" << entity->position.x << " " << entity->position.y << ") HP = " << entity->health << '\n';
    // }

    // for( const auto &kv : positionToEntity ) {
    //     std::cout << "(" << kv.first.x << " " << kv.first.y << ") -> " << kv.second << '\n';
    // }

    std::cout << "================\n" << std::endl;
}

int main( void ) {
    auto grid = getGrid();
    int nElves, nGoblins;
    auto entities = findEntities( grid, nElves, nGoblins );

    PositionEntityMap positionToEntity;
    for( auto &entity : entities ) {
        positionToEntity[ entity->position ] = entity.get();
    }

    // diagnosticPrint( grid, entities, positionToEntity );

    int nRounds = 0;
    bool battleIsOver = false;
    while( !battleIsOver ) {
        std::cout << nRounds << '\n';
        std::sort( entities.begin(), entities.end(), 
            []( const std::unique_ptr< Entity > &e1, const std::unique_ptr< Entity > &e2 ) -> bool {
                return *e1 < *e2;
        });

        for( auto &entity : entities ) {
            if( entity->health <= 0 ) {
                continue;
            }

            std::cout << entity.get() << " (" << entity->position.x << " " << entity->position.y << ") HP = " << entity->health << '\n';
            moveEntity( grid, positionToEntity, *entity );
            if( nElves == 0 || nGoblins == 0 ) {
                battleIsOver = true;
                break;
            }

            auto target = getAttackTarget( grid, positionToEntity, *entity );
            if( target != nullptr ) {
                // std::cout << &entity << " attacking " << target << '\n';
                attackTarget( grid, positionToEntity, *entity, *target, nElves, nGoblins );
            }

            diagnosticPrint( grid, entities, positionToEntity );
            // std::this_thread::sleep_for( std::chrono::milliseconds(300));
        }

        if( !battleIsOver ) {
            ++nRounds;
        }
    }

    int healthSum = 0;
    for( const auto &entity : entities ) {
        healthSum += entity->health > 0 ? entity->health : 0;
    }

    std::cout << nRounds * healthSum << '\n';

    return 0;
}