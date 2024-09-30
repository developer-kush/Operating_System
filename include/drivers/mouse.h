
#ifndef __MYOS__DRIVER__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos {
    namespace drivers {
        
        class MouseDriver : public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver {
            myos::hardwarecommunication::Port8Bit dataport;
            myos::hardwarecommunication::Port8Bit commandport;

            myos::common::uint8_t buffer[3];
            myos::common::uint8_t offset;
            myos::common::uint8_t buttons;

            myos::common::int8_t x, y;

            public:
                MouseDriver(myos::hardwarecommunication::InterruptManager* manager);
                ~MouseDriver();
                virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
                virtual void Activate();
        };
        
    }
}


#endif