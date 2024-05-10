#include "paging.h"
#include "lib.h"

/* 
 * paging_init(void)
 * Initialize paging for kernel.c use
 * Inputs: void
 * Outputs: none
 */
void paging_init() {
    int i; // index for loop
    //In this layout everything in the first 4MB, that isn’t the page for video memory, should be marked not present.
    SET_PTE(page_table, VIDEO_ADDR / SIZE_4KB, 0, VIDEO_ADDR / SIZE_4KB);
    // Checkpoint 5
    for (i = 0; i < MAX_TERMINAL + VIDEO_GAP; i++) {
        SET_PTE(page_table, (VIDEO_ADDR / SIZE_4KB + i), 0, VIDEO_ADDR / SIZE_4KB + i); // video pages for terminals
    }
    
    //the first 4 MB of memory should broken down into 4kB pages. (MP3 6.1.5)    
    SET_PDE(page_dir, 0, 0, 0, 0, ((uint32_t) page_table) / SIZE_4KB);  // 4KBs

    /* need only map virtual memory 4-8 MB to physical memory at 4-8 MB,  
    In addition to 8MB to 4GB being marked not present, you should also set any
    unused pages to not present as well. In this layout everything in the first 4MB, 
    that isn’t the page for video memory, should be marked not present. */
    SET_PDE(page_dir, 1, 0, 1, 1, KERNEL_ADDR / SIZE_4KB);// kernel

    // https://wiki.osdev.org/Paging#32-bit_Paging_.28Protected_Mode.29

    /*Paging is controlled by three flags in the processor’s control registers:
    • PG (paging) flag. Bit 31 of CR0 (available in all IA-32 processors beginning with the Intel386 processor). 
    • PSE (page size extensions) flag. Bit 4 of CR4 (introduced in the Pentium processor).
    • PAE (physical address extension) flag. Bit 5 of CR4 (introduced in the Pentium Proprocessors).*/
    asm volatile(
        "movl  %0, %%eax;           \
         movl  %%eax, %%cr3;        \
         movl  %%cr4, %%eax;        \
         orl   $0x00000010, %%eax;  \
         movl  %%eax, %%cr4;        \
         movl  %%cr0, %%eax;        \
         orl   $0x80000000, %%eax;  \
         movl  %%eax, %%cr0;"
        : /*no output*/
        : "r" (page_dir)
        : "%eax"
    );


    return;
}

/* 
 * vid_remap
 * remap the video pages
 * Inputs: tid
 * Outputs: none
 */
void vid_remap(uint8_t tid) {

    SET_PTE(page_table, VIDEO_ADDR/SIZE_4KB, 0, VIDEO_ADDR/SIZE_4KB);
        
    // if (tid == curr_tid) {
    //     SET_PTE(page_table_video, 0, 1,(VIDEO_ADDR/SIZE_4KB));
    // } else {
    //     SET_PTE(page_table_video, 0, 1,(terminal_pages/SIZE_4KB) + tid);
    // }
    SET_PTE(page_table_video, 0, 1,(VIDEO_ADDR/SIZE_4KB));
    flush_tlb(page_dir);
    return;
}


