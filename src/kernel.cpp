#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <drivers/vga.h>
#include <multitasking.h>
#include <memorymanagement.h>

#include <drivers/amd_am79c973.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

// #define __ADD_TASKS
// #define __DYNAMIC_MEMORY_TESTS
// #define __AMD_AM79C973_TESTS

void clrscr();
void scrlDown(uint8_t lines = 1);
void printHex(uint8_t key);

void printf(char* str){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0, y=0;
    
    for (int i=0; str[i] != '\0'; i++){
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
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00);
            }; break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x8F00) | str[i];
                x++;
        }
        if (x >= 80) { x = 0; y++; }
        if (y >= 25){
            scrlDown(2);
            x = 0; y = 23;
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

void scrlDown(uint8_t lines){
    if (lines == 0) return;
    if (lines >= 25) {
        clrscr(); return;
    }

    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    uint8_t x = 0, y = 0;

    for (int i = 0; i < 25-lines; i++){
        for (int j = 0; j < 80; j++){
            VideoMemory[80*i+j] = VideoMemory[80*(i+lines)+j];
        }
    }

    for (int i = 25-lines; i < 25; i++){
        for (int j = 0; j < 80; j++){
            VideoMemory[80*i+j] = (VideoMemory[80*i+j] & 0xFF00);
        }
    }
}

void clrscr(){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i=0; i<80*25; ++i){
        VideoMemory[i] = (VideoMemory[i] & 0xFF00);   
    }
}

// ================================= TASKS FOR TASK MANAGER =======================================

#ifdef __ADD_TASKS
void TaskA(){
    while(1){
        printf("A");
    }
}
void TaskB(){
    while(1){
        printf("B");
    }
}
#endif

// ========================================== KERNEL ==============================================

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
    for (constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)();
    }
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/){
    clrscr();
    printf("\n_____________________________ Kushagra's OS Kernel _____________________________\n");
    
    GlobalDescriptorTable gdt;

    TaskManager taskManager;

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (size_t)((*memupper)*1024 - heap - 10*1024));

    #ifdef __DYNAMIC_MEMORY_TESTS // -------------------------------------- DYNAMIC MEMORY TESTS
        printf("\n> Dynamic Memory Tests ........... CHECKING\n");
        printf("  Heap: 0x");
        printHex((heap >> 24) & 0xFF);
        printHex((heap >> 16) & 0xFF);
        printHex((heap >> 8) & 0xFF);
        printHex(heap & 0xFF);

        struct Point {
            int x, y, z;
        };

        void* alloc1 = new Point();
        printf("\n  Allocated 12 bytes at 0x");
        printHex((((size_t)alloc1) >> 24) & 0xFF);
        printHex((((size_t)alloc1) >> 16) & 0xFF);
        printHex((((size_t)alloc1) >> 8) & 0xFF);
        printHex(((size_t)alloc1) & 0xFF);

        void* alloc2 = new Point();
        printf("\n  Allocated 12 bytes at 0x");
        printHex((((size_t)alloc2) >> 24) & 0xFF);
        printHex((((size_t)alloc2) >> 16) & 0xFF);
        printHex((((size_t)alloc2) >> 8) & 0xFF);
        printHex(((size_t)alloc2) & 0xFF);

        delete alloc1;
        printf("\n  Freed 1024 bytes at 0x");

        void* alloc3 = new Point();
        printf("\n  Allocated 12 bytes at 0x");
        printHex((((size_t)alloc3) >> 24) & 0xFF);
        printHex((((size_t)alloc3) >> 16) & 0xFF);
        printHex((((size_t)alloc3) >> 8) & 0xFF);
        printHex(((size_t)alloc3) & 0xFF);

    #endif

    #ifdef __ADD_TASKS // ------------------------------------------------- TASK MANAGER TESTS
        Task task1(&gdt, TaskA);
        Task task2(&gdt, TaskB);
        taskManager.AddTask(&task1);
        taskManager.AddTask(&task2);
    #endif

    printf("\n> Global Descriptor Table ......... CHECK");
    printf("\n> Ports ........................... CHECK");
    
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    DriverManager drvManager;

    KeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);

    MouseEventHandler mhandler;
    MouseDriver mouse(&interrupts, &mhandler);

    drvManager.AddDriver(&keyboard);
    drvManager.AddDriver(&mouse);

    PCIController pciController;
    pciController.SelectDrivers(&drvManager, &interrupts);

    VideoGraphicsArray vga;

    drvManager.ActivateAll();

    // vga.SetMode(320, 200, 8);

    // for (uint32_t y=0; y<200; y++){
    //     for (uint32_t x=0; x < 320; x++){
    //         vga.PutPixel(x, y, 0x00, 0x00, 0xA8);
    //     }
    // }

    #ifdef __AMD_AM79C973_TESTS // ---------------------------------------- AMD AM79C973 TESTS
        AMD_AM79C973* eth0 = (AMD_AM79C973*)drvManager.drivers[2];
        eth0->Send((uint8_t*)"Hello, World!", 13);
    #endif


    interrupts.Activate();
    printf("\n> Interrupts Activated ............ CHECK\n\n");

    printf("Activating SHELL ---\n");
    printf("$ ");

    while(1);
}