#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <algorithm>
#include <queue>

enum class DamageType {
    Fire,
    Radiation,
    Slashing,
    Bludgeoning,
    Cold
};

std::map< std::string, DamageType > damageTypes {
    { "fire", DamageType::Fire },
    { "radiation", DamageType::Radiation },
    { "slashing", DamageType::Slashing },
    { "bludgeoning", DamageType::Bludgeoning },
    { "cold", DamageType::Cold }
};

struct Group {
    int nUnits, unitHealth, unitAttack, initiative;
    DamageType attackType;
    std::set< DamageType > weaknesses, immunities;

    int getEffectiveAttack() const {
        return nUnits * unitAttack;
    }

    int getTotalHp() const {
        return nUnits * unitHealth;
    }

    bool isWeakTo( DamageType type ) {
        return weaknesses.find( type ) != weaknesses.end();
    }

    bool isImmuneTo( DamageType type ) {
        return immunities.find( type ) != immunities.end();
    }

    bool operator<( const Group &other ) const {
        auto effAtt = getEffectiveAttack();
        auto oEffAtt = other.getEffectiveAttack();
        if( effAtt != oEffAtt ) {
            return effAtt > oEffAtt;
        } else {
            return initiative > other.initiative;
        }
    }
};

using Army = std::vector< Group >;

struct BattleState {
    Army immuneSysArmy;
    Army infectionArmy;
};

std::set< DamageType > parseDamageTypes( std::string &&s ) {
    std::set< DamageType > parsed;

    if( !s.empty()) {
        int lastFound = 0, next = 0;
        while(( next = s.find( ", ", next )) != std::string::npos ) {
            parsed.insert( damageTypes[ s.substr( lastFound, next - lastFound ) ]);
            next += 2;
            lastFound = next;
        }
        parsed.insert( damageTypes[ s.substr( lastFound ) ]);
    }

    return parsed;
}

Group parseGroup( std::string &line ) {
    Group group;

    std::smatch match;
    std::regex r( R"__(^(\d+) units each with (\d+) hit points (?:\(([a-z, ]+)(?:; ([a-z, ]+))?\) )?with an attack that does (\d+) ([a-z]+) damage at initiative (\d+)$)__" );

    std::regex_search( line, match, r );

    group.nUnits = std::stoi( match[ 1 ].str());
    group.unitHealth = std::stoi( match[ 2 ].str());

    auto attributes = match[ 3 ].str();
    if( !attributes.empty() && attributes.compare( 0, 4, "weak" ) == 0 ) {
        group.weaknesses = parseDamageTypes( attributes.substr( 8 ));
    } else if( !attributes.empty() && attributes.compare( 0, 6, "immune" ) == 0 ) {
        group.immunities = parseDamageTypes( attributes.substr( 10 ));
    }

    attributes = match[ 4 ].str();
    if( !attributes.empty() && attributes.compare( 0, 4, "weak" ) == 0 ) {
        group.weaknesses = parseDamageTypes( attributes.substr( 8 ));
    } else if( !attributes.empty() && attributes.compare( 0, 6, "immune" ) == 0 ) {
        group.immunities = parseDamageTypes( attributes.substr( 10 ));
    }

    group.unitAttack = std::stoi( match[ 5 ].str());
    group.attackType = damageTypes[ match[ 6 ].str()];
    group.initiative = std::stoi( match[ 7 ].str());

    return group;
}

BattleState getInitialState() {
    BattleState state;

    std::string line;
    std::getline( std::cin, line ); // "Immune System:\n"
    while( std::getline( std::cin, line )) {
        if( line.empty()) {
            break;
        }
        state.immuneSysArmy.push_back( parseGroup( line ));
    }

    std::getline( std::cin, line ); // "Infection:\n"
    while( std::getline( std::cin, line )) {
        state.infectionArmy.push_back( parseGroup( line ));
    }

    return state;
}

struct AttackInfo {
    Group *attacker, *target;

    AttackInfo( Group *attacker, Group *target )
            : attacker( attacker ), target( target ) {

    }

    AttackInfo( const AttackInfo &other ) 
            : attacker( other.attacker ), target( other.target ) {

    }

    AttackInfo &operator=( const AttackInfo &other ) {
        if( this != &other ) {
            attacker = other.attacker;
            target = other.target;
        }
    }

    bool operator<( const AttackInfo &other ) const {
        return attacker->initiative < other.attacker->initiative;
    }
};

