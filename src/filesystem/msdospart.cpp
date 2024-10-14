
# include <drivers/ata.h>
# include <common/types.h>
# include <filesystem/msdospart.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::filesystem;

void printf(char* str);
void printHex(uint8_t key);

void MSDOSPartitionTable::ReadPartitions(myos::drivers::AdvancedTechnologyAttachment *hd){
    MasterBootRecord mbr;
    hd->Read28(0, (uint8_t*)&mbr, 512);

    if (mbr.magicNumber != 0xAA55){
        printf("\nInvalid MBR");
        return;
    }

    printf("\nMaster Boot Record:");
    for (int i = 446; i < 446 + (4 * 16); i++){
        printHex(((uint8_t*)&mbr)[i]); printf(" ");
    }

    for (int i = 0; i < 4; i++){
        printf("\nPartition ");
        printHex( i & 0xFF); printf(" : ");

        if (mbr.primaryPartition[i].bootable == 0x80){
            printf("Bootable. Type: ");
        } else {
            printf("Not Bootable. Type:");
        }
        printHex(mbr.primaryPartition[i].partition_id);
    }
};
