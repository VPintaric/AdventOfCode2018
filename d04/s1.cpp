#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

std::vector< std::string > getInput() {
    std::string line;
    std::vector< std::string > lines;

    while( std::getline( std::cin, line )) {
        lines.push_back( line );
    }

    return lines;
}

struct Period {
    int id;
    int start, end;
};

struct PeriodCompareStart {
    bool operator()( const Period &e1, const Period &e2 ) {
        return e1.start > e2.start;
    }
};

struct PeriodCompareEnd {
    bool operator()( const Period &e1, const Period &e2 ) {
        return e1.end > e2.end;
    }
};

using PeriodStartPQ = std::priority_queue< Period, std::vector< Period >, PeriodCompareStart >;
using PeriodEndPQ = std::priority_queue< Period, std::vector< Period >, PeriodCompareEnd >;

PeriodStartPQ parseRawEntries( const std::vector< std::string > &rawEntries ) {
    std::regex beginShiftExpr( "\\[[^\\]]+\\] Guard #([0-9]+) begins shift$" );
    std::regex fallAsleepExpr( "\\[[^:]+:([0-9][0-9])\\] falls asleep$" );
    std::regex wakesUpExpr( "\\[[^:]+:([0-9][0-9])\\] wakes up$" );

    PeriodStartPQ periods;

    int currentGuardId;
    int currentPeriodStartMinute;
    for( const auto &rawEntry : rawEntries ) {
        std::smatch matchGroup;
        if( std::regex_match( rawEntry, matchGroup, beginShiftExpr )) {
            currentGuardId = std::stoi( matchGroup[1]);
        } else if( std::regex_match( rawEntry, matchGroup, fallAsleepExpr )) {
            currentPeriodStartMinute = std::stoi( matchGroup[1]);
        } else if( std::regex_match( rawEntry, matchGroup, wakesUpExpr )) {
            Period period;
            period.id = currentGuardId;
            period.start = currentPeriodStartMinute;
            period.end = std::stoi( matchGroup[1]);

            periods.push( period );
        }
    }

    return periods;
}

struct GuardInfo {
    int totalMinutesSlept { 0 };
    int nPeriodsSleptCurrentMinute { 0 };

    int maxPeriodsSleptInAMinute { 0 };
    int maxPeriodsSleptMinute { 0 };
};

int main( void ) {
    auto rawEntries = getInput();
    std::sort( rawEntries.begin(), rawEntries.end());

    auto periods = parseRawEntries( rawEntries );
    PeriodEndPQ activePeriods;

    std::map< int, GuardInfo > guardInfos;

    while( !periods.empty()) {
        auto period = periods.top();
        periods.pop();

        activePeriods.push( period );

        auto &guardInfo = guardInfos[ period.id ];

        guardInfo.totalMinutesSlept += period.end - period.start;
        ++guardInfo.nPeriodsSleptCurrentMinute;
        if( guardInfo.maxPeriodsSleptInAMinute < guardInfo.nPeriodsSleptCurrentMinute ) {
            guardInfo.maxPeriodsSleptInAMinute = guardInfo.nPeriodsSleptCurrentMinute;
            guardInfo.maxPeriodsSleptMinute = period.start;
        }

        while( !periods.empty() && !activePeriods.empty() &&
                activePeriods.top().end <= periods.top().start ) {

            auto expired = activePeriods.top();
            activePeriods.pop();
            auto &guardInfo = guardInfos[ expired.id ];
            --guardInfo.nPeriodsSleptCurrentMinute;
        }
    }

    int maxSleptMinutes = 0;
    int sleepiestMinute, sleepiestGuardId;
    for( const auto &guardInfo : guardInfos ) {
        if( guardInfo.second.totalMinutesSlept > maxSleptMinutes ) {
            maxSleptMinutes = guardInfo.second.totalMinutesSlept;
            sleepiestMinute = guardInfo.second.maxPeriodsSleptMinute;
            sleepiestGuardId = guardInfo.first;
        }
    }

    std::cout << sleepiestGuardId * sleepiestMinute << std::endl;

    return 0;
}