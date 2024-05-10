#include "scheduler.h"
uint8_t curr_schedule = 0;
volatile int32_t count;
/* 
 * scheduler_init
 * initialize scheduler
 * Input: None
 * Output: None
 */
void scheduler_init() {
    int i;
    curr_schedule = 0;
    count = 2;
    for (i = 0; i < MAX_SCHEDULE; i++) // init all in schedules_rr to -2
        schedules_rr[i] = -1;
    return;
}
/* 
 * scheduler_rr
 * Round Robin scheduler
 * Input: None
 * Output: None
 */
void scheduler_rr() {
    // int next_schedule = 0;
    // int32_t next_pid;
    // /* ==================CALCULATE PCB================== */
    // register int32_t esp_value asm("esp");
    // register uint32_t ebp_value asm("ebp");
    // uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	// pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    // pcb_t* next_pcb;
    // cur_pcb->esp_s = esp_value;
    // cur_pcb->ebp_s = ebp_value;
    // printf("current ebp: %d, esp: %d",ebp_value, esp_value);
    // curr_schedule = (curr_schedule + 1) % MAX_SCHEDULE; // Round Robin
    // next_pid = schedules_rr[(int32_t)curr_schedule];
    // next_pcb = (pcb_t*)(SIZE_8MB - (next_pid + 1) * SIZE_8KB);
    // printf(" ---- ter: %d, pid: %d\n", curr_schedule, next_pid);

    // if (next_pid == -2) {
    //     // return;
    //     // vid_remap(curr_schedule);
    //     execute((uint8_t*)"shell");
    // }
    // vid_remap(curr_schedule);
    // else if (next_pid == 0) {
    //     printf("schedule terminal0!\n");
    // }
    // else if (next_pid == 1) {
    //     printf("schedule terminal1!\n");
    // }
    // curr_schedule = cur_pcb->curr_pid;
    
    // if (schedules_rr[curr_schedule] == -2) { // first switch
            // map virtual video memory(4KB) to reserve physical video page for terminal# (next_schedule) 
            // SET_PTE(page_table, VIDEO_ADDR/SIZE_4KB, 0, VIDEO_ADDR/SIZE_4KB + VIDEO_GAP + curr_schedule); 
            // SET_PTE(page_table_video, 0, 1, VIDEO_ADDR/SIZE_4KB + VIDEO_GAP + curr_schedule);
            // SET_PDE(page_dir, PROG_START/SIZE_4MB, 1, 1, 0, (SIZE_8MB + curr_schedule * SIZE_4MB)/SIZE_4KB); 
    //     vid_remap(curr_schedule);
    //     printf("current schedule = %d", curr_schedule);
    //     execute((uint8_t*)"shell");
    // }
    // if (count > 0) {
    //     vid_remap(curr_schedule);
    //     execute((uint8_t*)"shell");
    //     printf("count = %d", count);
    //     count--;
    // }
    register uint32_t ebp_value asm("ebp");
    register uint32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    pcb_t* next_pcb;
    cur_pcb->ebp_s = ebp_value;

    int32_t next_pid;
    curr_schedule = (curr_schedule + 1) % MAX_SCHEDULE; // Round Robin

    int j;
    for (j = 0; j < MAX_PROCESS; j++) {
        if (tid_pids[curr_schedule][j] == -1)
            break;
    }
    if (j == 0) return;
    next_pid = tid_pids[curr_schedule][j-1];

    
    next_pcb = (pcb_t*)(SIZE_8MB - (next_pid+ 1) * SIZE_8KB);

    // map virtual program image (128MB) to physical user program (shell# 8MB-12MB-16MB)
    SET_PDE(page_dir, PROG_START/SIZE_4MB, 1, 1, 0, (SIZE_8MB + (next_pid * SIZE_4MB)) / SIZE_4KB);

    /* Flush TLB */
    flush_tlb(page_dir);

    // vid_remap(curr_schedule);
    // Write Parent Process Info Back into TSS
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)(SIZE_8MB - (next_pid) * SIZE_TASK) - 4;
    asm volatile("                  \n\
                movl %0, %%ebp      \n\
                leave               \n\
                ret"
            : /* no output */
            : "r" (next_pcb->ebp_s)
            :"ebp");
}
