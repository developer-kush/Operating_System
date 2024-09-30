
#include <drivers/mouse.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printHex(uint8_t key);
void printf(char* str);
void clrscr();

// ==================================== MOUSE EVENT HANDLER =====================================

MouseEventHandler::MouseEventHandler(){};

void MouseEventHandler::OnActivate(){};
void MouseEventHandler::OnMouseDown(uint8_t button){};
void MouseEventHandler::OnMouseUp(uint8_t button){};
void MouseEventHandler::OnMouseMove(int x, int y){};

// ======================================== MOUSE DRIVER ========================================

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(manager, 0x2C), dataport(0x60), commandport(0x64) {
    this->handler = handler;
}

void MouseDriver::Activate(){

    offset = 0;
    buttons = 0;
    x = 40;
    y = 12;

    static uint16_t* VideoMemory = (uint16_t*) 0xB8000;

    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xF000) >> 4 
                        | (VideoMemory[80*y+x] & 0x0F00) << 4 
                        | (VideoMemory[80*y+x] & 0x00FF);
    
    commandport.Write(0xA8);
    commandport.Write(0x20);

    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);

    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver(){

}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    uint8_t status = commandport.Read();
    if (!(status & 0x20)){
        return esp;
    }
    
    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    if (offset == 0){

        if (buffer[1] != 0 || buffer[2] != 0){

            static uint16_t* VideoMemory = (uint16_t*) 0xB8000;

            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xF000) >> 4 
                                | (VideoMemory[80*y+x] & 0x0F00) << 4 
                                | (VideoMemory[80*y+x] & 0x00FF);

            x += buffer[1];
            y -= buffer[2];

            if (x < 0) x = 0;
            if (x >= 80) x = 79;
            if (y < 0) y = 0;
            if (y >= 25) y = 24;

            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xF000) >> 4 
                                | (VideoMemory[80*y+x] & 0x0F00) << 4 
                                | (VideoMemory[80*y+x] & 0x00FF);

        }
    }
    return esp;
}