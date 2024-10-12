#include <common/types.h>
#include <syscalls.h>
#include <hardwarecommunication/interrupts.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(char* str);

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t interruptNumber):
InterruptHandler(interruptManager, interruptNumber + interruptManager->HardwareInterruptOffset()){

};
SyscallHandler::~SyscallHandler(){};

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp){
    CPUState* cpu = (CPUState*)esp;

    switch (cpu->eax){
        case 4:
            printf((char*)cpu->ebx);

        default: break;
    }

    return esp;
};