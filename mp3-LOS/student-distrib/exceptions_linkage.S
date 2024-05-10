#define vector0 0
#define vector1 1
#define vector2 2
#define vector3 3
#define vector4 4
#define vector5 5
#define vector6 6
#define vector7 7
#define vector8 8
#define vector9 9
#define vector10 10
#define vector11 11
#define vector12 12
#define vector13 13
#define vector14 14
#define vector15 15
#define vector16 16
#define vector17 17
#define vector18 18
#define vector19 19
#define vector33 33
#define vector40 40
/* Macro for saving all registers */
#define SAVE_ALL    \
    cld;            \
    pushl %fs;      \
    pushl %es;      \
    pushl %ds;      \
    pushl %ebp;     \
    pushl %edi;     \
    pushl %esi;     \
    pushl %edx;     \
    pushl %ecx;     \
    pushl %ebx;     \
    pushl %eax;     \

.global pit_handler_linkage
.global page_fault_linkage
.global divide_error_linkage
.global debug_exception_linkage
.global nmi_interrupt_linkage
.global breakpoint_exception_linkage
.global overflow_exception_linkage
.global bound_re_linkage
.global invalid_opcode_linkage
.global device_na_linkage
.global double_fault_linkage
.global coprocessor_so_linkage
.global invalid_tss_linkage
.global segment_np_linkage
.global stack_fault_linkage
.global general_protection_linkage
.global reserved_exc_linkage
.global FPUfpe_linkage
.global alignment_check_linkage
.global machine_check_linkage
.global simd_fp_linkage
.global pit_linkage
.global keyboard_linkage
.global rtc_interrupt_linkage



#define PUSHALL \
pushl %gs       ;\
pushl %fs       ;\
pushl %es       ;\
pushl %ds       ;\
pushl %ebp      ;\
pushl %esi      ;\
pushl %edi      ;\
pushl %edx      ;\
pushl %ecx      ;\      
pushl %ebx      ;\

#define POPALL  \
popl %ebx       ;\
popl %ecx       ;\
popl %edx       ;\
popl %edi       ;\
popl %esi       ;\
popl %ebp       ;\
popl %ds        ;\
popl %es        ;\
popl %fs        ;\
popl %gs        ;\

.global halt
.global execute
.global read 
.global write
.global open
.global close
.global getargs
.global vidmap
.global set_handler
.global sigreturn
.global system_call_linkage

system_call_jmp:
    .long 0
    .long halt
    .long execute
    .long read 
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long set_handler
    .long sigreturn

system_call_linkage:
    addl $-4, %esp      # Push Error Code / Dummy
    PUSHALL
    pushl %edx
    pushl %ecx
    pushl %ebx
    cmpl $0, %eax       # Check bounds for jump table index
    jle invalid_call
    cmpl $10, %eax
    jg invalid_call
    call *system_call_jmp(, %eax, 4)
    addl $12, %esp      # Pop arguments
    POPALL
    addl $4, %esp       # Pop Error Code / Dummy
    iret

invalid_call:
    movl $-1, %eax
    addl $12, %esp
    POPALL
    addl $4, %esp
    iret

divide_error_linkage:
    .align
    #pushl $vector0-256          /* Push inverted vector value onto stack */
    SAVE_ALL                    /* Save all registers */       
    call divide_error           /* Call the function handler */
    jmp ret_from_intr           /* Jump to pop all registers and return */

debug_exception_linkage:
    .align
    #pushl $vector1-256
    SAVE_ALL        
    call debug_exception
    jmp ret_from_intr

nmi_interrupt_linkage:
    .align
    #pushl $vector2-256
    SAVE_ALL        
    call nmi_interrupt
    jmp ret_from_intr

breakpoint_exception_linkage:
    .align
    #pushl $vector3-256
    SAVE_ALL      
    call breakpoint_exception
    jmp ret_from_intr

overflow_exception_linkage:
    .align
    #pushl $vector4-256
    SAVE_ALL      
    call overflow_exception
    jmp ret_from_intr

bound_re_linkage:
    .align
    #pushl $vector5-256
    SAVE_ALL        
    call bound_re
    jmp ret_from_intr

invalid_opcode_linkage:
    .align
    #pushl $vector6-256
    SAVE_ALL       
    call invalid_opcode
    jmp ret_from_intr

device_na_linkage:
    .align
    #pushl $vector7-256
    SAVE_ALL        
    call device_na
    jmp ret_from_intr

double_fault_linkage:
    .align
    #pushl $vector8-256
    SAVE_ALL        
    call double_fault
    jmp ret_from_intr

coprocessor_so_linkage:
    .align
    #pushl $vector9-256
    SAVE_ALL        
    call coprocessor_so
    jmp ret_from_intr

invalid_tss_linkage:
    .align
    #pushl $vector10-256
    SAVE_ALL       
    call invalid_tss
    jmp ret_from_intr

segment_np_linkage:
    .align
    #pushl $vector11-256
    SAVE_ALL        
    call segment_np
    jmp ret_from_intr

stack_fault_linkage:
    .align
    #pushl $vector12-256
    SAVE_ALL        
    call stack_fault
    jmp ret_from_intr

general_protection_linkage:
    .align
    #pushl $vector13-256
    SAVE_ALL       
    call general_protection
    jmp ret_from_intr

page_fault_linkage:
    .align
    #pushl $vector14-256
    SAVE_ALL      
    call page_fault
    jmp ret_from_intr

reserved_exc_linkage:
    iret

FPUfpe_linkage:
    .align
    #pushl $vector16-256
    SAVE_ALL        
    call FPUfpe
    jmp ret_from_intr

alignment_check_linkage:
    .align
    #pushl $vector17-256
    SAVE_ALL        
    call alignment_check
    jmp ret_from_intr

machine_check_linkage:
    .align
    #pushl $vector18-256
    SAVE_ALL        
    call machine_check
    jmp ret_from_intr

simd_fp_linkage:
    .align
    # #pushl $vector19-256
    SAVE_ALL        
    call simd_fp
    jmp ret_from_intr

pit_linkage:
    .align
    addl $-4, %esp
    #pushl $vector33-256
    SAVE_ALL        
    call pit_interrupt_handler
    # addl $4, %esp
    jmp ret_from_intr




keyboard_linkage:
    .align
    addl $-4, %esp
    #pushl $vector33-256
    SAVE_ALL        
    call handle_keyboard_interrupt
    # addl $4, %esp
    jmp ret_from_intr

rtc_interrupt_linkage:
    .align
    addl $-4, %esp
    #pushl $vector40-256
    SAVE_ALL
    call rtc_interrupt_handler
    jmp ret_from_intr

ret_from_intr:
    addl $4, %esp
    popl %ebx           /* Pop all saved registers */
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %ds
    popl %es
    popl %fs
    addl $4, %esp
    iret                /* Return from interrupt */

