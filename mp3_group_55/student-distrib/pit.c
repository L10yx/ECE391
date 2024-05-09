#define pit_data_port_0 0x40
#define pit_command 0x36
#define freq 20 
#define pit_IRQ_num 0
#define BASE_FREQ 11931820

#include "pit.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

unsigned int counter;

/* 
 * pit_init
 * Initialize the Programmable Interval Timer (PIT). PIT oscillator chip runs at
 * about 1.193182 MHz freq, which is the set base frequency. Dividing this freq will
 * derive other desired frequencies.
 * Inputs: None
 * Outputs: None
 * Side Effects: Enables IRQ0 to use for PIT
 */
void pit_init() {
    int divided;
    divided = BASE_FREQ / freq;
    enable_irq(pit_IRQ_num);

    /* Set command byte 
     Bits 6-7: 00 = Channel 0
     Bits 4-5: 11 = Access mode lobyte/hibyte
     Bits 1-3: 011 = Mode 3 (Square Wave Generator)
     Bit 0: 0 = 16-bit binary mode (values x0000 - xFFFF for counter)
     pit_command = 00 11 011 0 = x36 */
    outb(pit_command, pit_data_port_0);

    // Write to low byte first (lobyte/hibyte access mode)
    outb(divided & 0xFF, pit_data_port_0);

    // Write to high byte second
    outb((divided & 0xFF00) >> 8, pit_data_port_0);

    counter = 0;
}

void pit_interrupt_handler() {
    cli();
    // printf("%u\n", counter);
    // if (counter % 20 == 0) printf("%u\n", counter);
    counter++;

    send_eoi(pit_IRQ_num);
    // printf("do schedule\n");
    sti();
    if (counter % 2 == 0) {
        // scheduler_rr();
    }
    
}
