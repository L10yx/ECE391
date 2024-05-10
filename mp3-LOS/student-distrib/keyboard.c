#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal_driver.h"
#include "rtc.h"

int SHIFTED;
int CAPS_LOCK;
int CONTROL;
int capslock_flag = 0;
int ctrl_flag = 0;
int shift_flag = 0;
int alt_flag = 0;
int numlock_flag = 0;

char ASCII[][2] = {
        {0,0},
        {ASCII_ESC,ASCII_ESC},
        {'1','!'},
        {'2','@'},
        {'3','#'},
        {'4','$'},
        {'5','%'},
        {'6','^'},
        {'7','&'},
        {'8','*'},
        {'9','('},
        {'0',')'},
        {'-','_'},
        {'=','+'},
        {ASCII_BACKSPACE,ASCII_BACKSPACE},
        {ASCII_TAB, ASCII_TAB},
        {'q','Q'},
        {'w','W'},
        {'e','E'},
        {'r','R'},
        {'t','T'},
        {'y','Y'},
        {'u','U'},
        {'i','I'},
        {'o','O'},
        {'p','P'},
        {'[','{'},
        {']','}'},
        {ASCII_ENTER, ASCII_ENTER},
        {0,0},          //LCTRL
        {'a','A'},
        {'s','S'},
        {'d','D'},
        {'f','F'},
        {'g','G'},
        {'h','H'},
        {'j','J'},
        {'k','K'},
        {'l','L'},
        {';',':'},
        {ASCII_SINGLE_QUOTE,'"'},
        {0,0},          //TODO: BACKTICK
        {0,0},          //lshift
        {ASCII_BACKSLASH,ASCII_VERTICAL_BAR},
        {'z','Z'},
        {'x','X'},
        {'c','C'},
        {'v','V'},
        {'b','B'},
        {'n','N'},
        {'m','N'},
        {',','<'},
        {'.','>'},
        {'/','?'},
        {'z','Z'},
        {'x','X'},
        {'c','C'},
        {' ',' '},
        {'b','B'},
        {'n','N'},
        {'m','M'},
        {',','<'},
        {'.','>'},
        {'/','?'},
        {0,0},          //rshift
        {'*','*'},
        {0,0},          //LALT
        {' ', ' '},
        {0,0},          //CAPS LOCK
        {0,0},          //f1
        {0,0},          //f2
        {0,0},          //f3
        {0,0},          //f4
        {0,0},          //f5
        {0,0},          //f6
        {0,0},          //f6
        {0,0},          //f7
        {0,0},          //f8
        {0,0},          //f9
        {0,0},          //f10
        {0,0},          //NUMLOCK
        {0,0},          //SCROLL LOCK
        {'7','7'},      //KEYPAD START
        {'8','8'},
        {'9','9'},
        {'-','-'},
        {'4','4'},
        {'5','5'},
        {'6','6'},
        {'+','+'},
        {'1','1'},
        {'2','2'},
        {'3','3'},
        {'0','0'},
        {'0','0'},      //KEYPAD END
        {0,0},          //0x54
        {0,0},          //0x55
        {0,0},          //0x56
        {0,0},          //0x57
        {0,0},

    };

void handle_keyboard_interrupt(void) {
//     //Local Variables
    int released;
    int alt_char;
    char chara;
    uint8_t input;
    send_eoi(KBD_IRQ_NUM);
    
    
    cli();
    // disable_irq(RTC_IRQ_NUM);
    //clear();
	// printf("Keyboard Interrupt Called!\n");		//Debug

    // //Set Default Values
    // released = false;

    // //READ PORT 0x60
    input = inb(KEYBOARD_PORT);
    // printf("SCANCODE: %x\n", input); //Debug

    //Determine if is being released
    released = 0;
    if (input > 0x80) {
        input -= 0x80;
        released = 1;
		//printf("Key Released!\n"); 		//Debug
    }
    switch (input) {
        case CAPS_LOCK_PRESSED:		
            capslock_flag = !capslock_flag;	
            break;
        case NUM_LOCK_PRESSED:		
            numlock_flag = !numlock_flag;	
            break;
        case LSHIFT_PRESSED:	
            shift_flag = 1;		
            break;
        case RALT_PRESSED:	
            shift_flag = 1;		
            break;
        case LSHIFT_RELEASED:	
            shift_flag = 0;		
            break;
        case RALT_RELEASED:	
            shift_flag = 0;		
            break;
		case RSHIFT_PRESSED:
            ctrl_flag = 1;
            break;
        case RSHIFT_RELEASED:
            ctrl_flag = 0;
            break;
		case LALT_PRESSED:
            alt_flag = 1;
            // printf("ALT PRESS\n");
            break;
		case LALT_RELEASED:
            alt_flag = 0;
            // printf("ALT RELEASE\n");
            break;


		case F1_PRESSED:
            // printf("F1 PRESS\n");
            // printf("alt_press: %d\n",alt_press);
            if(alt_flag) {
                terminal_switch(0);  
            }
            break;
		case F2_PRESSED:	
            // printf("F2 PRESS\n");	
            if(alt_flag)
                terminal_switch(1);
            break;
		case F3_PRESSED:		
            // printf("F3 PRESS\n");
            if(alt_flag)
                terminal_switch(2);
            break;
        default:
            //Process Inputs Without ASCII VALUES
            if (ASCII[input][0] == 0) {
                switch(input) {
                    //case SCANCODE_CAPS_LOCK: if (!released) CAPS_LOCK = (!CAPS_LOCK);
                    case SCANCODE_LSHIFT: 
                        SHIFTED = released ^ 1;
                    case SCANCODE_RSHIFT: 
                        SHIFTED = !released;
                    case (SCANCODE_LCONTROL) : 
                        CONTROL = (released ^ 1);
                        printf("PRESS LCTL");
                        break;
                    default: // printf("Key Not Implemented! Scancode: %x\n", input);
                        printf("");
                }
                send_eoi(KBD_IRQ_NUM);
                sti();
                // enable_irq(RTC_IRQ_NUM);
                return;
            }


        //    //Determine if we are using the shifted character based on whether or not its a letter
        //     else if (ASCII[input][0] >= ASCII_a && ASCII[input][0] <= ASCII_z) alt_char = CAPS_LOCK ^ SHIFTED;
        //     else alt_char = SHIFTED;

            //Determine if we are using the alternative character 
            alt_char = SHIFTED;
            if (ASCII[input][0] >= ASCII_a && ASCII[input][0] <= ASCII_z) alt_char ^= CAPS_LOCK;

            //Determine Character of the Scancode from table
            chara = ASCII[input][alt_char];

            //Clear Command
            if (ctrl_flag && chara == 'l') {
                clear();
                key_buffer_clear();
            }

            //Print the Character and send to buffer;
            if (released == 0 && CONTROL == 0) {
                // if (of_flag == 0 || chara == ASCII_ENTER) putc(chara);
                key_buffer_push(chara);
            }
            break;
    }
    

    
    send_eoi(KBD_IRQ_NUM);
    // outb(0x22, 0x20);
    sti();
    // enable_irq(RTC_IRQ_NUM);
}

void key_init(void) { // https://wiki.osdev.org/Keyboard
	printf("Running key_init...\n");
    enable_irq(KBD_IRQ_NUM);
	CAPS_LOCK = 0;
	SHIFTED = 0;
    CONTROL = 0;
}
