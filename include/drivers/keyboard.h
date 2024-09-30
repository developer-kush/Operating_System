
#ifndef __MYOS__DRIVERS__KEYBOARD_H
#define __MYOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos {
    namespace drivers {

        class KeyboardEventHandler {
            
            #define KEY_CAPSLOCK 0x3A
            #define KEY_LEFTSHIFT 0x2A
            #define KEY_RIGHTSHIFT 0x36
            #define KEY_LEFTCTRL 0x1D
            #define KEY_ESC 0x01
            #define KEY_NUMLOCK 0x45
            #define KEY_BACKSPACE 0x0E
            #define KEY_TAB 0x0F 
            #define KEY_SPACEBAR 0x39 
            #define KEY_ENTER 0x1C

            #define ARROW_LEFT 0x4B
            #define ARROW_RIGHT 0x4D
            #define ARROW_UP 0x48
            #define ARROW_DOWN 0x50

            #define KEY_A 0x1E
            #define KEY_B 0x30
            #define KEY_C 0x2E
            #define KEY_D 0x20
            #define KEY_E 0x12
            #define KEY_F 0x21
            #define KEY_G 0x22
            #define KEY_H 0x23
            #define KEY_I 0x17
            #define KEY_J 0x24
            #define KEY_K 0x25
            #define KEY_L 0x26
            #define KEY_M 0x32
            #define KEY_N 0x31
            #define KEY_O 0x18
            #define KEY_P 0x19
            #define KEY_Q 0x10
            #define KEY_R 0x13
            #define KEY_S 0x1F
            #define KEY_T 0x14
            #define KEY_U 0x16
            #define KEY_V 0x2F
            #define KEY_W 0x11
            #define KEY_X 0x2D
            #define KEY_Y 0x15
            #define KEY_Z 0x2C

            #define KEY_SQOPEN 0x1A 
            #define KEY_SQCLOSE 0x1B 
            #define KEY_BACKSLASH 0x2B 
            #define KEY_SEMICOLON 0x27 
            #define KEY_QUOTE 0x28 
            #define KEY_COMMA 0x33 
            #define KEY_DOT 0x34 
            #define KEY_FORWARDSLASH 0x35 

            #define KEY_BACKTICK 0x29
            #define KEY_1 0x02
            #define KEY_2 0x03
            #define KEY_3 0x04
            #define KEY_4 0x05
            #define KEY_5 0x06
            #define KEY_6 0x07
            #define KEY_7 0x08
            #define KEY_8 0x09
            #define KEY_9 0x0A
            #define KEY_0 0x0B
            #define KEY_MINUS 0x0C
            #define KEY_EQUAL 0x0D

            #define KEY_LEFTSHIFT_RELEASE 0xAA
            #define KEY_RIGHTSHIFT_RELEASE 0xB6
            #define KEY_LEFTCTRL_RELEASE 0x9D

            protected:
                bool leftShift, rightShift, leftCtrl, leftAlt, rightAlt, capsLock, numLock, scrollLock;

            public:
                KeyboardEventHandler();
                ~KeyboardEventHandler();

                virtual void OnKeyDown(myos::common::uint8_t key);
                virtual void OnKeyUp(myos::common::uint8_t key);
        };

        class KeyboardDriver : public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver {

            myos::hardwarecommunication::Port8Bit dataport;
            myos::hardwarecommunication::Port8Bit commandport;

            KeyboardEventHandler* handler;

            public:
                KeyboardDriver(myos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler* handler);
                ~KeyboardDriver();
                virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
                virtual void Activate();
        };

    }
}

#endif