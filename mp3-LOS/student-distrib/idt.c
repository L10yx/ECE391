#include "idt.h"
#include "lib.h"
#include "x86_desc.h"
#include "exceptions_linkage.h"
#include "rtc.h"
#include "system_calls.h"


/* Exception handlers
 * Description: Exception handler to be called from assembly linkage 
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Prints error onto terminal and program loops */
void divide_error(){
    printf("Divide Error Exception\n");
    while(1);
}

void debug_exception(){
    printf("Debug Exception\n");
    while(1);
}

void nmi_interrupt(){
    printf("NMI Interrupt\n");
    while(1);
}

void breakpoint_exception(){
    printf("Breakpoint Exception\n");
    while(1);
}

void overflow_exception(){
    printf("Overflow Exception\n");
    while(1);
}

void bound_re(){
    printf("BOUND Range Exceeded Exception\n");
    while(1);
}

void invalid_opcode(){
    printf("Invalid Opcode Exception\n");
    while(1);
}

void device_na(){
    printf("Device Not Available Exception\n");
    while(1);
}

void double_fault(){
    printf("Double Fault Exception\n");
    while(1);
}

void coprocessor_so(){
    printf("Coprocessor Segment Overun\n");
    while(1);
}

void invalid_tss(){
    printf("Invalid TSS Exception\n");
    while(1);
}

void segment_np(){
    printf("Segment Not Present\n");
    while(1);
}

void stack_fault(){
    printf("Stack Fault Exception\n");
    while(1);
}

void general_protection(){
    printf("General Protection Exception\n");
    while(1);
}

void page_fault(){
    printf("Page-Fault Exception\n");
    while(1);
}

// void reserved_exc(){ 
//     printf("EXCEPTION Reserved\n");
//     while(1);
// }

void FPUfpe(){
    printf("x87 FPU Floating Point Error\n");
    while(1);
}

void alignment_check(){
    printf("Alignment Check Exception\n");
    while(1);
}

void machine_check(){
    printf("Machine-Check Exception\n");
    while(1);
}

void simd_fp(){
    printf("SIMD Floating-Point Exception\n");
    while(1);
}

/* idt_init
 * Description: Initialize the 256 entries of the IDT and link them to the assembly linkage.
                Save idt address onto idtr after init is complete.
 * Inputs: None
 * Outputs: None
 * Return Value: None
 * Side Effects: Changes IDT memory */
void idt_init() {
    int i;

    /* Loop through all 256 vectors in idt and initialize with default values */
    for(i=0; i<NUM_VEC; i++){
        idt[i].offset_15_00 = 0x0;
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0x0;
        idt[i].reserved3 = 0x0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0x0;
        idt[i].dpl = 0;
        idt[i].present = 0x0;
        idt[i].offset_31_16 = 0x0;
    }

    SET_IDT_ENTRY(idt[0], divide_error_linkage);               /* Divide Error Exception */
    SET_IDT_ENTRY(idt[1], debug_exception_linkage);            /* Debug Exception */
    SET_IDT_ENTRY(idt[2], nmi_interrupt_linkage);               /* NMI Interrupt */
    SET_IDT_ENTRY(idt[3], breakpoint_exception_linkage);       /* Breakpoint Exception */
    SET_IDT_ENTRY(idt[4], overflow_exception_linkage);          /* Overflow Exception */
    SET_IDT_ENTRY(idt[5], bound_re_linkage);                  /* BOUND Range Exceeded Exception */
    SET_IDT_ENTRY(idt[6], invalid_opcode_linkage);             /* Invalid Opcode Exception */
    SET_IDT_ENTRY(idt[7], device_na_linkage);                 /* Device Not Available Exception */
    SET_IDT_ENTRY(idt[8], double_fault_linkage);                /* Double Fault Exception */
    SET_IDT_ENTRY(idt[9], coprocessor_so_linkage);           /* Coprocessor Segment Overun */
    SET_IDT_ENTRY(idt[10], invalid_tss_linkage);               /* Invalid TSS Exception */
    SET_IDT_ENTRY(idt[11], segment_np_linkage);               /* Segment Not Present */
    SET_IDT_ENTRY(idt[12], stack_fault_linkage);               /* Stack Fault Exception */
    SET_IDT_ENTRY(idt[13], general_protection_linkage);         /* General Protection Exception */
    SET_IDT_ENTRY(idt[14], page_fault_linkage);                /* Page-Fault Exception */
    // SET_IDT_ENTRY(idt[15], 0x00);                        Reserved by Intel
    SET_IDT_ENTRY(idt[16], FPUfpe_linkage);                   /* x87 FPU Floating Point Error */
    SET_IDT_ENTRY(idt[17], alignment_check_linkage);            /* Alignment Check Exception */
    SET_IDT_ENTRY(idt[18], machine_check_linkage);            /* Machine-Check Exception */
    SET_IDT_ENTRY(idt[19], simd_fp_linkage);                  /* SIMD Floating-Point Exception */
    

    idt[0x20].reserved3 = 1;
    idt[0x21].reserved3 = 0x1;
    idt[0x21].present = 1;
    idt[0x28].reserved3 = 0x1;
    SET_IDT_ENTRY(idt[0x20], pit_linkage);
    SET_IDT_ENTRY(idt[0x21], keyboard_linkage);             /* Keyboard interrupt handler*/
    SET_IDT_ENTRY(idt[0x28], rtc_interrupt_linkage);        /* RTC interrupt handler */
    

    SET_IDT_ENTRY(idt[0x80], system_call_linkage);
    idt[0x80].dpl = 3;
    idt[0x80].reserved3 = 0x1;
    /* Load idtr with attributes*/
    lidt(idt_desc_ptr);
}