using PlannedAttacks = std::priority_queue< AttackInfo >;

void chooseTargets( Army &army, Army &targetArmy, PlannedAttacks &plans ) {
    std::set< int > takenTargetsIndices;

    for( auto &attacker : army ) {
        int bestTargetIndex = -1;
        int bestPotentialDamage = 0;
        for( int i = 0; i < targetArmy.size(); ++i ) {
            auto &target = targetArmy[ i ];
            if( target.isImmuneTo( attacker.attackType ) || 
                    takenTargetsIndices.find( i ) != takenTargetsIndices.end()) {
                continue;
            }

            int potentialDamage = attacker.getEffectiveAttack();
            potentialDamage *= target.isWeakTo( attacker.attackType ) ? 2 : 1;

            if( bestTargetIndex == -1 ) {
                bestTargetIndex = i;
                bestPotentialDamage = potentialDamage;
                continue;
            }

            auto &cTarget = targetArmy[ bestTargetIndex ];

            if( bestPotentialDamage < potentialDamage ||
                ( bestPotentialDamage == potentialDamage &&
                  cTarget.getEffectiveAttack() < target.getEffectiveAttack()) || 
                    bestPotentialDamage == potentialDamage &&
                    cTarget.getEffectiveAttack() == target.getEffectiveAttack() &&
                    cTarget.initiative < target.initiative ) {
                bestPotentialDamage = potentialDamage;
                bestTargetIndex = i;
            }
        }

        if( bestTargetIndex != -1 ) {
            takenTargetsIndices.insert( bestTargetIndex );
            plans.push( AttackInfo( &attacker, &targetArmy[ bestTargetIndex ])); 
        }
    }
}

void executeAttacks( PlannedAttacks &plans ) {
    while( !plans.empty()) {
        auto p = plans.top();
        plans.pop();

        if( p.attacker->nUnits == 0 ) {
            continue;
        }

        auto damage = p.attacker->getEffectiveAttack();
        damage *= p.target->isWeakTo( p.attacker->attackType ) ? 2 : 1;

        auto totalHp = p.target->getTotalHp() - damage;
        totalHp = totalHp < 0 ? 0 : totalHp;

        p.target->nUnits = ( totalHp / p.target->unitHealth ) + ( totalHp % p.target->unitHealth ? 1 : 0 );
    }
}

void removeDead( Army &army ) {
    Army cleansedArmy;

    for( auto &group : army ) {
        if( group.nUnits > 0 ) {
            cleansedArmy.push_back( group );
        }
    }

    army = std::move( cleansedArmy );
}

void simulate( BattleState &battleState ) {
    while( !battleState.immuneSysArmy.empty() && !battleState.infectionArmy.empty()) {
        std::sort( battleState.immuneSysArmy.begin(), battleState.immuneSysArmy.end());
        std::sort( battleState.infectionArmy.begin(), battleState.infectionArmy.end());

        PlannedAttacks plannedAttacks;
        chooseTargets( battleState.immuneSysArmy, battleState.infectionArmy, plannedAttacks );
        chooseTargets( battleState.infectionArmy, battleState.immuneSysArmy, plannedAttacks );

        if( plannedAttacks.empty()) {
            // stalemate
            break;
        }
        executeAttacks( plannedAttacks );

        removeDead( battleState.immuneSysArmy );
        removeDead( battleState.infectionArmy );
    }
}

BattleState giveBoost( const BattleState &state, int boost ) {
    BattleState boostedState = state;

    for( auto &group : boostedState.immuneSysArmy ) {
        group.unitAttack += boost;
    }

    return boostedState;
}

int main( void ) {
    auto initialState = getInitialState();

    int lo = 0, hi = 1000000;
    while( lo < hi ) {
        int boost = ( lo + hi ) / 2;
        auto state = giveBoost( initialState, boost );
        simulate( state );

        if( !state.immuneSysArmy.empty() && state.infectionArmy.empty()) {
            hi = boost;
        } else {
            lo = boost + 1;
        }
    }

    std::cout << "Needed boost: " << lo << '\n';
    auto state = giveBoost( initialState, lo );
    simulate( state );

    int totalUnits = 0;
    for( auto const &group : state.immuneSysArmy ) {
        totalUnits += group.nUnits;
    }

    std::cout << "Remaining units: " << totalUnits << std::endl;

    return 0;
}