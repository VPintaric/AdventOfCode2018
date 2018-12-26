#include <iostream>
#include <vector>
#include <cstring>
#include <functional>
#include <cstdio>
#include <array>
#include <map>
#include <set>

using Arguments = std::array< long long, 3 >;
using Registers = std::array< long long, 6 >;

struct Instruction {
    bool argAIsRegister, argBIsRegister;
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

    while( scanf( "%s %lld %lld %lld\n", instr, &args[ 0 ], &args[ 1 ], &args[ 2 ] ) > 0 ) {
        Instruction i;
        i.args = args;
        if( std::strcmp( instr, "addr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
            i.func = &addr;
        } else if( std::strcmp( instr, "addi" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &addi;
        } else if( std::strcmp( instr, "mulr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
            i.func = &mulr;
        } else if( std::strcmp( instr, "muli" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &muli;
        } else if( std::strcmp( instr, "banr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
            i.func = &banr;
        } else if( std::strcmp( instr, "bani" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &bani;
        } else if( std::strcmp( instr, "borr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
            i.func = &borr;
        } else if( std::strcmp( instr, "bori" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &bori;
        } else if( std::strcmp( instr, "setr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &setr;
        } else if( std::strcmp( instr, "seti" ) == 0 ) {
            i.argAIsRegister = false;
            i.argBIsRegister = false;
            i.func = &seti;
        } else if( std::strcmp( instr, "gtir" ) == 0 ) {
            i.argAIsRegister = false;
            i.argBIsRegister = true;
            i.func = &gtir;
        } else if( std::strcmp( instr, "gtri" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &gtri;
        } else if( std::strcmp( instr, "gtrr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
            i.func = &gtrr;
        } else if( std::strcmp( instr, "eqir" ) == 0 ) {
            i.argAIsRegister = false;
            i.argBIsRegister = true;
            i.func = &eqir;
        } else if( std::strcmp( instr, "eqri" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = false;
            i.func = &eqri;
        } else if( std::strcmp( instr, "eqrr" ) == 0 ) {
            i.argAIsRegister = true;
            i.argBIsRegister = true;
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
    long long instructionRegister = 0;

    std::set< long long > seenValues;
    int solution;
    while( instructionRegister >= 0 && instructionRegister < instructions.size()) {
        state[ irBound ] = instructionRegister;
        auto &instr = instructions[ instructionRegister ];
        if(( instr.args[ 0 ] == 0 && instr.argAIsRegister ) || 
                (instr.args[ 1 ] == 0 && instr.argBIsRegister)) {
            int valueOfOtherReg;
            if( instr.args[ 0 ] == 0 && instr.argAIsRegister ) {
                valueOfOtherReg = state[ instr.args[ 1 ] ];
            } else if( instr.args[ 1 ] == 0 && instr.argBIsRegister ) {
                valueOfOtherReg = state[ instr.args[ 0 ] ];
            }

            if( seenValues.find( valueOfOtherReg ) == seenValues.end()) {
                solution = valueOfOtherReg;
                seenValues.insert( valueOfOtherReg );
            } else {
                break;
            }
        }
        instr.func( instr.args, state );
        instructionRegister = state[ irBound ];
        ++instructionRegister;
    }

    std::cout << solution << std::endl;

    return 0;
}