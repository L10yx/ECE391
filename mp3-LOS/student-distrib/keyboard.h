#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define KBD_IRQ_NUM 1 // use IR1 of master

#define KEYBOARD_PORT 0x60
#define NUM_SCANCODES 0x58

#define ASCII_ESC 27
#define ASCII_TAB 9
#define ASCII_BACKSLASH 92
#define ASCII_VERTICAL_BAR 124
#define ASCII_CAPS_LOCK 20
#define ASCII_ENTER 10
#define ASCII_SINGLE_QUOTE 39
#define ASCII_SPACE 32
#define ASCII_BACKSPACE 8
#define ASCII_a 97
#define ASCII_z 122


#define SCANCODE_CAPS_LOCK 0x0E
#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x38
#define SCANCODE_LCONTROL 0x1D 

#define CAPS_LOCK_PRESSED    0x3A

#define LSHIFT_PRESSED   0x2A
#define LSHIFT_RELEASED  0xAA

#define RSHIFT_PRESSED    0x1D
#define RSHIFT_RELEASED   0x9D

#define LALT_PRESSED     0x38
#define LALT_RELEASED    0xB8

#define RALT_PRESSED  0x36
#define RALT_RELEASED 0xB6

#define NUM_LOCK_PRESSED     0x45

#define F1_PRESSED 0x3B
#define F2_PRESSED 0x3C
#define F3_PRESSED 0x3D
#define F4_PRESSED 0x3E
#define F5_PRESSED 0x3F
#define F6_PRESSED 0x40
#define F7_PRESSED 0x41
#define F8_PRESSED 0x42
#define F9_PRESSED 0x43
#define F10_PRESSED 0x44
#define F11_PRESSED 0x57
#define F12_PRESSED 0x58
// Reference: https://kbdlayout.info/kbdusx/scancodes

// initialize keyboard

extern void key_init(void);

extern void handle_keyboard_interrupt(void);

#endif
