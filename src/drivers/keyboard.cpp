
#include <drivers/keyboard.h>
#include <drivers/driver.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


void printHex(uint8_t key);
void printf(char* str);
void clrscr();

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x21), dataport(0x60), commandport(0x64) {}

void KeyboardDriver::Activate(){
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
    static bool lshift = 0, rshift = 0;
    static bool CapsLock = false;

    switch(key){
        case 0x3A: CapsLock = !CapsLock; break; // Caps Lock hold
        case 0x2A: lshift = 1; break; // Left Shift hold
        case 0xAA: lshift = 0; break; // Left Shift release
        case 0x36: rshift = 1; break; // Right Shift hold
        case 0xB6: rshift = 0; break; // Right Shift release

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

            case 0x1E: printf((char*)((CapsLock || (lshift+rshift))?"A":"a")); break;
            case 0x30: printf((char*)((CapsLock || (lshift+rshift))?"B":"b")); break;
            case 0x2E: printf((char*)((CapsLock || (lshift+rshift))?"C":"c")); break;
            case 0x20: printf((char*)((CapsLock || (lshift+rshift))?"D":"d")); break;
            case 0x12: printf((char*)((CapsLock || (lshift+rshift))?"E":"e")); break;
            case 0x21: printf((char*)((CapsLock || (lshift+rshift))?"F":"f")); break;
            case 0x22: printf((char*)((CapsLock || (lshift+rshift))?"G":"g")); break;
            case 0x23: printf((char*)((CapsLock || (lshift+rshift))?"H":"h")); break;
            case 0x17: printf((char*)((CapsLock || (lshift+rshift))?"I":"i")); break;
            case 0x24: printf((char*)((CapsLock || (lshift+rshift))?"J":"j")); break;
            case 0x25: printf((char*)((CapsLock || (lshift+rshift))?"K":"k")); break;
            case 0x26: if(ctrl) clrscr(); else printf((char*)((CapsLock || (lshift+rshift))?"L":"l")); break;
            case 0x32: printf((char*)((CapsLock || (lshift+rshift))?"M":"m")); break;
            case 0x31: printf((char*)((CapsLock || (lshift+rshift))?"N":"n")); break;
            case 0x18: printf((char*)((CapsLock || (lshift+rshift))?"O":"o")); break;
            case 0x19: printf((char*)((CapsLock || (lshift+rshift))?"P":"p")); break;
            case 0x10: printf((char*)((CapsLock || (lshift+rshift))?"Q":"q")); break;
            case 0x13: printf((char*)((CapsLock || (lshift+rshift))?"R":"r")); break;
            case 0x1F: printf((char*)((CapsLock || (lshift+rshift))?"S":"s")); break;
            case 0x14: printf((char*)((CapsLock || (lshift+rshift))?"T":"t")); break;
            case 0x16: printf((char*)((CapsLock || (lshift+rshift))?"U":"u")); break;
            case 0x2F: printf((char*)((CapsLock || (lshift+rshift))?"V":"v")); break;
            case 0x11: printf((char*)((CapsLock || (lshift+rshift))?"W":"w")); break;
            case 0x2D: printf((char*)((CapsLock || (lshift+rshift))?"X":"x")); break;
            case 0x15: printf((char*)((CapsLock || (lshift+rshift))?"Y":"y")); break;
            case 0x2C: printf((char*)((CapsLock || (lshift+rshift))?"Z":"z")); break;

            case 0x1A: printf((char*)((lshift+rshift)?"{":"[")); break;
            case 0x1B: printf((char*)((lshift+rshift)?"}":"]")); break;
            case 0x2B: printf((char*)((lshift+rshift)?"|":"\\")); break;
            case 0x27: printf((char*)((lshift+rshift)?":":";")); break;
            case 0x28: printf((char*)((lshift+rshift)?"\"":"'")); break;
            case 0x33: printf((char*)((lshift+rshift)?"<":",")); break;
            case 0x34: printf((char*)((lshift+rshift)?">":".")); break;
            case 0x35: printf((char*)((lshift+rshift)?"?":"/")); break;

            case 0x29: printf((char*)((lshift+rshift)?"~":"`")); break;
            case 0x02: printf((char*)((lshift+rshift)?"!":"1")); break;
            case 0x03: printf((char*)((lshift+rshift)?"@":"3")); break;
            case 0x04: printf((char*)((lshift+rshift)?"#":"2")); break;
            case 0x05: printf((char*)((lshift+rshift)?"$":"4")); break;
            case 0x06: printf((char*)((lshift+rshift)?"%":"5")); break;
            case 0x07: printf((char*)((lshift+rshift)?"^":"6")); break;
            case 0x08: printf((char*)((lshift+rshift)?"&":"7")); break;
            case 0x09: printf((char*)((lshift+rshift)?"*":"8")); break;
            case 0x0A: printf((char*)((lshift+rshift)?"(":"9")); break;
            case 0x0B: printf((char*)((lshift+rshift)?")":"0")); break;
            case 0x0C: printf((char*)((lshift+rshift)?"_":"-")); break;
            case 0x0D: printf((char*)((lshift+rshift)?"+":"=")); break;

            case 0x0E: printf("\b"); break; // BackSpace
            case 0x0F: printf("\t"); break; // Tab
            case 0x39: printf(" "); break;  // Space
            case 0x1C: printf("\n"); break; // Enter

            case 0x45: printf("NumLock"); break;

            default:
                printf("KEYBOARD !!!");
                printHex(key);
                printf("\n");
        }
    }

    return esp;
}