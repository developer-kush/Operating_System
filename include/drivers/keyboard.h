
#ifndef __MYOS__DRIVERS__KEYBOARD_H
#define __MYOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos {
    namespace drivers {

        class KeyboardDriver : public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver {
            myos::hardwarecommunication::Port8Bit dataport;
            myos::hardwarecommunication::Port8Bit commandport;

            public:
                KeyboardDriver(myos::hardwarecommunication::InterruptManager* manager);
                ~KeyboardDriver();
                virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
                virtual void Activate();
        };

    }
}

#endif