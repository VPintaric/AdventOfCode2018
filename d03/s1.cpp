#include <iostream>
#include <regex>
#include <map>

int main( void ) {
    // e.g.
    // #1 @ 861,330: 20x10
    std::regex expr( "^#[0-9]+ @ ([0-9]+),([0-9]+): ([0-9]+)x([0-9]+)$" );
    std::string line;

    // Unordered map would be better but I'm lazy
    std::map< std::pair< int, int >, int > counter;

    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

        auto x = std::stoi( matchGroup[1] );
        auto y = std::stoi( matchGroup[2] );
        auto w = std::stoi( matchGroup[3] );
        auto h = std::stoi( matchGroup[4] );

        for( int i = 0; i < w; ++i ) {
            for( int j = 0; j < h; ++j ) {
                auto pos = std::make_pair( x + i, y + j );
                ++counter[ pos ];
            }
        }
    }

    int totalArea = 0;
    for( const auto &entry : counter ) {
        if( entry.second > 1 ) {
            ++totalArea;
        }
    }

    std::cout << totalArea << "\n";
 
    return 0;
}