#ifndef EXCEPTIONS_LINKAGE_H
#define EXCEPTIONS_LINKAGE_H


/* Exception handlers initialized in idt.c
 * Description: Exception handler to be called from assembly linkage 
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Prints error onto terminal and program loops */

extern void system_call_linkage(void);
extern void divide_error_linkage(void);
extern void debug_exception_linkage(void);
extern void nmi_interrupt_linkage(void);
extern void breakpoint_exception_linkage(void);
extern void overflow_exception_linkage(void);
extern void bound_re_linkage(void);
extern void invalid_opcode_linkage(void);
extern void device_na_linkage(void);
extern void double_fault_linkage(void);
extern void coprocessor_so_linkage(void);
extern void invalid_tss_linkage(void);
extern void segment_np_linkage(void);
extern void stack_fault_linkage(void);
extern void general_protection_linkage(void);
extern void page_fault_linkage(void);
extern void reserved_exc_linkage(void);
extern void FPUfpe_linkage(void);
extern void alignment_check_linkage(void);
extern void machine_check_linkage(void);
extern void simd_fp_linkage(void);
extern void rtc_interrupt_linkage(void);
extern void keyboard_linkage(void);
extern void pit_linkage(void);
#endif
