/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* 
 * i8259_init(void)
 * Initialize the 8259 PIC
 * Inputs: void
 * Outputs: none
 */
void i8259_init(void) {
    master_mask = MASK_INIT;
    slave_mask = MASK_INIT;

    /* mask all of 8259A-1 and 8259A-2 */
    outb(MASK_INIT, MASTER_8259_WRITE_PORT);
    outb(MASK_INIT, SLAVE_8259_WRITE_PORT);           

    /* Any command with A = 0 (writing to 0x20) and D4 = 1 is recognized as ICW1.*/
    outb(ICW1, MASTER_8259_PORT);               // ICW1: select 8259A-1 master init
    outb(ICW2_MASTER, MASTER_8259_WRITE_PORT);	// ICW2: 8259A-1 IR0-7 mapped to 0x20 - 0x27
    outb(ICW3_MASTER, MASTER_8259_WRITE_PORT);	// ICW3: 8259A-1(master) has a slave on IR2
    outb(ICW4, MASTER_8259_WRITE_PORT);	        // ICW4: master expects normal EOI

    outb(ICW1, SLAVE_8259_PORT);	            // ICW1: select 8259A-2 slave init
    outb(ICW2_SLAVE, SLAVE_8259_WRITE_PORT);	// ICW2: 8259A-2 IR0-7 mapped to 0x28 - 0x2f
    outb(ICW3_SLAVE, SLAVE_8259_WRITE_PORT);    // ICW3: 8259A-2 is a slave on master's IR2
    outb(ICW4, SLAVE_8259_WRITE_PORT);	        // ICW4: slave expects normal EOI

    enable_irq(2);                              // enable IR2 for slave
}

/* 
 * enable_irq(uint32_t irq_num)
 * Enable (unmask) the specified IRQ, 0 to indicate unmask
 * Inputs: irq_num
 * Outputs: none
 */
void enable_irq(uint32_t irq_num) {
    if (irq_num > MAX_IRQ) return; // out of range
    if (irq_num <= MAX_MASTER_IRQ) { // master
        master_mask &= (~(1 << irq_num));
        outb(master_mask, MASTER_8259_WRITE_PORT);       
        return;         
    }
    master_mask &= (~(1 << (irq_num - MAX_MASTER_IRQ - 1))); // slave
    outb(slave_mask, SLAVE_8259_WRITE_PORT);       
    return; 
}

/* 
 * disable_irq(uint32_t irq_num)
 * disable (mask) the specified IRQ, 1 to indicate mask
 * Inputs: irq_num
 * Outputs: none
 */
void disable_irq(uint32_t irq_num) {
    if (irq_num > MAX_IRQ) return; // out of range
    if (irq_num <= MAX_MASTER_IRQ) { // master
        master_mask |= (~(1 << irq_num));
        outb(master_mask, MASTER_8259_WRITE_PORT);       
        return;         
    }
    master_mask |= (~(1 << (irq_num - MAX_MASTER_IRQ - 1))); // slave
    outb(slave_mask, SLAVE_8259_WRITE_PORT);       
    return; 
}

/* 
 * send_eoi(uint32_t irq_num)
 * Send end-of-interrupt signal for the specified IRQ
 * Inputs: irq_num
 * Outputs: none
 */
void send_eoi(uint32_t irq_num) {
    /* End-of-interrupt byte.  This gets OR'd with
     * the interrupt number and sent out to the PIC
     * to declare the interrupt finished */
    
    if (irq_num > MAX_IRQ) return; // out of range
    if (irq_num <= MAX_MASTER_IRQ) { // master
        outb(EOI | (~(1 << irq_num)), MASTER_8259_PORT);       
        return;         
    }
    // slave
    outb(EOI | (~(1 << irq_num - MAX_MASTER_IRQ - 1)), SLAVE_8259_PORT);
    outb(EOI | 2, MASTER_8259_PORT); // also send to master IR2
    return; 
}
