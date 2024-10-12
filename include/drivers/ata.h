
#ifndef __MYOS__DRIVERS__ATA_H
#define __MYOS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>
#include <common/types.h>

namespace myos {
    namespace drivers {
        
        class AdvancedTechnologyAttachment {

            protected:
                myos::hardwarecommunication::Port16Bit dataPort;
                myos::hardwarecommunication::Port8Bit errorPort;
                myos::hardwarecommunication::Port8Bit sectorCountPort;
                myos::hardwarecommunication::Port8Bit lbaLowPort;
                myos::hardwarecommunication::Port8Bit lbaMidPort;
                myos::hardwarecommunication::Port8Bit lbaHiPort;
                myos::hardwarecommunication::Port8Bit devicePort;
                myos::hardwarecommunication::Port8Bit commandPort;
                myos::hardwarecommunication::Port8Bit controlPort;

                bool master;
                myos::common::uint16_t bytesPerSector;

            public:
                AdvancedTechnologyAttachment(int portBase, bool master);
                ~AdvancedTechnologyAttachment();

                void Identify();
                void Read28(myos::common::uint32_t sector, myos::common::uint8_t* data, int count);
                void Write28(myos::common::uint32_t sector, myos::common::uint8_t* data, int count);
                void Flush();
        };
    }
}

#endif