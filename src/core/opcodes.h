#ifndef _OPCODES
#define _OPCODES


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

// Trap codes
// address in memory that contain trap code : 0x0 to 0x2999
enum { 
    TRAP_GETC  = 0x20,  // get a character from the keyboard, not echoed on the terminal 
    TRAP_OUT   = 0x21,  // output a character
    TRAP_PUTS  = 0x22,  // output a word string
    TRAP_IN    = 0x23,  // Input a string ( get a character from temrinal echoed on the terminal )
    TRAP_PUTSP = 0x24, // output a byte string
    TRAP_HALT  = 0x25,  // halt the program 
};

#endif

