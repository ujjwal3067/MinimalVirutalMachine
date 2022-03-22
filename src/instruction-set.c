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
    uint16_t PCOffset11 = instruction & 0x7Ff; 
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


void load(uint16_t instruction) { 
/*
 * Instruction format
 An address is computed by sign-extending bits [8:0] to 16 bits and adding this value to the incremented PC.
 The contents of memory at this address are loaded into DR. The condition codes are set,
 based on whether the value loaded is negative, zero, or positive.

    15          Dest   PCOffset9                0
    |-------------------------------------------|
    | 0 0 1 0 | D D D | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 0 0 1 0 |   DR  |     PCoffset9           |
    |-------------------------------------------|

    DR = D D D = 3-bit Destination Register
    P P P P P P P P P = PCOffset9
    Sign extend ( from 9 bit to 16 bit ) PCOffset9 and add to PC ( 16 bit ).
    Load the value at that memory address into destination
 */
    // get the destination register
    uint16_t r0 = (instruction >> 9) & 0x7; 
    uint16_t PCoffset9 = instruction & 0x1FF; 
    uint16_t pc_offset = sign_extend(PCoffset9, 9);  
    reg[r0] = mem_read(reg[R_PC], + pc_offset); 
    update_flags(r0);
}



void loadRegister(uint16_t instruction) { 

    /* Instruction Format:
     
    15          Dest   Base     Offset6         0
    |-------------------------------------------|
    | 0 1 1 0 | D D D | B B B | O O O O O O     |
    |-------------------------------------------|

    |-------------------------------------------|
    | 0 1 1 0 |  DR   | BaseR  |  offset6       |
    |-------------------------------------------|


    DR = D D D = 3-bit Destination Register
    BaseR = B B B = 3-bit Base Register
    offset6 = O O O O O O = 6-bit offset

    Sign extend the offset, add this value to
    the value in the base register. Read the 
    memory at this location and load into
    destination

    
    note : An address is computed by sign-extending bits [5:0] (offset6) to 16 bits and adding this value to
    the contents of the register specified by bits [8:6](baseR). 
    The contents of memory at this address are loaded into DR. The condition codes are set,
    based on whether the value loaded is negative, zero, or positive (updating flags).
    */


    uint16_t r0 = (instruction >> 9) & 0x7 ;  // DR
    uint16_t r1 = (instruction >> 6) & 0x7 ; 
    uint16_t offset = sign_extend(instruction & 0x3F ,6);
    registers[r0] = mem_read(registers[r1] + offset); 
    update_flags(r0); 
}


void loadEffectiveAddress(uint16_t instruction) { 

    /*
     Instruciton set


     15          Dest   PCOffset9                0
    |-------------------------------------------|
    | 1 1 1 0 | D D D | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 1 1 1 0 |  DR   |       offset9           |
    |-------------------------------------------|

    D D D = 3-bit Destination Register
    P P P P P P P P P = PCOffset9
    Sign extend PCOffset9, add to PC, and store
    that ADDRESS in the destination register


    note : An address is computed by sign-extending bits [8:0] to 16 bits and 
    adding this value to the incremented PC. 
    This address is loaded into DR.‡ The condition codes are set, 
    based on whether the value loaded is negative, zero, or positive. 
     */

    uint16_t r0 =  (instruction >> 9) & 0x7 ;// DR
    uint16_t offset9  = instruction & 0x1FF; 
    uint16_t pc_offset = sign_extend(offset9 , 9); 
    registers[r0] = registers[R_PC] + pc_offset; 
    update_flags(r0);
} 

