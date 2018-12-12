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
    prunedState.track = state.track.substr( first, last - first );
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

int main( void ) {
    auto state = getInitialState();

    {
        // skipping the empty line
        std::string _;
        std::getline( std::cin, _ );
    }

    auto rules = parseRules();

    std::vector< std::string > stateHistory;
    std::vector< int > firstIndexHistory;
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
            firstIndexHistory.push_back( prunedState.firstIndex );
            std::cout << prunedState.track << '\n';
            std::cout << prunedState.firstIndex << '\n';
        } else {
            break;
        }

        advanceState( state, rules );
    }

    auto cycleLength = stateHistory.size() - cycleStart;
    auto generationsLeft = 50000000001L - (stateHistory.size() - cycleLength);
    auto nCycles = generationsLeft / cycleLength;
    auto leftOver = generationsLeft % cycleLength;

    auto firstIndexAtStartOfCycle = firstIndexHistory[ cycleStart ];
    auto firstIndexDiffAfterCycle = newCycleStartState.firstIndex - firstIndexAtStartOfCycle;
    auto leftOverDiff = firstIndexHistory[ cycleStart + leftOver ] - firstIndexHistory[ cycleStart ];

    std::cout << '\n';
    std::cout << stateHistory.size() << '\n';
    std::cout << cycleStart << '\n';
    std::cout << cycleLength << '\n';
    std::cout << generationsLeft << '\n';
    std::cout << nCycles << '\n';
    std::cout << leftOver << '\n';
    std::cout << firstIndexAtStartOfCycle << '\n';
    std::cout << firstIndexDiffAfterCycle << '\n';
    std::cout << leftOverDiff << '\n';

    State finalState;
    finalState.track = stateHistory[ cycleStart + leftOver ];
    finalState.firstIndex = firstIndexAtStartOfCycle + nCycles * firstIndexDiffAfterCycle + leftOverDiff;

    std::cout << finalState.track << '\n';
    std::cout << finalState.firstIndex << '\n';

    long sum = 0;
    for( int i = 0; i < finalState.track.size(); ++i ) {
        if( finalState.track[ i ] == '#' ) {
            sum += finalState.firstIndex + i;
        }
    }

    std::cout << sum << std::endl;

    return 0;
}