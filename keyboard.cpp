
#include "keyboard.h"

void printHex(uint8_t key);
void printf(char* str);
void clrscr();

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x21), dataport(0x60), commandport(0x64) {
    while (commandport.Read() & 0x1){
        dataport.Read();
    }
    commandport.Write(0xae);
    commandport.Write(0x20);

    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60);

    dataport.Write(status);
    dataport.Write(0xf4);
}

KeyboardDriver::~KeyboardDriver(){

}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){

    uint8_t key = dataport.Read();

    static bool ctrl = false;
    static bool shift = false;
    static bool CapsLock = false;

    switch(key){
        case 0x3A: CapsLock = !CapsLock; break; // Caps Lock hold
        case 0x2A: shift = !shift; break; // Left Shift hold
        case 0xAA: shift = !shift; break; // Left Shift release
        case 0x36: shift = !shift; break; // Right Shift hold
        case 0xB6: shift = !shift; break; // Right Shift release
        case 0x1D: ctrl = true; break; // Left Control hold
        case 0x9D: ctrl = false; break; // Left Control release
        default: break;
    }

    // printHex(key);

    if (key < 0x80){ 
        switch(key){

            case 0x3A: break; // Caps Lock
            case 0x2A: break; // Left Shift
            case 0x36: break; // Right Shift
            case 0x1D: break; // Left Control
            case 0x01: break; // Escape (ESC)

            case 0x1E: printf((char*)((CapsLock || shift)?"A":"a")); break;
            case 0x30: printf((char*)((CapsLock || shift)?"B":"b")); break;
            case 0x2E: printf((char*)((CapsLock || shift)?"C":"c")); break;
            case 0x20: printf((char*)((CapsLock || shift)?"D":"d")); break;
            case 0x12: printf((char*)((CapsLock || shift)?"E":"e")); break;
            case 0x21: printf((char*)((CapsLock || shift)?"F":"f")); break;
            case 0x22: printf((char*)((CapsLock || shift)?"G":"g")); break;
            case 0x23: printf((char*)((CapsLock || shift)?"H":"h")); break;
            case 0x17: printf((char*)((CapsLock || shift)?"I":"i")); break;
            case 0x24: printf((char*)((CapsLock || shift)?"J":"j")); break;
            case 0x25: printf((char*)((CapsLock || shift)?"K":"k")); break;
            case 0x26: if(ctrl) clrscr(); else printf((char*)((CapsLock || shift)?"L":"l")); break;
            case 0x32: printf((char*)((CapsLock || shift)?"M":"m")); break;
            case 0x31: printf((char*)((CapsLock || shift)?"N":"n")); break;
            case 0x18: printf((char*)((CapsLock || shift)?"O":"o")); break;
            case 0x19: printf((char*)((CapsLock || shift)?"P":"p")); break;
            case 0x10: printf((char*)((CapsLock || shift)?"Q":"q")); break;
            case 0x13: printf((char*)((CapsLock || shift)?"R":"r")); break;
            case 0x1F: printf((char*)((CapsLock || shift)?"S":"s")); break;
            case 0x14: printf((char*)((CapsLock || shift)?"T":"t")); break;
            case 0x16: printf((char*)((CapsLock || shift)?"U":"u")); break;
            case 0x2F: printf((char*)((CapsLock || shift)?"V":"v")); break;
            case 0x11: printf((char*)((CapsLock || shift)?"W":"w")); break;
            case 0x2D: printf((char*)((CapsLock || shift)?"X":"x")); break;
            case 0x15: printf((char*)((CapsLock || shift)?"Y":"y")); break;
            case 0x2C: printf((char*)((CapsLock || shift)?"Z":"z")); break;

            case 0x1A: printf((char*)(shift?"{":"[")); break;
            case 0x1B: printf((char*)(shift?"}":"]")); break;
            case 0x2B: printf((char*)(shift?"|":"\\")); break;
            case 0x27: printf((char*)(shift?":":";")); break;
            case 0x28: printf((char*)(shift?"\"":"'")); break;
            case 0x33: printf((char*)(shift?"<":",")); break;
            case 0x34: printf((char*)(shift?">":".")); break;
            case 0x35: printf((char*)(shift?"?":"/")); break;

            case 0x29: printf((char*)(shift?"~":"`")); break;
            case 0x02: printf((char*)(shift?"!":"1")); break;
            case 0x03: printf((char*)(shift?"@":"3")); break;
            case 0x04: printf((char*)(shift?"#":"2")); break;
            case 0x05: printf((char*)(shift?"$":"4")); break;
            case 0x06: printf((char*)(shift?"%":"5")); break;
            case 0x07: printf((char*)(shift?"^":"6")); break;
            case 0x08: printf((char*)(shift?"&":"7")); break;
            case 0x09: printf((char*)(shift?"*":"8")); break;
            case 0x0A: printf((char*)(shift?"(":"9")); break;
            case 0x0B: printf((char*)(shift?")":"0")); break;
            case 0x0C: printf((char*)(shift?"_":"-")); break;
            case 0x0D: printf((char*)(shift?"+":"=")); break;

            case 0x0E: printf("\b"); break;
            case 0x0F: printf("\t"); break;
            case 0x39: printf(" "); break;
            case 0x1C: printf("\n"); break;

            case 0x45: printf("NumLock"); break;

            default:
                printf("KEYBOARD !!!");
                printHex(key);
                printf("\n");
        }
    }

    return esp;
}