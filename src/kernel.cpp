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
#include <drivers/ata.h>

#include <syscalls.h>
#include <filesystem/msdospart.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::filesystem;
using namespace myos::hardwarecommunication;

// #define __DYNAMIC_MEMORY_TESTS
// #define __AMD_AM79C973_TESTS
// #define __ATA_TESTS
// #define __SYSTEM_CALL_TASKS
// #define __VGA_MODE
// #define __READ_PARTITION_TABLE

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

#ifdef __SYSTEM_CALL_TASKS

void sysprintf(char* str){
    asm("int $0x80" : : "a" (4), "b" (str));
}

void TaskA(){
    while(1){
        sysprintf("A");
    }
}
void TaskB(){
    while(1){
        sysprintf("B");
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

    #ifdef __SYSTEM_CALL_TASKS // ----------------------------------------- SYSTEM CALL TESTS
        Task task1(&gdt, TaskA);
        Task task2(&gdt, TaskB);
        taskManager.AddTask(&task1);
        taskManager.AddTask(&task2);
    #endif

    // printf("\n> Global Descriptor Table ......... CHECK");
    // printf("\n> Ports ........................... CHECK");
    
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);
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

    #ifdef __AMD_AM79C973_TESTS // ---------------------------------------- AMD AM79C973 TESTS
        AMD_AM79C973* eth0 = (AMD_AM79C973*)drvManager.drivers[2];
        eth0->Send((uint8_t*)"Hello, World!", 13);
    #endif

    AdvancedTechnologyAttachment ata0m(0x1F0, true);
    printf("\nATA Primary Master: ");
    ata0m.Identify();

    AdvancedTechnologyAttachment ata0s(0x1F0, false);
    printf("\nATA Primary Slave: ");
    ata0s.Identify();

    // AdvancedTechnologyAttachment ata1m(0x170, true);
    // printf("\nATA Secondary Master: ");
    // ata1m.Identify();

    // AdvancedTechnologyAttachment ata1s(0x170, false);
    // printf("\nATA Secondary Slave: ");
    // ata1s.Identify();

    // .... More ATA at 0x1E8, 0x168, 0x3E0, 0x360

    
    #ifdef __READ_PARTITION_TABLE // ------------------------------------------ READ PARTITION TABLE
        MSDOSPartitionTable::ReadPartitions(&ata0m);
        MSDOSPartitionTable::ReadPartitions(&ata0s);
        printf("\n> Reading Partition Table ........ CHECK\n");
    #endif

    #ifdef __ATA_TESTS // ----------------------------------------------------- ATA TESTS
        printf("\n> ATA Tests ....................... CHECK\n");

        char* result = "             \0";
        ata0m.Read28(0, (uint8_t*)result, 13);

        for (int i=0; i<513; i++){
            printHex((uint8_t)result[i]);
            printf(" ");
        }
        ata0m.Flush();

        printf("Reading Result : ");
        printf(result);
    #endif

    #ifdef __VGA_MODE
        vga.SetMode(320, 200, 8);

        for (uint32_t y=0; y<200; y++){
            for (uint32_t x=0; x < 320; x++){
                if (x>=100 && x<220 && y>=100 && y<120){
                    vga.PutPixel(x, y, 0x01);
                } else {
                    vga.PutPixel(x, y, 0x02);
                }
            }
        }
    #endif

    interrupts.Activate();
    printf("\n> Interrupts Activated ............ CHECK\n\n");

    printf("Activating SHELL ---\n");
    printf("$ ");

    while(1);
}