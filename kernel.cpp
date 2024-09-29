#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

void clrscr();

void printf(char* str){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0, y=0;
    
    for (int i=0; str[i] != '\0'; i++){
        if (x >= 80) { x = 0; y++; }
        if (y >= 25){
            clrscr();
            x = y = 0;
        }
        switch(str[i]){
            case '\n': x = 0; y++; break;
            case '\t': x += 4; break;
            case '\b': if (y!=0){
                if (x==0){
                    y--;
                    x=79;
                } else {
                    x--;
                }
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x8F00) | (int)" ";
            }; break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x8F00) | str[i];
                x++;
        }
    }
}

void printHex(uint8_t key){
    char* hex = "0123456789ABCDEF";
    char* foo = "00";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

void clrscr(){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i=0; i<80*25; ++i){
        VideoMemory[i] = (VideoMemory[i] & 0xFF00);   
    }
}

// ========================================== KERNEL ==============================================

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
    printf("\n_____________________________ Kushagra's OS Kernel ____________________________\n");
    
    GlobalDescriptorTable gdt;

    printf("\n> Global Descriptor Table ......... CHECK\n");
    printf("> Ports ........................... CHECK\n");
    
    InterruptManager interrupts(0x20, &gdt);

    KeyboardDriver keyboard(&interrupts);
    
    interrupts.Activate();

    printf("> Interrupts Activated ............ CHECK\n\n");
    printf("Activating SHELL ---\n");
    printf("$ ");

    while(1);
}