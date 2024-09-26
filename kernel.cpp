#include "types.h"
#include "gdt.h"
#include "interrupts.h"

void clrscr();

void print(char* str){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0, y=0;
    
    for (int i=0; str[i] != '\0'; i++){
        if (x >= 80) { x = 0; y++; }
        if (y >= 25){
            clrscr();
            x = y = 0;
        }
        switch(str[i]){
            case '\n':
                x = 0; y++; break;
            case '\t':
                x += 4; break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x8F00) | str[i];
                x++;
        }
    }
    y++; x=0;
}

void clrscr(){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i=0; i<80*25; ++i){
        VideoMemory[i] = (VideoMemory[i] & 0xFF00);   
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
    for (constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}

extern "C" void myKernel(const void* multiboot_structure, uint32_t /*multiboot_magic*/){
    clrscr();
    print("\n_____________________________ Kushagra's OS Kernel _____________________________");
    
    GlobalDescriptorTable gdt;

    print("\n> Global Descriptor Table ......... CHECK");
    print("> Ports ........................... CHECK");
    
    InterruptManager interrupts(0x20, &gdt);
    interrupts.Activate();

    print("> Interrupts Activated ............ CHECK");

    while(1);
}