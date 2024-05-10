#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#define FREQ 1024
// bool INTERRUPT_HAPPEN;

/* 
 * rtc_init(void)
 * Initialize rtc
 * Inputs: void
 * Outputs: none
 */
void rtc_init() {   //https://wiki.osdev.org/RTC
    unsigned char prev;

    //cli();
    outb(RTC_REG_B, RTC_PORT);          // select register B, and disable NMI
    prev = inb(RTC_W_PORT);             // read the current value of register B
    outb(RTC_REG_B,RTC_PORT);           // set the index again (a read will reset the index to register D)
    outb(prev | BIT_SIX, RTC_W_PORT);   // write the previous value ORed with 0x40. This turns on bit 6 of register B
    enable_irq(RTC_IRQ_NUM);    

    // outb(RTC_REG_A, RTC_PORT);                              // selecting register A and disabling NMI's
    // char save = inb(RTC_W_PORT);                            // reading previous value of A 
    // outb(RTC_REG_A ,RTC_PORT);                              // selecting A again 
    // outb(((save & 0xF0) | 15), RTC_W_PORT);                 // getting correct rate
    num_interrupts_needed = FREQ;
    num_interrupts_generated = 0;
    rtc_read_flag = 0;
    //sti();

}


/*
 * void rtc_interrupt_handler()
 * Handles an RTC interrupt when it happens
 * Inputs:  none
 * Outputs: returns 1
 */
void rtc_interrupt_handler() {

    // printf("RTC");

    cli();
    if(num_interrupts_generated == num_interrupts_needed){
        rtc_read_flag = 1;
        // printf("ret 11111\n");
        num_interrupts_generated = 0;
        // printf("RTCif");
    }else{
        num_interrupts_generated = num_interrupts_generated + 1;
    }
    outb((RTC_REG_C), RTC_PORT);      // select register C
    // printf("RTC1");
    inb(RTC_W_PORT);                                // toss the contents
    // printf("RTC2");
    send_eoi(RTC_IRQ_NUM);                          // end of interrupt
    // printf("RTC3");
    //rtc_open(NULL);
    sti();
    // printf("RTC4");
    // test_interrupts();

    // INTERRUPT_HAPPEN = 1;                           // sending signal that an interrupt happened
}

/*
 * int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
 * Waits for an interrupt and then returns 0
 * Inputs:  fd (ignored), buf (ignored), nbytes (ignored)
 * Outputs: returns zero
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    // rtc_read_flag = 0;
    printf("");                                 // for whatever reason removing this print breaks the rtc...
    while(rtc_read_flag != 1);
    rtc_read_flag = 0;                          // resetting signal
    return 0;
}

/*
 * int32_t write(int32_t fd, const void* buf, int32_t)
 * Changes the frequency of the RTC interrupts without changing the RTC's ability to keep time
 * Inputs:  fd (ignored), buf (pointer to requested frequency), nbytes (ignored)
 * Outputs: returns zero on success and -1 on failure
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){

    uint32_t rate = *(uint32_t*) buf;

    if(rate < 2 || rate > FREQ){
        return -1;                  // rate is outside allowed range                                       
    }
    if((rate & ((rate-1))) != 0){                             
        return -1;                  // is not a power of two
    }
    num_interrupts_needed = FREQ/rate;
    // // uint32_t divider = divider_calc(rate); 
    // uint32_t divider;

    // divider = 32768 << rate;
    // divider--;
    // // if(rate == 2){ divider = 15; }                  // calculating correct divider
    // // else if(rate == 4){ divider = 14; }
    // // else if(rate == 8){ divider = 13; }
    // // else if(rate == 16){ divider = 12; }
    // // else if(rate == 32){ divider = 11; }
    // // else if(rate == 64){ divider = 10; }
    // // else if(rate == 128){ divider = 9; }
    // // else if(rate == 256){ divider = 8; }
    // // else if(rate == 512){ divider = 7; }
    // // else{ divider = 6; }

    // cli();
    // outb(RTC_REG_A, RTC_PORT);                              // selecting register A and disabling NMI's
    // char prev = inb(RTC_W_PORT);                            // reading previous value of A 
    // outb(RTC_REG_A ,RTC_PORT);                              // selecting A again 
    // outb(((prev & 0xF0) | divider), RTC_W_PORT);            // writing only bottom 4 bits of register A
    //                                                         // ANDing prev with 0xF0 saves the highest four 
    //                                                         // bits of the previous value of A, then ORing
    //                                                         // sets the new desired rate into the lowest bits 
    // sti();

    return 0;                                               // success!
}


/*
 * int32_t rtc_open(const uint_t* filename)
 * resets the RTC to the default frequency (2Hz)
 * Inputs:  filename (ignored)
 * Outputs: returns 0
 */
int32_t rtc_open(const uint8_t* filename){
    num_interrupts_generated = 0;
    // num_interrupts_needed = FREQ/2;
    return 0;
}


/*
 * int32_t rtc_close(int32_t fd)
 * does nothing
 * Inputs:  fd (ignored)
 * Outputs: returns 0
 */
int32_t rtc_close(int32_t fd){
    return 0;
}
