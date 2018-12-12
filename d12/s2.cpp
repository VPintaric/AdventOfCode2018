#include <iostream>
#include <regex>
#include <list>
#include <string>
#include <sstream>
#include <set>

struct State {
    std::string track;
    long firstIndex { 0 };
};

struct Rule {
    std::string left;
    char right;
};

void padIfNeeded( State &state ) {
    int first = 0;
    while( first < 4 && state.track[ first ] != '#' ) {
        ++first;
    }
    state.firstIndex -= 4 - first;
    state.track.insert( 0, 4 - first, '.' );

    int last = 0;
    while( last < 4 && state.track[ state.track.size() - last - 1 ] != '#' ) {
        ++last;
    }
    state.track.insert( state.track.size(), 4 - last, '.' );
}

State prunePadding( const State &state ) {
    auto first = state.track.find_first_of( '#' );
    auto last = state.track.find_last_of( '#' );

    State prunedState;
    prunedState.track = state.track.substr( first, last - first + 1 );
    prunedState.firstIndex = state.firstIndex + first;

    return prunedState;
}

State getInitialState() {
    std::regex expr( "initial state: ([#\\.]+)" );
    std::string line;

    std::getline( std::cin, line );

    std::smatch matchGroup;
    std::regex_match( line, matchGroup, expr );

    State state;
    state.track = matchGroup[ 1 ].str();

    padIfNeeded( state );

    return state;
}

std::vector< Rule > parseRules() {
    std::regex expr( "([#\\.]+) => ([#\\.])" );
    std::string line;

    std::vector< Rule > rules;

    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

        Rule rule;
        rule.left = matchGroup[ 1 ].str();
        rule.right = matchGroup[ 2 ].str()[ 0 ];

        rules.push_back( std::move( rule ));
    }

    return rules;
}

void advanceState( State &state, std::vector< Rule > &rules ) {
    std::string newTrack( state.track );

    for( const auto &rule : rules ) {
        auto foundAt = -1;
        while((foundAt = state.track.find( rule.left, foundAt + 1 )) != std::string::npos ) {
            newTrack[ foundAt + 2 ] = rule.right;
        }
    }

    state.track = std::move( newTrack );
    padIfNeeded( state );
}

long calculateSum( const State &state ) {
    long sum = 0;
    for( int i = 0; i < state.track.size(); ++i ) {
        if( state.track[ i ] == '#' ) {
            sum += state.firstIndex + i;
        }
    }

    return sum;
}

int main( void ) {
    auto state = getInitialState();

    {
        // skipping the empty line
        std::string _;
        std::getline( std::cin, _ );
    }

    auto rules = parseRules();

    std::vector< std::string > stateHistory;
    std::vector< int > sumHistory;
    int cycleStart = -1;

    State newCycleStartState;

    while( true ) {
        auto prunedState = prunePadding( state );

        for( int i = 0; i < stateHistory.size(); ++i ) {
            if( stateHistory[ i ].compare( prunedState.track ) == 0 ) {
                cycleStart = i;
                newCycleStartState = std::move( prunedState );
                break;
            }
        }

        if( cycleStart == -1 ) {
            stateHistory.push_back( prunedState.track );
            sumHistory.push_back( calculateSum( prunedState ));
        } else {
            break;
        }

        advanceState( state, rules );
    }

    auto cycleLength = stateHistory.size() - cycleStart;
    auto generationsLeft = 50000000000L - (stateHistory.size() - cycleLength);
    auto nCycles = generationsLeft / cycleLength;
    auto leftOver = generationsLeft % cycleLength;

    auto sumAtStartOfCycle = sumHistory[ cycleStart ];
    auto sumDiffAfterCycle = calculateSum( newCycleStartState ) - sumAtStartOfCycle;
    auto sumLeftOverDiff = sumHistory[ cycleStart + leftOver ] - sumHistory[ cycleStart ];

    auto finalSum = sumAtStartOfCycle + sumDiffAfterCycle * nCycles + sumLeftOverDiff;

    std::cout << finalSum << '\n';

    return 0;
}