#include <iostream>
#include <vector>
#include <cstring>
#include <functional>
#include <cstdio>
#include <array>
#include <map>

using Arguments = std::array< int, 3 >;
using Registers = std::array< int, 6 >;

struct Instruction {
    std::function< void( const Arguments &, Registers &) > func;
    Arguments args;  
};

void addr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] + state[ args[1] ];
}

void addi( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] + args[1];
}

void mulr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] * state[ args[1] ];
}

void muli( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] * args[1];
}

void banr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] & state[ args[1] ];
}

void bani( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] & args[1];
}

void borr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] | state[ args[1] ];
}

void bori( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] | args[1];
}

void setr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ];
}

void seti( const Arguments &args, Registers &state ) {
    state[ args[2] ] = args[0];
}

void gtir( const Arguments &args, Registers &state ) {
    state[ args[2] ] = args[0] > state[ args[1] ] ? 1 : 0;
}

void gtri( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] > args[1] ? 1 : 0;
}

void gtrr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] > state[ args[1] ] ? 1 : 0;
}

void eqir( const Arguments &args, Registers &state ) {
    state[ args[2] ] = args[0] == state[ args[1] ] ? 1 : 0;
}

void eqri( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] == args[1] ? 1 : 0;
}

void eqrr( const Arguments &args, Registers &state ) {
    state[ args[2] ] = state[ args[0] ] == state[ args[1] ] ? 1 : 0;
}

int getIRBoundRegister() {
    int irBound;
    scanf( "#ip %d\n", &irBound );
    return irBound;
}

std::vector< Instruction > getInstructions() {
    std::vector< Instruction > instructions;

    char instr[ 5 ] { 0 };
    Arguments args;

    while( scanf( "%s %d %d %d\n", instr, &args[ 0 ], &args[ 1 ], &args[ 2 ] ) > 0 ) {
        Instruction i;
        i.args = args;
        if( std::strcmp( instr, "addr" ) == 0 ) {
            i.func = &addr;
        } else if( std::strcmp( instr, "addi" ) == 0 ) {
            i.func = &addi;
        } else if( std::strcmp( instr, "mulr" ) == 0 ) {
            i.func = &mulr;
        } else if( std::strcmp( instr, "muli" ) == 0 ) {
            i.func = &muli;
        } else if( std::strcmp( instr, "banr" ) == 0 ) {
            i.func = &banr;
        } else if( std::strcmp( instr, "bani" ) == 0 ) {
            i.func = &bani;
        } else if( std::strcmp( instr, "borr" ) == 0 ) {
            i.func = &borr;
        } else if( std::strcmp( instr, "bori" ) == 0 ) {
            i.func = &bori;
        } else if( std::strcmp( instr, "setr" ) == 0 ) {
            i.func = &setr;
        } else if( std::strcmp( instr, "seti" ) == 0 ) {
            i.func = &seti;
        } else if( std::strcmp( instr, "gtir" ) == 0 ) {
            i.func = &gtir;
        } else if( std::strcmp( instr, "gtri" ) == 0 ) {
            i.func = &gtri;
        } else if( std::strcmp( instr, "gtrr" ) == 0 ) {
            i.func = &gtrr;
        } else if( std::strcmp( instr, "eqir" ) == 0 ) {
            i.func = &eqir;
        } else if( std::strcmp( instr, "eqri" ) == 0 ) {
            i.func = &eqri;
        } else if( std::strcmp( instr, "eqrr" ) == 0 ) {
            i.func = &eqrr;
        } else {
            std::cerr << "Invalid instruction\n";
        }

        instructions.push_back( i );
    }

    return instructions;
}

int main( void ) {
    int irBound = getIRBoundRegister();
    auto instructions = getInstructions();

    Registers state;
    state.fill( 0 );
    int instructionRegister = 0;

    while( instructionRegister >= 0 && instructionRegister < instructions.size()) {
        state[ irBound ] = instructionRegister;
        auto &instr = instructions[ instructionRegister ];
        instr.func( instr.args, state );
        instructionRegister = state[ irBound ];
        ++instructionRegister;
    }

    std::cout << state[ 0 ] << std::endl;

    return 0;
}