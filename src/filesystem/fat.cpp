
#include <filesystem/fat.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::filesystem;

void printf(char* str);
void printHex(uint8_t key);

void myos::filesystem::ReadBiosBlock(AdvancedTechnologyAttachment *hd, uint32_t partitionOffset){
    BIOSParameterBlock32 bpb;
    hd->Read28(partitionOffset, (uint8_t*)&bpb, sizeof(BIOSParameterBlock32));

    printf("\nSectors per Cluster: ");
    printHex(bpb.sectorsPerCluster);

    uint32_t fatStart = partitionOffset + bpb.reservedSectors;
    uint32_t fatSize = bpb.tableSize;

    uint32_t dataStart = fatStart + (bpb.fatCopies * fatSize);

    uint32_t rootStart = dataStart + bpb.sectorsPerCluster*(bpb.rootCluster-2);

    DirectoryEntryFAT32 dirEntry[16];
    hd->Read28(rootStart, (uint8_t*)&dirEntry[0], sizeof(DirectoryEntryFAT32)*16);

    for (int i=0; i<16; i++){
        if (dirEntry[i].name[0] == 0x00){
            printf("\nNo More Entries");
            break;
        }

        if (dirEntry[i].attributes == 0x0F){
            continue;
        }

        char* name = "        \n";
        for (int j=0; j<8; j++){
            name[j] = dirEntry[i].name[j];
        }
        printf(name);
        
        if ((dirEntry[i].attributes & 0x10) == 0x10){
            continue;
        }

        uint32_t firstCluster = ((uint32_t)dirEntry[i].firstClusterHi) << 16
                             | ((uint32_t)dirEntry[i].firstClusterLo); 

        int32_t SIZE = dirEntry[i].size;
        int32_t nextCluster = firstCluster;

        uint8_t buffer[513];
        uint8_t fatBuffer[513];

        while (SIZE > 0) {
            uint32_t fileSector = dataStart + bpb.sectorsPerCluster*(nextCluster-2); 
            int sectorOffset = 0;

            for (; SIZE > 0; SIZE -= 512){
                hd->Read28(fileSector + sectorOffset, buffer, 512);

                buffer[SIZE > 512 ? 512 : SIZE] = '\0';
                printf((char*)buffer);

                if (++sectorOffset > bpb.sectorsPerCluster){
                    break;
                }
            }

            uint32_t fatSectorForCurrentCluster = nextCluster / (512/sizeof(uint32_t));
            hd->Read28(fatStart+fatSectorForCurrentCluster, fatBuffer, 512);

            uint32_t fatOffsetInSectorForCurrentCluster = nextCluster % (512/sizeof(uint32_t));
            nextCluster = ((uint32_t*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;
        }
    }
};