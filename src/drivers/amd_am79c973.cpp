
#include <drivers/amd_am79c973.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);

AMD_AM79C973::AMD_AM79C973(myos::hardwarecommunication::PCIDeviceDescriptor* dev, myos::hardwarecommunication::InterruptManager* interrupts)
: Driver(),
InterruptHandler(interrupts, dev -> interrupt + interrupts->HardwareInterruptOffset()),
MACAddress0Port(dev->portBase),
MACAddress2Port(dev->portBase + 0x02),
MACAddress4Port(dev->portBase + 0x04),
registerDataPort(dev->portBase + 0x10),
registerAddressPort(dev->portBase + 0x12),
resetPort(dev->portBase + 0x14),
busControlRegisterDataPort(dev->portBase + 0x16) {
    currentSendBuffer = 0;
    currentRecvBuffer = 0;

    uint64_t MAC0 = MACAddress0Port.Read() % 256;
    uint64_t MAC1 = MACAddress0Port.Read() / 256;
    uint64_t MAC2 = MACAddress2Port.Read() % 256;
    uint64_t MAC3 = MACAddress2Port.Read() / 256;
    uint64_t MAC4 = MACAddress4Port.Read() % 256;
    uint64_t MAC5 = MACAddress4Port.Read() / 256;
    uint64_t MAC = MAC5 << 40
                 | MAC4 << 32
                 | MAC3 << 24
                 | MAC2 << 16
                 | MAC1 << 8
                 | MAC0;

    registerAddressPort.Write(20);
    busControlRegisterDataPort.Write(0x102);

    registerAddressPort.Write(0);
    registerDataPort.Write(0x04);

    initBlock.mode = 0x0000; // promiscuous mode
    initBlock.reserved1 = 0;
    initBlock.numSendBuffers = 3;
    initBlock.reserved2 = 0;
    initBlock.numRecvBuffers = 3;
    initBlock.physicalAddress = MAC;
    initBlock.reserved3 = 0;
    initBlock.logicalAddress = 0;

    sendBufferDescr = (BufferDescriptor*)(((uint32_t)(&sendBufferDescrMemory) + 15) & 0xFFF0);
    initBlock.sendBufferDescrAddress = (uint32_t)sendBufferDescr;

    recvBufferDescr = (BufferDescriptor*)(((uint32_t)(&recvBufferDescrMemory) + 15) & 0xFFF0);
    initBlock.recvBufferDescrAddress = (uint32_t)recvBufferDescr;

    for (uint8_t i = 0; i < 8; i++) {
        sendBufferDescr[i].address = (((uint32_t)(&sendBuffers[i]) + 15) & 0xFFF0);
        sendBufferDescr[i].flags = 0xF7FF;
        sendBufferDescr[i].flags2 = 0;
        sendBufferDescr[i].avail = 0;

        recvBufferDescr[i].address = (((uint32_t)(&recvBuffers[i]) + 15) & 0xFFF0);
        recvBufferDescr[i].flags = 0x8000F000;
        recvBufferDescr[i].flags2 = 0;
        recvBufferDescr[i].avail = 0;
    }

    registerAddressPort.Write(1);
    registerDataPort.Write((uint32_t)(&initBlock) & 0xFFFF);
    registerAddressPort.Write(2);
    registerDataPort.Write(((uint32_t)(&initBlock) >> 16) & 0xFFFF);
};
AMD_AM79C973::~AMD_AM79C973(){

};

void AMD_AM79C973::Activate(){
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41);

    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xC00);
    
    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);
};
int AMD_AM79C973::Reset(){
    resetPort.Read();
    resetPort.Write(0);
    return 10;
};
myos::common::uint32_t AMD_AM79C973::HandleInterrupt(myos::common::uint32_t esp){
    printf("\nINTERRUPT FROM AMD 79C973");

    registerAddressPort.Write(0);
    uint32_t temp = registerDataPort.Read();

    if ((temp & 0x8000) == 0x8000) printf("\nAMD am79c973 error");
    if ((temp & 0x2000) == 0x2000) printf("\nAMD Collision error");
    if ((temp & 0x1000) == 0x1000) printf("\nAMD Missed Frame");
    if ((temp & 0x0800) == 0x0800) printf("\nAMD Memory Error");
    if ((temp & 0x0400) == 0x0400) printf("\nAMD Data Received");
    if ((temp & 0x0200) == 0x0200) printf("\nAMD Data Sent");

    registerAddressPort.Write(0);
    registerDataPort.Write(temp);

    if ((temp & 0x0200) == 0x0200) printf("\nAMD INITIALIZING DONE");

    return esp;
};