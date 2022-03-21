#include "core.h"

#include<stdio.h> 
#include<unistd.h> 
#include<sys/time.h> 

uint16_t check_key() { 
}


/* 
  Any time a value is written to a register we need to update 
  the flags to indicate its sign. 

 */
void update_flags(uint16_t r) { 

    if(reg[r] == 0 ) { 

        reg[R_COND] = FL_ZERO;  // a 1 in the left-most bit indicates negative

    }else if (reg[r] >> 15) 
    { 
        reg[R_COND] >> FL_NEG; 
    }else { 
        reg[R_COND]  = FL_POS;
    }

}

void mem_write(uint16_t address, uint16_t val) { 

}


uint16_t mem_read(uint16_t address) { 

}
