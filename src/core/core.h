#ifndef _CORE
#define _CORE

#include <stdint.h> 

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
    R_PC,  // program counter
    R_COND, 
    R_COUNT
};


uint16_t registers[R_COUNT];

/**
 * 3 Conditional flags
 * ( indicates the sign of previous calculation)
 */
enum { 
    FL_POS = 1 << 0, // P(ositive) 
    FL_ZRO = 1 << 1, // Z(ero) 
    FL_NEG = 1 << 2, // N(egative)
};

void update_flags(uint16_t r); 

void mem_write(uint16_t address, uint16_t val); 
uint16_t mem_read(uint16_t address); 


#endif


