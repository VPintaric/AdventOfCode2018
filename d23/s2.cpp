#include <iostream>
#include <cstdio>
#include <vector>
#include <random>
#include <set>

using LL = long long;

constexpr int mutMagnitude = 5000;
constexpr int popSize = 1000;
constexpr int nGenerations = 100000;
constexpr int maxElitism = 20;

std::default_random_engine randGen;

struct Vector3D {
    LL x, y, z;

    bool operator<( const Vector3D &other ) const {
        if( x != other.x ) {
            return x < other.x;
        } else if( y != other.y ) {
            return y < other.y;
        } else {
            return z < other.z;
        }
    }
};

struct Nanobot {
    Vector3D p;
    LL r;
};

std::vector< Nanobot > getNanobots() {
    std::vector< Nanobot > nanobots;

    Nanobot n;
    while( scanf( "pos=<%lld,%lld,%lld>, r=%lld\n", &n.p.x, &n.p.y, &n.p.z, &n.r ) == 4 ) {
        nanobots.push_back( n );
    }

    return nanobots;
}

LL dist( const Vector3D &n1, const Vector3D &n2 ) {
    return std::abs( n1.x - n2.x ) + std::abs( n1.y - n2.y ) + std::abs( n1.z - n2.z );
}

struct Solution {
    Vector3D p;
    LL fitness, originDistance;

    bool operator<( const Solution &other ) const {
        return p < other.p;
    }
};

void eval( const std::vector< Nanobot > &nanobots, Solution &s ) {
    s.fitness = 0;
    for( auto &n : nanobots ) {
        if( dist( s.p, n.p ) <= n.r ) {
            ++s.fitness;
        }
    }
    Vector3D origin { 0, 0, 0 };
    s.originDistance = dist( s.p, origin );
}

std::pair< int, int > selection( const std::vector< Solution > &solutions ) {
    LL fitnessSum = 0;
    for( const auto &s : solutions ) {
        fitnessSum += s.fitness;
    }

    std::uniform_int_distribution< int > distr( 0, fitnessSum );
    int first  = distr( randGen );
    int second = distr( randGen );

    int firstIdx;
    fitnessSum = 0;
    for( int i = 0; i < solutions.size(); ++i ) {
        fitnessSum += solutions[ i ].fitness;
        if( first <= fitnessSum ) {
            firstIdx = i;
            break;
        }
    }

    int secondIdx;
    for( int i = 0; i < solutions.size(); ++i ) {
        fitnessSum += solutions[ i ].fitness;
        if( second <= fitnessSum ) {
            secondIdx = i;
            break;
        }
    }

    return std::make_pair( firstIdx, secondIdx );
}

Solution crx( const Solution &s1, const Solution &s2 ) {
    Solution child;

    child.p.x = ( s1.p.x + s2.p.x ) / 2;
    child.p.y = ( s1.p.y + s2.p.y ) / 2;
    child.p.z = ( s1.p.z + s2.p.z ) / 2; 

    return child;
}

void mut( Solution &s ) {
    std::uniform_int_distribution< LL > distr( -mutMagnitude / 2, mutMagnitude / 2 );

    s.p.x += distr( randGen );
    s.p.y += distr( randGen );
    s.p.z += distr( randGen );
}

Solution randomSolution( LL minPos, LL maxPos ) {
    std::uniform_int_distribution< LL > distr( minPos, maxPos );

    Solution s;
    s.p.x = distr( randGen );
    s.p.y = distr( randGen );
    s.p.z = distr( randGen );

    return s;
}

std::set< Solution > getBestSolutions( const std::vector< Nanobot > &nanobots ) {
    LL minPos = nanobots[ 0 ].p.x;
    LL maxPos = nanobots[ 0 ].p.x;
    for( const auto &n : nanobots ) {
        minPos = std::min( n.p.x, minPos );
        minPos = std::min( n.p.y, minPos );
        minPos = std::min( n.p.z, minPos );
        maxPos = std::max( n.p.x, maxPos );
        maxPos = std::max( n.p.y, maxPos );
        maxPos = std::max( n.p.z, maxPos );
    }

    std::vector< Solution > solutions;
    for( int i = 0; i < popSize; ++i ) {
        auto s = randomSolution( minPos, maxPos );
        eval( nanobots, s );
        solutions.push_back( s ); 
    }

    std::set< Solution > bestSolutions;
    bestSolutions.insert( solutions.front());
    LL bestFitness = solutions.front().fitness;
    LL bestOriginDistance = solutions.front().originDistance;
    for( int i = 0; i < nGenerations; ++i ) {
        std::vector< Solution > newGeneration( bestSolutions.begin(), bestSolutions.end());
        if( newGeneration.size() > maxElitism ) {
            newGeneration.erase( newGeneration.begin() + maxElitism, newGeneration.end());
        }

        while( newGeneration.size() < solutions.size()) {
            auto selected = selection( solutions );
            auto child = crx( solutions[ selected.first ], solutions[ selected.second ]);
            mut( child );
            eval( nanobots, child );
            newGeneration.push_back( child );

            if( bestFitness <= child.fitness ) {
                if( bestFitness < child.fitness || bestOriginDistance > child.originDistance ) {
                    bestFitness = child.fitness;
                    bestOriginDistance = child.originDistance;

                    std::cout << bestFitness << " " << bestOriginDistance << '\n';

                    bestSolutions.clear();
                }
                bestSolutions.insert( child );
            }
        }

        solutions = std::move( newGeneration );
    }

    return bestSolutions;
}

int main( void ) {
    auto nanobots = getNanobots();

    auto solutions = getBestSolutions( nanobots );

    Vector3D origin { 0, 0, 0 };

    LL closestDist = 9999999999999;
    for( const auto &s : solutions ) {
        closestDist = std::min( closestDist, dist( s.p, origin ));
    }

    std::cout << closestDist << std::endl;

    return 0;
}