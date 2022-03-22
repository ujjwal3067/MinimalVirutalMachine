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


#include "./core/bit-utilities.h"
#include "./core/core.h"
#include "./core/input-buffering.h"
#include "./core/read-image.h"
#include "./core/input-buffering.h"



// Standard fetch/execute cycle using switch statement
void fetchExecute() {
  /* FETCH */
  uint16_t instruction = mem_read(registers[R_PC]++);
  uint16_t opcode = instruction >> 12;

  switch (opcode) {
  case OP_ADD:
    add(instruction);      
    break;
  case OP_AND:
    and(instruction);
    break;
  case OP_NOT:
    not(instruction);
    break;
  case OP_BR:
    branch(instruction);
    break;
  case OP_JMP:
    jump(instruction);
    break;
  case OP_JSR:
    jumpToSubroutine(instruction);
    break;
  case OP_LD:
    load(instruction);
    break;
  case OP_LDI:
    loadIndirect(instruction);
    break;
  case OP_LDR:
    loadRegister(instruction);
    break;
  case OP_LEA:
    loadEffectiveAddress(instruction);
    break;
  case OP_ST:
    store(instruction);
    break;
  case OP_STI:
    storeIndirect(instruction);
    break;
  case OP_STR:
    storeRegister(instruction);
    break;
  case OP_TRAP:
    trap(instruction);
    break;
  case OP_RES:
    abort();
    break;
  case OP_RTI:
    abort();
    break;
  default:
    // Bad opcode
    printf("BAD OPCODE\n");
    break;
  }
}

fn main(int argc, const char* argv[]) { 
    if(argc < 2) { 
        printf("lc2 [image-file]...\n"); 
        exit(2); 
    }

    for(int j = 1 ; j < argc, ++j) { 
        if(!read_image(argv[j], memory)) { 
            printf("fialed to load image : %s\n", argv[i]); 
            exit(1); 
        }
    }

    signal(SIGINT, handle_interrupt); 
    disable_input_buffering(); 

    // set the program counter to the default address : 0x3000
    // address from 0x0000 to 0x2999 are left empty to leave space for trap routines
    enum { 
        PC_START : 0x3000
    };
    registers[R_PC] = PC_START; 

    // fetch and execute using switch statement

    restore_input_buffering(); 
}
