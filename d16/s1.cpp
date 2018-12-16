#include <iostream>
#include <cstdio>
#include <optional>
#include <array>
#include <functional>

using Registers = std::array< int, 4 >;
using Instruction = std::array< int, 4 >;

struct Sample {
    Instruction instruction;
    Registers stateBefore;
    Registers stateAfter;
};

std::optional< Sample > nextSample() {
    Sample sample;
    auto readSymbols = std::scanf( "Before: [%d, %d, %d, %d]\n"
                                    "%d %d %d %d\n"
                                    "After:  [%d, %d, %d, %d]\n\n"
                                    , &sample.stateBefore[0], &sample.stateBefore[1], &sample.stateBefore[2], &sample.stateBefore[3]
                                    , &sample.instruction[0], &sample.instruction[1], &sample.instruction[2], &sample.instruction[3]
                                    , &sample.stateAfter[0], &sample.stateAfter[1], &sample.stateAfter[2], &sample.stateAfter[3] );

    if( readSymbols < 12 ) {
        return std::nullopt;
    }

    return sample;
}

void addr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] + state[ instr[2] ];
}

void addi( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] + instr[2];
}

void mulr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] * state[ instr[2] ];
}

void muli( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] * instr[2];
}

void banr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] & state[ instr[2] ];
}

void bani( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] & instr[2];
}

void borr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] | state[ instr[2] ];
}

void bori( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] | instr[2];
}

void setr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ];
}

void seti( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = instr[1];
}

void gtir( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = instr[1] > state[ instr[2] ] ? 1 : 0;
}

void gtri( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] > instr[2] ? 1 : 0;
}

void gtrr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] > state[ instr[2] ] ? 1 : 0;
}

void eqir( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = instr[1] == state[ instr[2] ] ? 1 : 0;
}

void eqri( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] == instr[2] ? 1 : 0;
}

void eqrr( const Instruction &instr, Registers &state ) {
    state[ instr[3] ] = state[ instr[1] ] == state[ instr[2] ] ? 1 : 0;
}

int countPossibleInstructions( const Sample &sample ) {
    const static std::array< std::function< void(const Instruction &, Registers &) >, 16 > executables {
        addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr
    };

    int counter = 0;
    for( const auto &executable : executables ) {
        Registers mutableState = sample.stateBefore;
        executable( sample.instruction, mutableState );
        if( mutableState == sample.stateAfter ) {
            ++counter;
        }
    }

    return counter;
}

int main( void ) {
    int nSamplesWithMultiInstructions = 0;
    while( auto sample = nextSample()) {
        if( countPossibleInstructions( sample.value()) >= 3 ) {
            ++nSamplesWithMultiInstructions;
        }
    }

    std::cout << nSamplesWithMultiInstructions << std::endl;

    return 0;
}