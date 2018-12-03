#include <iostream>
#include <regex>
#include <algorithm>
#include <vector>
#include <unordered_set>

template< class T, class Comparator >
class IterablePQueue {
public:
    void push( T val ) {
        c.push_back( val );
        std::push_heap( c.begin(), c.end(), comparator );
    }

    T pop() {
        std::pop_heap( c.begin(), c.end(), comparator );
        auto val = c.back();
        c.pop_back();
        return val;
    }

    T front() {
        return c.front();
    }

    bool empty() {
        return c.empty();
    }

    Comparator comparator;
    std::vector< T > c;
};

struct Block {
    int id, left, right, down, up;
};

class CompareStartPoints {
public:
    bool operator()( const Block &b1, const Block &b2 ) {
        return b1.left > b2.left;
    }
};

class CompareEndPoints {
public:
    bool operator()( const Block &b1, const Block &b2 ) {
        return b1.right > b2.right;
    }
};

using PQueueComparingStartPoints = IterablePQueue< Block, CompareStartPoints >;
using PQueueComparingEndPoints   = IterablePQueue< Block, CompareEndPoints >;

PQueueComparingStartPoints parseInput() {
    // e.g.
    // #1 @ 861,330: 20x10
    std::regex expr( "^#([0-9]+) @ ([0-9]+),([0-9]+): ([0-9]+)x([0-9]+)$" );
    std::string line;

    PQueueComparingStartPoints blocks;

    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

        auto id = std::stoi( matchGroup[1] );
        auto x  = std::stoi( matchGroup[2] );
        auto y  = std::stoi( matchGroup[3] );
        auto w  = std::stoi( matchGroup[4] );
        auto h  = std::stoi( matchGroup[5] );

        blocks.push({ 
            id, x, x + w, y, y + h
        });
    }

    return blocks;
}

bool isOverlapOnYAxis( const Block &b1, const Block &b2 ) {
    return !( b1.down > b2.up || b2.down > b1.up );
}

int main( void ) {
    auto blocks = parseInput();
    std::unordered_set< int > noOverlapBlockIds;

    PQueueComparingEndPoints activeBlocks;
    auto firstBlock = blocks.pop();
    activeBlocks.push( firstBlock );
    noOverlapBlockIds.insert( firstBlock.id );

    while( !blocks.empty()) {
        auto block = blocks.pop();
        while( block.left > activeBlocks.front().right ) {
            activeBlocks.pop();
        }

        noOverlapBlockIds.insert( block.id );

        for( const auto &b : activeBlocks.c ) {
            if( isOverlapOnYAxis( b, block )) {
                noOverlapBlockIds.erase( b.id );
                noOverlapBlockIds.erase( block.id );
            }
        }

        activeBlocks.push( block );
    }

    std::cout << *noOverlapBlockIds.begin() << std::endl;
 
    return 0;
} 