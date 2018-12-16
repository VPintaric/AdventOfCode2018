#include <iostream>
#include <cstdio>
#include <optional>
#include <array>
#include <functional>
#include <set>

static constexpr int nOpCodes = 16;
static constexpr int nRegisters = 4; 

using Registers = std::array< int, nRegisters >;
using Instruction = std::array< int, nRegisters >;
using Executable = std::function< void(const Instruction &, Registers &) >;

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

const static std::array< Executable, nOpCodes > executables {
    addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr
};

bool deduceMapping( const std::array< std::set< int >, nOpCodes > &possibleMappings, 
                    std::set< int > &usedExes, int opCodeToMap, std::array< int, nOpCodes > &resultMapping ) {
    if( opCodeToMap == nOpCodes ) {
        return true;
    }

    for( auto tryExe : possibleMappings[ opCodeToMap ] ) {
        if( usedExes.find( tryExe ) == usedExes.end()) {
            resultMapping[ opCodeToMap ] = tryExe;
            usedExes.insert( tryExe );
            if( deduceMapping( possibleMappings, usedExes, opCodeToMap + 1, resultMapping )) {
                return true;
            }
            usedExes.erase( tryExe );
        }
    }

    return false;
}

std::array< Executable, nOpCodes > decipherOpCodes() {
    std::array< std::set< int >, nOpCodes > possibleMappings;

    while( auto sample = nextSample()) {
        auto &unwrapped = sample.value();
        for( int exeIndex = 0; exeIndex < nOpCodes; ++exeIndex ) {
            Registers mutableState = unwrapped.stateBefore;
            executables[exeIndex]( unwrapped.instruction, mutableState );
            if( mutableState == unwrapped.stateAfter ) {
                possibleMappings[ unwrapped.instruction[0] ].insert( exeIndex );
            }
        }
    }

    std::array< int, nOpCodes > opCodeToExeIndex;
    std::set< int > usedExes; // helper set for recursion
    deduceMapping( possibleMappings, usedExes, 0, opCodeToExeIndex );

    std::array< Executable, nOpCodes > finalMapping;
    for( int opCode = 0; opCode < nOpCodes; ++opCode ) {
        finalMapping[ opCode ] = executables[ opCodeToExeIndex[ opCode ] ];
    }

    return finalMapping;
}

int main( void ) {
    auto opcodeToInstr = decipherOpCodes();

    scanf("\n\n");

    Registers state;
    state.fill( 0 );
    Instruction instr;
    while( scanf( "%d %d %d %d\n", &instr[0], &instr[1], &instr[2], &instr[3]) == 4 ) {
        opcodeToInstr[ instr[ 0 ] ]( instr, state );
    }

    std::cout << state[ 0 ] << std::endl; 

    return 0;
}