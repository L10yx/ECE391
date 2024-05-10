#ifndef RTC_H
#define RTC_H

#include "types.h"

/*The 2 IO ports used for the RTC and CMOS are 0x70 and 0x71. Port 0x70 is used to specify an index or "register number", 
and to disable NMI. Port 0x71 is used to read or write from/to that byte of CMOS configuration space. 
Only three bytes of CMOS RAM are used to control the RTC periodic interrupt function. 
They are called RTC Status Register A, B, and C. They are at offset 0xA, 0xB, and 0xC in the CMOS RAM. */
#define RTC_PORT        0x70
#define RTC_W_PORT      0x71
#define RTC_REG_A       0x8A // RegA and disable NMI
#define RTC_REG_B       0x8B
#define RTC_REG_C       0x8C

#define DISABLE_NMI     0x80

#define RTC_IRQ_NUM     8
#define BIT_SIX         0x40
#define RTC_MAX_RATE    32768

int num_interrupts_needed;
int num_interrupts_generated;
int rtc_read_flag;

// initialize rtc 
void rtc_init(void);

// RTC interrupt handler
void rtc_interrupt_handler(void);

// checkpoint 2 functions
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);

// helper for write
// uint32_t divider_calc(uint32_t rate);

#endif
