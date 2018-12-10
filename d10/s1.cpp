#include <iostream>
#include <vector>
#include <array>
#include <regex>
#include <thread>
#include <chrono>

constexpr int screenWidth = 80;
constexpr int screenHeight = 40;

constexpr int animationSteps = 10;
constexpr int animationFrameLengthMs = 500;

int viewportX = 0;
int viewportY = 0;

std::array< char, screenWidth * screenHeight > screen;

struct Point {
    int x, y;
    int vx, vy;
};

std::vector< Point > parseInput() {
    std::regex expr( "position=< *(-?[0-9]+), *(-?[0-9]+)> velocity=< *(-?[0-9]+), *(-?[0-9]+)>" );
    std::string line;

    std::vector< Point > points;

    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

        if( matchGroup.size() != 4 ) {
            int i = 0;
            ++i;
        }

        Point p;
        p.x  = std::stoi( matchGroup[ 1 ] );
        p.y  = std::stoi( matchGroup[ 2 ] );
        p.vx = std::stoi( matchGroup[ 3 ] );
        p.vy = std::stoi( matchGroup[ 4 ] );

        points.push_back( p );
    }

    return points;
}

void updateScreen( const std::vector< Point > points ) {
    screen.fill( '.' );
    for( const auto &p : points ) {
        auto x = p.x - viewportX;
        auto y = p.y - viewportY;

        if( x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
            screen[ x + y * screenWidth ] = '#';
        }
    }
}

void printScreen() {
    for( int y = 0; y < screenHeight; ++y ) {
        for( int x = 0; x < screenWidth; ++x ) {
            std::cout << screen[ x + y * screenWidth ];
        }
        std::cout << '\n';
    }
}

int calculateDispersionAt( const std::vector< Point > &points, int seconds ) {
    // Is this a good metric?
    int maxx = -1000000;
    int minx =  1000000;
    int maxy = -1000000;
    int miny =  1000000;

    for( const auto &p : points ) {
        auto x = p.x + seconds * p.vx;
        auto y = p.y + seconds * p.vy;
        
        maxx = std::max( maxx, x );
        maxy = std::max( maxy, y );
        minx = std::min( minx, x );
        miny = std::min( miny, y );
    }

    return maxx - minx + maxy - miny;
}

Point calculateCenter( const std::vector< Point > &points ) {
    Point center;
    center.x = center.y = 0;

    for( const auto &p : points ) {
        center.x += p.x;
        center.y += p.y;
    }

    center.x /= points.size();
    center.y /= points.size();

    return center;
}

void advanceTime( std::vector< Point > &points, int seconds ) {
    for( auto &p : points ) {
        p.x += seconds * p.vx;
        p.y += seconds * p.vy;
    }
}

int main( void ) {
    auto points = parseInput();

    int lo = 0, hi = 100000;
    while( lo < hi ) {
        int t1 = lo + ( hi - lo ) / 3 + 1;
        int t2 = hi - ( hi - lo ) / 3 - 1;

        int f1 = calculateDispersionAt( points, t1 );
        int f2 = calculateDispersionAt( points, t2 );

        if( f1 > f2 ) {
            lo = t1;
        } else if( f1 < f2 ) {
            hi = t2;
        } else {
            lo = t1;
            hi = t2;
        }
    }

    auto t = lo - animationSteps / 2;
    advanceTime( points, t );
    for( int i = 0; i < animationSteps; ++i ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( animationFrameLengthMs ));

        auto center = calculateCenter( points );
        viewportX = center.x - screenWidth / 2;
        viewportY = center.y - screenHeight / 2;

        updateScreen( points );
        std::cout << "t = " << t << '\n';
        printScreen();
        std::cout << std::endl; // flush output

        advanceTime( points, 1 );
        ++t;
    }

    return 0;
}