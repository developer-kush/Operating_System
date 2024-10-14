
#ifndef __MYOS__FILESYSTEM__MSDOSPART_H
#define __MYOS__FILESYSTEM__MSDOSPART_H

#include <common/types.h>
#include <drivers/ata.h>

namespace myos{
    namespace filesystem{

        struct PartitionTableEntry {
            myos::common::uint8_t bootable;

            myos::common::uint8_t start_head;
            myos::common::uint8_t start_sector : 6;
            myos::common::uint16_t start_cylinder : 10;

            myos::common::uint8_t partition_id;

            myos::common::uint8_t end_head;
            myos::common::uint8_t end_sector : 6;
            myos::common::uint16_t end_cylinder : 10;

            myos::common::uint32_t start_lba;
            myos::common::uint32_t length;
        } __attribute__((packed));

        struct MasterBootRecord {
            myos::common::uint8_t bootloader[440];
            myos::common::uint32_t signature;
            myos::common::uint16_t unused;

            PartitionTableEntry primaryPartition[4];
            myos::common::uint16_t magicNumber; 
        } __attribute__((packed));

        class MSDOSPartitionTable {
            public:
                static void ReadPartitions(myos::drivers::AdvancedTechnologyAttachment *hd);
        };

    }
}

#endif