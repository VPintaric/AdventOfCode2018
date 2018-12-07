#include <iostream>
#include <regex>
#include <string>
#include <map>
#include <queue>
#include <set>

using DependencyMap = std::map< char, std::set< char >>;

DependencyMap parseDependencies() {
    std::regex expr( "^Step ([A-Z]) must be finished before step ([A-Z]) can begin\\.$" );
    std::string line;

    DependencyMap dependencyMap;
    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

        dependencyMap[ matchGroup[ 1 ].str()[0]].insert( matchGroup[ 2 ].str()[0]);
    }

    return dependencyMap;
}

std::map< char, int > generateDependencyCounter( const DependencyMap &dependencyMap ) {
    std::map< char, int > counter;

    for( const auto &dependencies : dependencyMap ) {
        counter[ dependencies.first ]; // init for each known task
        for( const auto &task : dependencies.second ) {
            ++counter[ task ];
        }
    }

    return counter;
}

int main( void ) {
    auto dependencyMap = parseDependencies();
    auto dependencyCounter = generateDependencyCounter( dependencyMap );
    std::priority_queue< char, std::vector< char >, std::greater< char >> availableTasks;
    std::vector< char > doneTasks;

    for( const auto &counter : dependencyCounter ) {
        if( counter.second == 0 ) {
            availableTasks.push( counter.first );
        }
    }

    while( !availableTasks.empty()) {
        auto task = availableTasks.top();
        availableTasks.pop();

        doneTasks.push_back( task );

        for( const auto &dependantTask : dependencyMap[ task ]) {
            auto &counter = dependencyCounter[ dependantTask ];
            --counter;
            if( counter == 0 ) {
                availableTasks.push( dependantTask );
            }

        }
    }

    for( const auto &task : doneTasks ) {
        std::cout << task;
    }

    std::cout << std::endl;

    return 0;
}