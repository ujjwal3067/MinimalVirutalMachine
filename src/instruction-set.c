#include "./core/core.h"
#include "instruction-set.h"

#include<stdio.h>
#include<stdint.h>
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
    uint16_t imm_flag = (instruction >> 5) & 0x1; 

    // if  immediate mode
    if (imm_flag) {   
        // sign extend the immediate value i.e. imm5 
        uint16_t imm5 = sign_extend(instruction & 0x1F, 5);
        registers[r0] = registers[r1] + imm5 ; 

    // if register mode
    }else { 
        uint16_t r2 = instruction & 0x7 ; 
        registers[r0] = registers[r1] + registers[r2]; 
    }

    // update the register flag ( based on result of instruction just performed)
    update_flags(r0);     
}

void loadIndirect(uint16_t instruction) { 
     /* 
      
    load indirect : this instruction is used to load a value from a location in memory into register
    od_code  = OP_LDI = 0b1010

   Instruction Format:
    15          Dest   PCOffset9                0
    |-------------------------------------------|
    | 1 0 1 0 | D D D | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 1 0 1 0 | DR    |     PCoffset9           |
    |-------------------------------------------|

    DR        = D D D             = 3-bit Destination Register
    PCoffset9 = P P P P P P P P P = PCOffset9 (immediate value, similar to imm5, is embedded in instruction)
                                    ( tells where to load from memory  )


    Sign extend PCOffset9 and add to PC. The value
    stored at that memory location (A) is another address (B). 
    The value stored in memory location B is loaded
    into the destination register.
    (A points to B. The value is located at memory location B)

    This instruction loads from memory. PCoffset9 is a number ( address ) which tells use where to load from.   

    sign extend this 9 bit value, but this time add it to current PC. resulting sum is an address to a location in memory, 
    and that address contains, yet another value which is the address of the value to load. 

    Pcoffset9 is like C pointer to a value
    e.g. 
    char* for_data = "apple"; 
    for_data itself resides in the memory and contains memory address where data resides. 

    In memory it may be layed out like this 

    Address     label       Value
    0x123       for_data    0x456 
    0x456       string      "a"
if PC was at 0x100
    LDI R0 0x023
    would load 'a' into R0 

  */

    // Destination register (DR)
    uint16_t r0 = (instruction >> 9) & 0x7 ; 
    // PCoffset9
    uint16_t pc_offet = sign_extend(instruction & 0x1FF, 9); 

    // add pc_offset to the current PC, look at that memory location to get the final address.  
    registers[r0] = mem_read(mem_read(registers[R_PC] + pc_offset)); 
    update_flags(r0); 
}

void and(uint16_t instruction) { 
    /*
   Instruction Format:
    15          Dest   Src1   Mode       Src2   0
    |-------------------------------------------|
    | 0 1 0 1 | D D D | A A A | 0 | 0 0 | B B B |
    |-------------------------------------------|

    DR    = D D D     = 3-bit Destination Register 
    SR1   = A A A     = 3-bit Source 1 Register
    SR2   = I I I I I = 5-bit Immediate Value Two's Complement Integer


    Immediate mode (Mode bit 1):

    15          Dest    Src1  Mode  Immediate   0
    |-------------------------------------------|
    | 0 1 0 1 | D D D | A A A | 1 | I I I I I   |
    |-------------------------------------------|

    D D D = 3-bit Destination Register
    A A A = 3-bit Source 1 Register
    I I I I I = 5-bit Immediate Value Two's Complement Integer
    NOTE: The immediate value must be sign extended

    */

    // Desitnation register (DR)
    uint16_t r0 = (instruction >> 9 ) & 0x7; 
    // get the source register (SR1)
     
    uint16_t r1 = (instruction >> 6) & 0x7 ; 

    // get immediate mode flag  
    uint16_t imm_flag = (instruction >> 5) & 0x1; 

    if (imm_flag) { 
        // signed extend the immediate value
        uint16_t immediateValue = instruction  & 0x1F;  
        uint16_t signExtendedImmediateValue = sign_extend(immediateValue, 5);
        registers[r0] = registers[r1] & signExtendedImmediateValue; 
    }else { 
        uint16_t r2 = instruction & 0x7 ; 
        registers[r0] = registers[r1] & registers[r2]; 
    }

    update_flags(r0); 
}

void branch(uint16_t instruction) { 
    /*
   Instruction Format:
    15          Flags   PCOffset9                0
    |-------------------------------------------|
    | 1 0 1 0 | N Z P | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 1 0 1 0 | N|Z|P |     PCoffset9           |
    |-------------------------------------------|


    N = Negative Flag ( BRN )
    Z = Zero Flag ( BRZ )
    P = positive Flag ( BRP )
    P P P P P P P P P P   =  Pcoffset9

    Flags can be combined to produce additional branch opcodes : 
    BRZP
    BRNP
    BRNZ
    BRNZP ( also equal to BR )


    if N bit is set, N is tested else N is not tested
    if Z bit is set, Z is test else Z is not tested etc. 

    how does it work ? : 
    If any of the condition codes tested is set, the program branches to the location 
    specified by adding the sign-extended PCoffset9 field to the incrementd PC. 


    Sign Extend PCoffset9 and add to PC

     */

    // Get PCoffset9
     uint16_t pcOffset9 = instruction & 0x1FF; 
     uint16_t signedExtendedpcOffset = sign_extend(pcOffset9, 9);


     // Get the Flag
      
     uint16_t conditionalFlag = (instruction >> 9) & 0x7; 
     if (conditionalFlag & registers[R_COND]) { 
         // if branch conditions are met, branch 
         registers[R_PC] += signedExtendedpcOffset ;  
     }
}

void jump(uint16_t instruction) { 
    /*
    note : RET instruction is special case of JMP instruction in assemlby and happens when R1 register value is 0x7 
           RED always loads R7
     */
    uint16_t r1 = (instruction >> 6) & 0x7; 
    // jump to the content of the registers R1  by pointing PC to value of R1 register 
    registers[R_PC] = registers[r1]; 
}


void jumpToSubroutine(uint16_t instruction) { 
    /* Instruction Format:
  JSR mode:
    15         11  PCOffset11                   0
    |-------------------------------------------|
    | 0 1 0 0 | 1 | P P P | P P P | P P P | P P |
    |-------------------------------------------|
    P P P P P P P P P P P = PCOffset11
  JSRR mode:
    15         11        Register               0
    |-------------------------------------------|
    | 0 1 0 0 | 0 | 0 0 | R R R | 0 0 0 0 0 0   |
    |-------------------------------------------|
    R R R = 3-bit base register
  */

    // Get the base register
    uint16_t baseRegister = (instruction >> 6) & 0x7; 
    uint16_t pcOffset11 = instruction & 0x7Ff; 
    uint16_t signExtendedPCoffset = sign_extend(PCOffset11, 11); 
    registers[R_R7] = registers[R_PC];
    uint16_t long_flag = (instruction >> 11) & 1 ; 

    if (long_flag) { 
        registers[R_PC] += signExtendedPCoffset;  // JSR
    }
    else { 
        registers[R_PC] = registers[baseRegister];  // JSRR
    }
    break; 
}


