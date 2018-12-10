#include <iostream>
#include <vector>
#include <array>
#include <regex>
#include <thread>
#include <chrono>

constexpr int screenWidth = 400;
constexpr int screenHeight = 100;

int viewportX = 0;
int viewportY = 0;

std::array< char, screenWidth * screenHeight > screen;

struct Point {
    int x, y;
    int vx, vy;
};

std::vector< Point > parseInput() {
    std::regex expr( "position=<([ -][0-9]+), ([ -][0-9]+)> velocity=<([ -][0-9]+), ([ -][0-9]+)>" );
    std::string line;

    std::vector< Point > points;

    while( std::getline( std::cin, line )) {
        std::smatch matchGroup;
        std::regex_match( line, matchGroup, expr );

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
    std::cout << "\n\n\n\n";
    for( int y = 0; y < screenHeight; ++y ) {
        for( int x = 0; x < screenWidth; ++x ) {
            std::cout << screen[ x + y * screenWidth ];
        }
        std::cout << std::endl;
    }
}

Point advanceTime( std::vector< Point > &points, int seconds ) {
    Point center;
    center.x = center.y = 0;

    for( auto &p : points ) {
        p.x += seconds * p.vx;
        p.y += seconds * p.vy;

        center.x += p.x;
        center.y += p.y;
    }

    center.x /= points.size();
    center.y /= points.size();

    return center;
}

int main( void ) {
    auto points = parseInput();

    int seconds = 10450;
    advanceTime( points, seconds );
    while( true ) {
        std::this_thread::sleep_for( std::chrono::milliseconds(200));

        updateScreen( points );
        printScreen();
        std::cout << seconds << '\n';

        auto center = advanceTime( points, 1 );
        ++seconds;

        viewportX = center.x - screenWidth / 2;
        viewportY = center.y - screenHeight / 2;
    }

    return 0;
}