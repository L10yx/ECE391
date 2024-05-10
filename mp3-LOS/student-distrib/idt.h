#ifndef IDT_H
#define IDT_H

#include "system_calls.h"

/* Exception handlers
 * Description: Exception handler to be called from assembly linkage 
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Prints error onto terminal and program loops */
extern void idt_init(void);
extern void divide_error(void);
extern void debug_exception(void);
extern void nmi_interrupt(void);
extern void breakpoint_exception(void);
extern void overflow_exception(void);
extern void bound_re(void);
extern void invalid_opcode(void);
extern void device_na(void);
extern void double_fault(void);
extern void coprocessor_so(void);
extern void invalid_tss(void);
extern void segment_np(void);
extern void stack_fault(void);
extern void general_protection(void);
extern void page_fault(void);
//extern void reserved_exc(void);
extern void FPUfpe(void);
extern void alignment_check(void);
extern void machine_check(void);
extern void simd_fp(void);
extern void rtc_interrupt_handler(void);
extern void handle_keyboard_interrupt(void);
#endif
