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

struct InProgressTask {
    int finishedAt;
    char task;
};

struct TaskCompare {
    bool operator()( const InProgressTask &t1, const InProgressTask &t2 ) {
        if( t1.finishedAt != t2.finishedAt ) {
            return t1.finishedAt > t2.finishedAt;
        } else {
            return t1.task > t2.task;
        }
    }
};

int main( void ) {
    constexpr int nWorkers = 5;
    constexpr int staticTaskTime = 60;

    auto dependencyMap = parseDependencies();
    auto dependencyCounter = generateDependencyCounter( dependencyMap );
    std::priority_queue< char, std::vector< char >, std::greater< char >> availableTasks;
    std::priority_queue< InProgressTask, std::vector< InProgressTask >, TaskCompare > inProgressTasks;
    int currentTime;
    int nCompletedTasks = 0;
    int nTotalTasks = dependencyCounter.size();

    for( const auto &counter : dependencyCounter ) {
        if( counter.second == 0 ) {
            availableTasks.push( counter.first );
        }
    }

    while( nCompletedTasks < nTotalTasks ) {
        while( !availableTasks.empty() && inProgressTasks.size() < nWorkers ) {
            auto task = availableTasks.top();
            availableTasks.pop();

            InProgressTask inProgressTask;
            inProgressTask.finishedAt = currentTime + staticTaskTime + static_cast<int>( task - 'A' + 1 );
            inProgressTask.task = task;

            inProgressTasks.push( inProgressTask );
        }

        do {
            auto doneTask = inProgressTasks.top();
            inProgressTasks.pop();
            currentTime = doneTask.finishedAt;
            ++nCompletedTasks;

            for( const auto &dependantTask : dependencyMap[ doneTask.task ]) {
                auto &counter = dependencyCounter[ dependantTask ];
                --counter;
                if( counter == 0 ) {
                    availableTasks.push( dependantTask );
                }
            }
        } while( !inProgressTasks.empty() && inProgressTasks.top().finishedAt == currentTime );
    }

    std::cout << currentTime << std::endl;

    return 0; }
