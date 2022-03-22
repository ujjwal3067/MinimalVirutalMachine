#include "core.h"

#include<stdio.h> 
#include<unistd.h> 
#include<sys/time.h> 

uint16_t check_key() { 
}


/* 
  Any time a value is written to a registers we need to update 
  the flags to indicate its sign. 

 */
void update_flags(uint16_t r) { 

    if(registers[r] == 0 ) { 

        registers[R_COND] = FL_ZRO;  // a 1 in the left-most bit indicates negative

    }else if (registers[r] >> 15) 
    { 
        registers[R_COND] >> FL_NEG; 
    }else { 
        registers[R_COND]  = FL_POS;
    }
}



// memory access ( write )
void mem_write(uint16_t address, uint16_t val) {
    memory[address] = val; 
}


// memory access ( read )
uint16_t mem_read(uint16_t address) { 
    if(address == MR_KBSR) { 
        if(check_key()) { 
            memory[MR_KBSR] = ( 1 << 15 ); 
            memory[MR_KBSR] = getchar() ; 
        }
        else { 
            memory[MR_KBSR] = 0 ; 
        }
    }
    return memory[address]; 
}
