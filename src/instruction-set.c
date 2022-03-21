#include "./core/core.h"
#include "instruction-set.h"

#include<stdio.h>
#include<stdin.h>
#include<stdlib.h>

// instruction 
void add(uint16_t instruction) { 

     /* Instruction format:
      * [note]: there are two different modes for this instruction ( mode = 1 and mode = 0 ) 
     
      total register = 2**3 = 8
      [note]: In instruction Encoding below SR1 doesn't mean the 3 bit number but rather location of the register   
      that will contain the number ( 16 bit ) we need 
      
    In both modes first 4 bits = opcode value for OP_ADD ( i.e. 0001 )
    Register mode (Mode bit 0):

    15          Dest    Src1   Mode       Src2  0
    |-------------------------------------------|
    | 0 0 0 1 | D D D | A A A | 0 | 0 0 | B B B |
    |-------------------------------------------|

    |-------------------------------------------|
    | 0 0 0 1 |  DR   |  SR1  | 0 | 0 0 |  SR2  |
    |-------------------------------------------|


    DR  = D D D = 3-bit Destination Register ( added sum will be stored here )
    SR1 = A A A = 3-bit Source 1 Register ( register containing first  number to add )
    SR2 = B B B = 3-bit Source 2 Register ( register containing second number to add )

    Assembly :   
        ADD R2 R0 R1; add the contents of R0 to R1 and store in R2
    where :     
        R0 = DR
        R1 = SR1
        R2 = SR2


    Immediate mode (Mode bit 1):


    15          Dest    Src1  Mode  Immediate   0
    |-------------------------------------------|
    | 0 0 0 1 | D D D | A A A | 1 | I I I I I   |
    |-------------------------------------------|


    |-------------------------------------------|
    | 0 0 0 1 |  DR   |  SR1  | 1 |    imm5     |
    |-------------------------------------------|


    DR   = D D D = 3-bit Destination Register ( added sum will be stored here )
    SR1  = A A A = 3-bit Source 1 Register
    imm5 = I I I I I = 5-bit Immediate Value Two's Complement Integer

    NOTE: The immediate value must be sign extended

    immediate mode ( second number is embeeded in the instruction itself)


    pros :  removes the need to write instruction to load the value from memory
    cons :  only has room for small number : (2^5 = 32) (unsigned)
    usecase :  immediate mode is usefull for incrementing and decrementing values 
    imm5 number ( 5 bit currently ) is extended to 16 bit first before adding to another number stored at SR1



    Assembly :   
        ADD R0 R0 1; add 1 to R0 and store back in R0 
    where :     
        R0 = DR
  */

    // [note] : instruction = op_code bits : 
    // for ADD : od_code = 0b0001
    

    // get destination register ( DR )
    uint16_t r0 = (instruction >> 9 ) & 0x7 ; 

    // get source 1 register : first operand ( SR1)
    uint16_t r1 = (instruction >> 6) & 0x7; 

    // get immediate mode flag  
    uint16_t imm_flag = (instr >> 5) & 0x1; 

    // if  immediate mode
    if (imm_flag) {   
        // sign extend the immediate value i.e. imm5 
        uint16_t imm5 = sign_extend(instruction & 0x1F, 5);
        register[r0] = register[r1] + imm5 ; 

    // if register mode
    }else { 
        uint16_t r2 = instruction & 0x7 ; 
        registers[r0] = registers[r1] + registers[r2]; 
    }

    // update the register flag ( based on result of instruction just performed)
    update_flags(r0);     
}
