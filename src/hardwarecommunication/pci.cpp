
#include <hardwarecommunication/pci.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);
void printHex(uint8_t key);


// ===================================== PCI Device Descriptor ========================================

PCIDeviceDescriptor::PCIDeviceDescriptor(){}
PCIDeviceDescriptor::~PCIDeviceDescriptor(){}

// ======================================== PCI Controller ============================================

PCIController::PCIController(): dataPort(0xCFC), commandPort(0xCF8){}
PCIController::~PCIController(){} 

uint32_t PCIController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset){
    uint32_t id = 
        (0x1 << 31)
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC)
        | ((uint32_t)0x80000000);

    commandPort.Write(id);
    uint32_t result = dataPort.Read();

    return result >> (8 * (registeroffset % 4));
}

void PCIController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value){
    uint32_t id = 
    (0x1 << 31)
    | ((bus & 0xFF) << 16)
    | ((device & 0x1F) << 11)
    | ((function & 0x07) << 8)
    | (registeroffset & 0xFC)
    | ((uint32_t)0x80000000);

    commandPort.Write(id);
    dataPort.Write(value);
}

bool PCIController::DeviceHasFunctions(uint16_t bus, uint16_t device){
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void PCIController::SelectDrivers(DriverManager* driverManager){
    for (int bus = 0; bus < 8; bus++){
        for (int device = 0; device < 32; device++){
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1 ;
            for (int function = 0; function < numFunctions; function++){

                PCIDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

                printf("PCI BUS:");
                printHex(bus & 0xFF);

                printf(", DEVICE: ");
                printHex(device & 0xFF);

                printf(", FUNCTION: ");
                printHex(function & 0xFF);

                printf(" = VENDOR: ");
                printHex((dev.vendor_id & 0xFF00)>>8);
                printHex(dev.vendor_id & 0xFF);

                printf(", DEVICE ID: ");
                printHex((dev.device_id & 0xFF00)>>8);
                printHex(dev.device_id & 0xFF);

                printf("\n");
            }
        }
    }
}

PCIDeviceDescriptor PCIController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function){
    PCIDeviceDescriptor result;
    
    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.vendor_id = Read(bus, device, function, 0x3C);

    return result;
}