#include <drivers/keyboard.h>
#include <drivers/driver.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printHex(uint8_t key);
void printf(char* str);
void clrscr();

// =================================== KEYBOARD EVENT HANDLER ======================================

KeyboardEventHandler::KeyboardEventHandler(){}
KeyboardEventHandler::~KeyboardEventHandler(){}

void KeyboardEventHandler::OnKeyDown(uint8_t key){
    switch (key){
        case KEY_LEFTSHIFT: leftShift = true; break;
        case KEY_RIGHTSHIFT: rightShift = true; break;
        case KEY_LEFTCTRL: leftCtrl = true; break;
        case KEY_CAPSLOCK: capsLock = !capsLock; break;

        case KEY_BACKSPACE: printf("\b"); break;
        case KEY_TAB: printf("\t"); break; 
        case KEY_SPACEBAR: printf(" "); break;
        case KEY_ENTER: printf("\n"); break;

        case KEY_A: (capsLock || leftShift || rightShift) ? printf("A") : printf("a"); break;
        case KEY_B: (capsLock || leftShift || rightShift) ? printf("B") : printf("b"); break;
        case KEY_C: (capsLock || leftShift || rightShift) ? printf("C") : printf("c"); break;
        case KEY_D: (capsLock || leftShift || rightShift) ? printf("D") : printf("d"); break;
        case KEY_E: (capsLock || leftShift || rightShift) ? printf("E") : printf("e"); break;
        case KEY_F: (capsLock || leftShift || rightShift) ? printf("F") : printf("f"); break;
        case KEY_G: (capsLock || leftShift || rightShift) ? printf("G") : printf("g"); break;
        case KEY_H: (capsLock || leftShift || rightShift) ? printf("H") : printf("h"); break;
        case KEY_I: (capsLock || leftShift || rightShift) ? printf("I") : printf("i"); break;
        case KEY_J: (capsLock || leftShift || rightShift) ? printf("J") : printf("j"); break;
        case KEY_K: (capsLock || leftShift || rightShift) ? printf("K") : printf("k"); break;
        case KEY_L: (leftCtrl) ? clrscr() :
                    (capsLock || leftShift || rightShift) ? printf("L") : printf("l"); break;
        case KEY_M: (capsLock || leftShift || rightShift) ? printf("M") : printf("m"); break;
        case KEY_N: (capsLock || leftShift || rightShift) ? printf("N") : printf("n"); break;
        case KEY_O: (capsLock || leftShift || rightShift) ? printf("O") : printf("o"); break;
        case KEY_P: (capsLock || leftShift || rightShift) ? printf("P") : printf("p"); break;
        case KEY_Q: (capsLock || leftShift || rightShift) ? printf("Q") : printf("q"); break;
        case KEY_R: (capsLock || leftShift || rightShift) ? printf("R") : printf("r"); break;
        case KEY_S: (capsLock || leftShift || rightShift) ? printf("S") : printf("s"); break;
        case KEY_T: (capsLock || leftShift || rightShift) ? printf("T") : printf("t"); break;
        case KEY_U: (capsLock || leftShift || rightShift) ? printf("U") : printf("u"); break;
        case KEY_V: (capsLock || leftShift || rightShift) ? printf("V") : printf("v"); break;
        case KEY_W: (capsLock || leftShift || rightShift) ? printf("W") : printf("w"); break;
        case KEY_X: (capsLock || leftShift || rightShift) ? printf("X") : printf("x"); break;
        case KEY_Y: (capsLock || leftShift || rightShift) ? printf("Y") : printf("y"); break;
        case KEY_Z: (capsLock || leftShift || rightShift) ? printf("Z") : printf("z"); break;

        case KEY_SQOPEN         : (leftShift || rightShift) ? printf("{") : printf("["); break;
        case KEY_SQCLOSE        : (leftShift || rightShift) ? printf("}") : printf("]"); break;
        case KEY_BACKSLASH      : (leftShift || rightShift) ? printf("|") : printf("\\"); break;
        case KEY_SEMICOLON      : (leftShift || rightShift) ? printf(":") : printf(";"); break;
        case KEY_QUOTE          : (leftShift || rightShift) ? printf("\"") : printf("'"); break;
        case KEY_COMMA          : (leftShift || rightShift) ? printf("<") : printf(","); break;
        case KEY_DOT            : (leftShift || rightShift) ? printf(">") : printf("."); break;
        case KEY_FORWARDSLASH   : (leftShift || rightShift) ? printf("?") : printf("/"); break;

        case KEY_BACKTICK       : (leftShift || rightShift) ? printf("~") : printf("`"); break;
        case KEY_1              : (leftShift || rightShift) ? printf("!") : printf("1"); break;
        case KEY_2              : (leftShift || rightShift) ? printf("@") : printf("3"); break;
        case KEY_3              : (leftShift || rightShift) ? printf("#") : printf("2"); break;
        case KEY_4              : (leftShift || rightShift) ? printf("$") : printf("4"); break;
        case KEY_5              : (leftShift || rightShift) ? printf("%") : printf("5"); break;
        case KEY_6              : (leftShift || rightShift) ? printf("^") : printf("6"); break;
        case KEY_7              : (leftShift || rightShift) ? printf("&") : printf("7"); break;
        case KEY_8              : (leftShift || rightShift) ? printf("*") : printf("8"); break;
        case KEY_9              : (leftShift || rightShift) ? printf("(") : printf("9"); break;
        case KEY_0              : (leftShift || rightShift) ? printf(")") : printf("0"); break;
        case KEY_MINUS          : (leftShift || rightShift) ? printf("_") : printf("-"); break;
        case KEY_EQUAL          : (leftShift || rightShift) ? printf("+") : printf("="); break;
    }
}

void KeyboardEventHandler::OnKeyUp(uint8_t key){
    switch (key){
        case KEY_LEFTSHIFT_RELEASE: leftShift = false; break;
        case KEY_RIGHTSHIFT_RELEASE: rightShift = false; break;
        case KEY_LEFTCTRL_RELEASE: leftCtrl = false; break;
    }
}

// ====================================== KEYBOARD DRIVER ==========================================

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
: InterruptHandler(manager, 0x21), dataport(0x60), commandport(0x64) {
    this->handler = handler;
}

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

    if (key < 0x80){ 
        this->handler->OnKeyDown(key);
    } else {
        this->handler->OnKeyUp(key);
    }

    return esp;
}