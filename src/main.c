#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
/* unix */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

// 65536 memory locations
// 16 bit memory slots 
uint16_t memory[UINT16_MAX];


// 16 bit registers
/* 3 registers
 * 8 general purpose registers (r0-r7)
 * 1 program counter ( PC  ) register
 * 1 condition flags ( COND ) register
 */
  
enum { 
    R_R0 = 0, 
    R_R1,  
    R_R2,  
    R_R3,  
    R_R4,  
    R_R5,  
    R_R6,  
    R_R7,  
    R_PC, 
    R_COND, 
    R_COUNT
};


uint16_t reg[R_COUNT];

/*
 * Instruction sets ;
 *
 * 16 bit : 4 bits + 12 bits
 *  where 4 bit = opcode  & 12 bits for storing parameters
 *
 */

enum { 
    OP_BR = 0, // branch 
    OP_ADD,    // add
    OP_LD,     // load
    OP_ST,     // store
    OP_JSR,    // jump register
    OP_LDR,    // load register
    OP_STR,    // store register
    OP_RTI,    // unused
    OP_NOT,    // bitwise not
    OP_LDI,    // load indirect
    OP_STI,    // store indirect
    OP_JMP,    // jump 
    OP_RES,    // reserved( unused )
    OP_LEA,    // load effective  address
    OP_TRAP,   // execute trap 
};

/**
 * 3 Conditional flags
 * ( indicates the sign of previouse calculation)
 */
enum { 
    FL_POS = 1 << 0, // P 
    FL_ZRO = 1 << 1, // Z 
    FL_NEG = 1 << 2, // N 
};

int main (int argc, const char * argv[]) { 
    // since exactly one condition flag should be set at any given time, set the Z flag
    reg[R_COND] = FL_ZRO; 
    // set the PC ( program counter ) to starting position 
    // 0x3000 is the default
    enum { 
        PC_START = 0x3000
    };
    reg[R_PC] = PC_START;  

    int running = 1; 
    while (running) { 

        // FETCH 
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12 ;  // opcode

        switch (op)
        {
            case OP_ADD:
                {ADD, 6}
                break;
            case OP_AND:
                {AND, 7}
                break;
            case OP_NOT:
                {NOT, 7}
                break;
            case OP_BR:
                {BR, 7}
                break;
            case OP_JMP:
                {JMP, 7}
                break;
            case OP_JSR:
                {JSR, 7}
                break;
            case OP_LD:
                {LD, 7}
                break;
            case OP_LDI:
                {LDI, 6}
                break;
            case OP_LDR:
                {LDR, 7}
                break;
            case OP_LEA:
                {LEA, 7}
                break;
            case OP_ST:
                {ST, 7}
                break;
            case OP_STI:
                {STI, 7}
                break;
            case OP_STR:
                {STR, 7}
                break;
            case OP_TRAP:
                {TRAP, 8}
                break;
            case OP_RES:
            case OP_RTI:
            default:
                {BAD OPCODE, 7}
                break;
        }

   }
    restore_input_buffering(); 

    if(argc < 2) { 
        // show usage string
       printf("lc3 [image-file]...\n"); 
       exit(2); 
    }

    for ( int j = 1 ; j < argc : ++j ) { 
        if (!read_image(argv[j])){ 
            printf("failed to load image : %s\n", argv[j]); 
            exit(1);
        }
    }

}