void not(uint16_t instruction) { 
    /*
     
   Instruction format 

    15          Dest    Src    Mode             0
    |-------------------------------------------|
    | 1 0 0 1 | D D D | S S S | 1 | 1 1 1 1 1   |
    |-------------------------------------------|

    |-------------------------------------------|
    | 1 0 0 1 |  DR   |  SR1  | 1 | 1 1 1 1 1   |
    |-------------------------------------------|

    DR  = D D D = 3-bit Destination Register
    SR1 = S S A = 3-bit Source Register

    note : The bit-wise complement of the contents of SR is stored in DR.
    The condition codes are set, based on whether the binary value produced, 
    taken as a 2’s complement integer, is negative, zero, or positive.

     */

    uint16_t r0 = (instruction >> 9) & 0x7 ; 
    uint16_t r1 = (instruction >> 6) & 0x7 ; 

    registers[r0] = ~registers[r1]; // bitwise NOT operation 
    update_flags(r0); 
}


void store(uint16_t instruction) { 
    /*
    Instruction format

     15          Src    PCOffset9                0
    |-------------------------------------------|
    | 0 0 1 1 | S S S | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 0 0 1 1 |   SR  |     PCoffset9           |
    |-------------------------------------------|

    SR = S S S = 3-bit Source Register
    P P P P P P P P P = PCOffset9

    Sign extend PCOffset9, add to PC, and read
    the value from the source register into
    that memory location

    note : The contents of the register specified by SR are stored in the memory location whose address is
    computed by sign-extending bits [8:0] to 16 bits and adding this value to the incremented PC. 
     */


    uint16_t r0 = (instruction >> 9) & 0x7;  
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9);
    mem_write(registers[R_PC] + pc_offet, registers[r0]);
}


void storeIndirect(uint16_t instruction){ 
    /* Instruction Format:
    15          Src    PCOffset9                0
    |-------------------------------------------|
    | 1 0 1 1 | S S S | P P P P P P P P P       |
    |-------------------------------------------|

    |-------------------------------------------|
    | 1 0 1 1 |   SR  |     PCoffset9           |
    |-------------------------------------------|

    SR = S S S = 3-bit Source Register
    P P P P P P P P P = PCOffset9

    Sign extend PCOffset9, add to PC to get an address. 
    Read the value from the source register and
    store in the computed address.

    note : The contents of the register specified by SR are stored in the memory location 
    whose address is obtained as follows: Bits [8:0] are sign-extended to 16 bits and added to the incremented PC. 
    What is in memory at this address is the address of the location to which the data in SR is stored.
  */

    uint16_t r0  = (instruction >> 9) & 0x7 ; 
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9);
    uint16_t address = mem_read(registers[R_PC] + pc_offet); 
    mem_write(address, registers[r0]); // writing address content in r0 register 
}

void storeRegister(uint16_t instruction) { 
      /* Instruction Format:
    15          Src    Base     Offset6         0

    |-------------------------------------------|
    | 0 1 1 1 | S S S | B B B | O O O O O O     |
    |-------------------------------------------|

    |-------------------------------------------|
    | 0 1 1 1 |  SR  | BaseR  |   offset6       |
    |-------------------------------------------|

    SR          = S S S = 3-bit Destination Register
    BaseR       = B B B = 3-bit Base Register
    O O O O O O = 6-bit offset

    Sign extend the offset, add this value to
    the value in the base register. Read the 
    value in the source register and store
    into memory at the computed address

    note : The contents of the register specified by SR are stored in the memory location 
    whose address is computed by
    sign-extending bits [5:0] to 16 bits and adding this value to the contents of the register
    specified by bits [8:6].

  */
    uint16_t r0 = (instruction >> 8) & 0x7 ;  // source register
    uint16_t r1 = (instruction >> 6) & 0x7 ; 
    uint16_t offset = sign_extend(instruction & 0x3F, 6); 
    uint16_t address = registers[r1]+ offset; 
    mem_write(address, registers[r0]); 
}

// trap functions
void trapGetC() { 
    /*
     Read a single character from the keyboard.
     The character is not echoed onto the console. Its ASCII code is copied into R0.
     The high eight bits of R0 are cleared.
     */

    // get a single ASCII char
    registers[R_R0]  = (uint16_t) getchar(); 
} 


