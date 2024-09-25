void printf(char* str){
    unsigned short* VideoMemory = (unsigned short*)0xb8000;
    
    for (int i=0; str[i] != '\0'; ++i){
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
    }
}

void clrscr(){
    unsigned short* VideoMemory = (unsigned short*)0xb8000;

    for (int i=0; i<1000; ++i){
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

extern "C" void myKernel(void* multiboot_structure, unsigned int multiboot_magic){
    clrscr();
    printf("Hello, from Kush's OS Kernel :) -> A New Begining");
    while(1);
}