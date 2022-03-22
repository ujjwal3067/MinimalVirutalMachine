#include <stdio.h> 
#include <stdint.h> 

#include "bit-utilities.h"
#include "read-image.h"

// read an executable file into memory 
void read_image_file(FILE* file, uint16_t memory[]) { 
    // note : LC-3 is big endian , but x86-84 is little endian
    printf("READ IMAGE FILE\n"); 
    // the original tells use wher in memory to place the image
    uint16_t origin ; 
    fread(&origin, sizeof(origin), 1, file); 
    origin = swap16(origin); 

    // We know the maxium file size so we only need one fread
    uint16_t max_read = UINT16_MAX - origin; 
    uint16_t* program = memory + origin; 
    size_t read = fread(program, sizeof(uint16_t), max_read, file); 

    // convert program from big endinan to little endian 
    // why ? LC-3 is big endian and most modern computers are little endian so we have to convert to little  endian
    while(read-- > 0 ) { 
        *program = swap16(*program); 
        ++program; 
    } 
}

int read_image(const char* image_path, uint16_t memory[]) { 
    printf("READ IMAGE\n"); 
    FILE* file = fopen(image_path, "rb"); 
    if(!file) { 
        return 0 ;
    };
    read_image_file(file, memory);
    fclose(file); 
    return 1; 
}
