#ifndef __MYOS__HARDWARECOMMUNICATION__PCI_H
#define __MYOS__HARDWARECOMMUNICATION__PCI_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <memorymanagement.h> 

namespace myos {
    namespace hardwarecommunication {

        enum BaseAddressRegisterType {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister{
            public: 
                bool prefetchable;
                myos::common::uint8_t* address;
                myos::common::uint32_t size;
                BaseAddressRegisterType type;
        };

        class PCIDeviceDescriptor {

            public:
                myos::common::uint32_t portBase;
                myos::common::uint32_t interrupt;

                myos::common::uint16_t bus;
                myos::common::uint16_t device;
                myos::common::uint16_t function;

                myos::common::uint16_t vendor_id;
                myos::common::uint16_t device_id;

                myos::common::uint8_t class_id;
                myos::common::uint8_t subclass_id;
                myos::common::uint8_t interface_id;

                myos::common::uint8_t revision;

                PCIDeviceDescriptor();
                ~PCIDeviceDescriptor();
        };

        class PCIController {

            Port32Bit dataPort;
            Port32Bit commandPort;
            
            public:
                PCIController();
                ~PCIController();

                myos::common::uint32_t Read(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset);
                void Write(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset, myos::common::uint32_t value);
                bool DeviceHasFunctions(myos::common::uint16_t bus, myos::common::uint16_t device);

                void SelectDrivers(myos::drivers::DriverManager *driverManager, myos::hardwarecommunication::InterruptManager *interruptManager);
                myos::drivers::Driver* GetDriver(PCIDeviceDescriptor dev, myos::hardwarecommunication::InterruptManager* interrupts);
                PCIDeviceDescriptor GetDeviceDescriptor(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function);

                BaseAddressRegister GetBaseAddressRegister(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint16_t bar);
        };
    }
}

#endif