void trapPuts(){ 
    /*
     PUTS trap code is used to output a null-terminated string ( similar to printf in C) 
     character are not stored in single byte, but in a single memory location. Memory location in LC-3
     are 16 bits, so each character in the string is 16 bits wide. To display this with a C function, 
     we will need to convert each value to char and output them individually. 


     To display a string, we must give the trap routine a string to display. 
     This is done by storing address of the first character in R0 before beginning the trap 

     Writing terminates with the occurence of 0x0000 in a memory location.   

     */

    // one char per word
    uint16_t* character = memory + registers[R_R0]; 
    while(*character) { 
        putc((char)*character, stdout); 
        ++character;  // increment the memory location  
    }
    fflush(stdout);
}


void trapPuts() { 
    /*
    Write a character in R0[7:0] to the console display.
     */
    putc((char)registers[R_R0], stdout); 
    fflush(stdout);
}

void trapIn() { 
    /**
     Print a prompt on the screen and read a single character from the keyboard. 
     The character is echoed onto the console monitor, and its ASCII code is copied into R0. 
     The high eight bits of R0 are cleared.
     */ 
    printf("Enter a character"); 
    char c = getchar(); 
    putc(c, stdout); 
    fflush(stdout);
    registers[R_R0]= (uint16_t)c; 
    update_flags(R_R0);
}

void trapPutSP() { 
    /*
     Write a string of ASCII characters to the console. 
     The characters are contained in consecutive memory locations,
     two characters per memory location, starting with the address specified in R0. 
     The ASCII code contained in bits [7:0] of a memory location is written to the console first. 
     Then the ASCII code contained in bits [15:8] of that memory location is written to the console. 
     (A character string consisting of an odd number of characters to be written will have 
     x00 in bits [15:8] of the memory location containing the last character to be written.)
     Writing terminates with the occurrence of x0000 in a memory location.
     */

    //one char per byte ( two bytes ( 16 bit ) per word )
    uint16_t * character = memory + registers[R_R0]; 
    while(*character) { 

        // 8 bits [7:0] ( rightmost )
        char char1 = (*character) & 0xFF ; 
        putc(char1, stdout);

        // second 8 bits [15:8] (leftmost)
        char char2 = (*character) >>8 ; 
        if (char2) putc(char2, stdout);
        ++character; 
    }
    fflush(stdout);
}

void trapHalt(){ 
    puts("HALT"); 
    fflush(stdout);
    running = 0 ; 
    exit(0); // if you don't want to quite running program comment this line
}


void trap(uint16_t instruction) { 
    /*
    
    Instruction format 
     
    |-------------------------------------------|
    | 1 1 1 1 | 0 0 0 0|      trapvect8         |
    |-------------------------------------------|

    Trap code is 8 bit

    TRAP_GETC = 0x20,        get character from keyboard, not echoed onto the terminal
    TRAP_OUT = 0x21,         output a character
    TRAP_PUTS = 0x22,        output a word string 
    TRAP_IN = 0x23,          get character from keyboard, echoed onto the terminal 
    TRAP_PUTSP = 0x24,       output a byte string
    TRAP_HALT = 0x25         halt the program 

    [note] : First R7 is loaded with the incremented PC. 
    (This enables a return to the instruction physically following the TRAP 
    instruction in the original program after the service routine has completed execution.) 
    Then the PC is loaded with the starting address of the system call specified by trapvector8. 
    The starting address is contained in the memory location whose address is 
    obtained by zero-extending trapvector8 to 16 bits.
    */

    uint16_t trapCode  = instruction & 0xFF;   
    switch(trapCode) { 
        case TRAP_GETC: 
            trapGetC(); 
            break; 
        case TRAP_OUT: 
            trapOut(); 
            break; 
        case TRAP_PUTS: 
            trapPuts(); 
            break; 
        case TRAP_IN: 
            trapIn(); 
            break ; 
        case TRAP_PUTSP: 
            trapPutSP(); 
            break; 
        case TRAP_HALT : 
            trapHalt(); 
            break; 
    }
}

