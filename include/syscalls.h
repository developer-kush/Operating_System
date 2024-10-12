
#ifndef __MYOS__SYSCALLS_H
#define __MYOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace myos {

    class SyscallHandler : myos::hardwarecommunication::InterruptHandler {
        public:
            SyscallHandler(myos::hardwarecommunication::InterruptManager* interruptManager, myos::common::uint8_t interruptNumber);
            ~SyscallHandler();

            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
    };

}

#endif