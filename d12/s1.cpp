#include <iostream>
#include <regex>
#include <list>
#include <string>
#include <sstream>

struct State {
    std::string track;
    int firstIndex { 0 };
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

    // std::cout << "Current state: " << state.track << '\n';
    for( const auto &rule : rules ) {
        auto foundAt = -1;
        while((foundAt = state.track.find( rule.left, foundAt + 1 )) != std::string::npos ) {
            // std::cout << "Found rule " << rule.left << " at " << foundAt << '\n';
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

    for( int i = 0; i < 20; ++i ) {
        advanceState( state, rules );
    }

    int sum = 0;
    for( int i = 0; i < state.track.size(); ++i ) {
        if( state.track[ i ] == '#' ) {
            sum += state.firstIndex + i;
        }
    }

    std::cout << sum << std::endl;

    return 0;
}