
#ifndef __MYOS__FILESYSTEM__FAT_H
#define __MYOS__FILESYSTEM__FAT_H

#include <common/types.h>
#include <drivers/ata.h>

namespace myos {
    namespace filesystem {

        struct BIOSParameterBlock32 {
            myos::common::uint8_t jump[3];
            myos::common::uint8_t softName[8];
            myos::common::uint16_t bytesPerSector;
            myos::common::uint8_t sectorsPerCluster;
            myos::common::uint16_t reservedSectors;
            myos::common::uint8_t fatCopies;
            myos::common::uint16_t rootDirEntries; 
            myos::common::uint16_t totalSectors;
            myos::common::uint8_t mediaType; 
            myos::common::uint16_t fatSectorCount;
            myos::common::uint16_t sectorsPerTrack;
            myos::common::uint16_t headCount;
            myos::common::uint32_t hiddenSectors;
            myos::common::uint32_t totalSectorCount;

            myos::common::uint32_t tableSize;
            myos::common::uint16_t extFlags;
            myos::common::uint16_t fatVersion;
            myos::common::uint32_t rootCluster;
            myos::common::uint16_t fatInfo;
            myos::common::uint16_t backupSector;
            myos::common::uint8_t reserved0[12];
            myos::common::uint8_t driveNumber;
            myos::common::uint8_t reserved;
            myos::common::uint8_t bootSignature;
            myos::common::uint32_t volumeId;
            myos::common::uint8_t volumeLabel[11];
            myos::common::uint8_t fatTypeLabel[8];
        } __attribute__((packed));

        struct DirectoryEntryFAT32 {
            myos::common::uint8_t name[8];
            myos::common::uint8_t ext[3];
            myos::common::uint8_t attributes;
            myos::common::uint8_t reserved;
            myos::common::uint8_t cTimeTenth;
            myos::common::uint16_t cTime;
            myos::common::uint16_t cDate;
            myos::common::uint16_t aTime;
            
            myos::common::uint16_t firstClusterHi;

            myos::common::uint16_t wTime;
            myos::common::uint16_t wDate;
            myos::common::uint16_t firstClusterLo;
            myos::common::uint32_t size;
        } __attribute__((packed));

        void ReadBiosBlock(myos::drivers::AdvancedTechnologyAttachment *hd, myos::common::uint32_t partitionOffset);
    }
}

#endif