#include "system_calls.h"
#include "i8259.h"
int32_t curr_pid;
uint8_t curr_tid = 0;
uint8_t vid_tid = 0;
/* pid_init
 * Inputs: status
 * Outputs: 0 - success
 * Functionality: initialize the pid
 */
void pid_init() {
    int i, j;
    for (j = 0; j < MAX_PROCESS; j++) {
        pids[j] = 0;
    }
    for (i = 0; i < MAX_SHELL; i++) {
        for (j = 0; j < MAX_PROCESS; j++) {
            tid_pids[i][j] = -1;
            // printf("tid_pids[%d][%d] == %d ",i,j,tid_pids[i][j]);
        }
        printf("\n");
    }
}

/* int32_t halt (uint8_t status)
 * Inputs: status
 * Outputs: 0 - success
 * Functionality: exit currnt process
 */
int32_t halt (uint8_t status){
    int i, j; // index to loop the files

    /* ==================CALCULATE PCB================== */
    // register int32_t esp_value asm("esp");
    // uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	// pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    uint32_t cur_pid;
	pcb_t* cur_pcb;
    for (j = 0; j < MAX_PROCESS; j++) {
        if (tid_pids[curr_tid][j] == -1)
            break;
    }
    if (j == 1) {
        printf("Do not exit the last shell!\n");
        cur_pid = tid_pids[curr_tid][0];
        cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
        tid_pids[curr_tid][0] = -1;
        pids[cur_pcb->curr_pid] = 0;
        execute((uint8_t*)"shell");
    }
    if (j == 0) {
        printf("No shell yet!\n");
        execute((uint8_t*)"shell");
    }
    j--;
    cur_pid = tid_pids[curr_tid][j];
    cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);

    pids[cur_pid] = 0;
    tid_pids[curr_tid][cur_pcb->pid_in_t] = -1;

    schedules_rr[curr_schedule] = cur_pcb->parent_pid;
    // printf("j = %d, cur_pcb->pid_in_t = %d\n",j, cur_pcb->pid_in_t);
    // pcb_t* par_pcb = (pcb_t*)(SIZE_8MB - (cur_pcb->parent_pid + 1) * SIZE_8KB);
    if (cur_pcb->parent_pid == -1){
        printf("Do not exit the last shell\n");
        pids[cur_pcb->curr_pid] = 0;
        execute((uint8_t*)"shell");
    }

    // for(i = 0; i < MAX_SCHEDULE; i++){
    //     if(schedules_rr[i] == cur_pid)
    //         schedules_rr[i] = cur_pcb->parent_pid;
    // }


    //-----Restore Parent Data-----//
        //(todo)

    //-----Restore Parent Paging-----//
    // printf("%d\n", cur_pcb->parent_pid);
    // (page_dir)[SIZE_128MB >> 22].present = 0;
    SET_PDE(page_dir, PROG_START/SIZE_4MB, 1, 1, 0, (SIZE_8MB + ((cur_pcb->parent_pid) * SIZE_4MB)) / SIZE_4KB);
    // printf("Set pde.\n");
    //-----Clear FD Array-----//
    for (i = 0; i < MAX_PCB_FILES; i++) {
        if (cur_pcb->file_descs[i].flags == 1) {
            cur_pcb->file_descs[i].file_ops_ptr.close_f(i);
            cur_pcb->file_descs[i].flags = 0;
        }
    }

    /* Flush TLB */
    flush_tlb(page_dir);
    //-----Write Parent Process Info Back into TSS-----//
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)(SIZE_8MB - (cur_pcb->parent_pid) * SIZE_TASK) - 4;
    //-----Jump to Execute Return-----//
        //(todo)
    asm volatile("                  \n\
                movl %0, %%eax      \n\
                movl %1, %%ebp      \n\
                leave               \n\
                ret"
            : /* no output */
            : "r" ((uint32_t)status),\
              "r" (cur_pcb->ebp)
            : "eax", "ebp");
    // printf("exit return 0\n");
    return 0;
}

/* int32_t execute (const uint8_t* command)
 * Inputs: command
 * Outputs: 0 - success
 * Functionality: create a process
 */
int32_t execute (const uint8_t* command){
   
    int32_t i, j;
    int32_t prog_buf, child_pid;
    dentry_t dentry_buf;
    pcb_t* child_pcb;
    uint8_t elf_buf[4]; /* Testing ELF needs 4 bytes */
    int data_len = 4;
    uint8_t program_name[FNAME_MAX] = {'\0'};
    int8_t args[NUM_CHARS + 1] = {'\0'};
    
    // uint32_t freq = 2;
    // disable_irq(RTC_IRQ_NUM);

    /* ==================SANITY CHECK================== */
    if (command == NULL) { 
        printf("Please type command!\n");
        return -1;
    }
        
    /*Store first word of command as the program name */
    for (i = 0; i < strlen((int8_t*)command) && command[i] != '\0' && command[i] != ' '; i++)
        program_name[i] = command[i];
    /* Find length of remaining command string increment in index i */
    while (i < strlen((int8_t*)command) && command[i] != '\0' && command[i] == ' ') // skip space between program and args
        i++;
    /* Given length store in args buffer with preset max size */
    for (j = 0; i < strlen((int8_t*)command) && command[i] != '\0';)
        args[j++] = command[i++];

    /* Check file validity */
    if (read_dentry_by_name(program_name, &dentry_buf) == -1)
        return -1;

    /*The first 4 bytes of the file represent a “magic number” that identifies the file as an executable. These bytes are, respectively, 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46. 
    If the magic number is not present, the execute system call should fail.*/
    if(4 != read_data((uint32_t)(dentry_buf.inode_num), (uint32_t)0, (uint8_t*)elf_buf, (uint32_t*)(&data_len)))
        return -1;
    if (elf_buf[0] != 0x7f || elf_buf[1] != 0x45 || elf_buf[2] != 0x4c || elf_buf[3] != 0x46)
        return -1;
    // printf("read data\n");  

    /* ==================LOAD DATA================== */
    /* Program start address which is in 24-27 bytes */
    read_data((uint32_t)(dentry_buf.inode_num), (uint32_t)24, (uint8_t*)(&prog_buf), (uint32_t*)(&data_len));
    // printf("read data\n");
    /* ==================CHECK EDGE CASES================== */
    for (j = 0; j < MAX_PROCESS; j++) {
        if (tid_pids[curr_tid][j] == -1)
            break;
    }
    for (i = 0; i < MAX_PROCESS; i++) {
        if (pids[i] == 0)
            break;
    }
    // printf("curr_tid = %d, pid = %d, pid in termianl = %d, program_name = %s\n", curr_tid, i, j, program_name);
    // if (i >= MAX_SHELL && strncmp((int8_t*)program_name,(int8_t*)"shell",6) == 0) {
    //     printf("Reach max shell!\n");
    //     return -1;
    // }
    if (i >= MAX_PROCESS) {
        printf("Reach max process!\n");
        return -1;
    }
    // if (strncmp((int8_t*)program_name,(int8_t*)"fish",5) == 0 || strncmp((int8_t*)program_name,(int8_t*)"pingpong",9) == 0) {
    //     rtc_init();
        	
    //     rtc_write(0, &freq, 0);
    // }
    /* ==================INIT PCB================== */
    /* Set child_pcb and pid */
    pids[i] = 1;
    tid_pids[curr_tid][j] = i;
    child_pid = i;
    child_pcb = (pcb_t*)(SIZE_8MB - (child_pid + 1) * SIZE_TASK);
    child_pcb->curr_pid = child_pid;
    child_pcb->pid_in_t = j;
    /* Child pid becomes current pid, save esp value*/
    register uint32_t esp_value asm("esp");
    register uint32_t ebp_value asm("ebp");
    child_pcb->esp = esp_value;
    child_pcb->ebp = ebp_value;
    // uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_TASK;
	// pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_TASK);

    /* Check if child is the first pid */
    // if(child_pid == 0 || child_pid == 1 || child_pid == 2){
    //     child_pcb->parent_pid = -1;
    // }else{
    //     child_pcb->parent_pid = cur_pcb->curr_pid;
    // }
    if (j == 0) {
        child_pcb->parent_pid = -1;
    } else {
        child_pcb->parent_pid = tid_pids[curr_tid][j - 1];
        // printf("cur_pcb->curr_pid = %d, tid_pids[curr_tid][j] = %d\n", cur_pcb->curr_pid, tid_pids[curr_tid][j - 1]);
    }

    // curr_tid = (uint8_t)child_pid;
    // printf("current tid = %d", curr_tid);

    /* Copy arguments into configured child pcb */
    memcpy(child_pcb->args, args, NUM_CHARS + 1);

    /* Configure file descriptors in the following for stdin and stout respectively */
       
    child_pcb->file_descs[0].file_ops_ptr.close_f = terminal_bad_close;
    child_pcb->file_descs[0].file_ops_ptr.open_f = terminal_bad_open;
    child_pcb->file_descs[0].file_ops_ptr.read_f = (read_ptr)terminal_read;
    child_pcb->file_descs[0].file_ops_ptr.write_f = terminal_bad_write;
    child_pcb->file_descs[0].inode = 0;
    child_pcb->file_descs[0].file_pos = 0;
    child_pcb->file_descs[0].flags = 1;

    child_pcb->file_descs[1].file_ops_ptr.close_f = terminal_bad_close;
    child_pcb->file_descs[1].file_ops_ptr.read_f = terminal_bad_read;
    child_pcb->file_descs[1].file_ops_ptr.open_f = terminal_bad_open;
    child_pcb->file_descs[1].file_ops_ptr.write_f = (write_ptr)terminal_write;
    child_pcb->file_descs[1].inode = 0; 
    child_pcb->file_descs[1].file_pos = 0;  
    child_pcb->file_descs[1].flags = 1;

    /* ==================SET SCHEDULE================== */

    schedules_rr[curr_schedule] = child_pid;
    // if(schedules_rr[curr_schedule] == -2 || schedules_rr[curr_schedule] == child_pcb->parent_pid)
    //     schedules_rr[curr_schedule] = child_pcb->curr_pid;
    // for(i = 0; i < MAX_SCHEDULE; i++){
    //     if(schedules_rr[i] == -2 || schedules_rr[i] == child_pcb->parent_pid){
    //         schedules_rr[i] = child_pid;
    //         break;
    //     }
    // }
    /* ==================SET PAGING================== */
    SET_PDE(page_dir, SIZE_128MB/SIZE_4MB, 1, 1, 0, (SIZE_8MB + child_pid * SIZE_4MB)/SIZE_4KB); // index 128/4 = 32, user program start at 8MB, 4MB each
    /* Flush TLB */
    flush_tlb(page_dir);
    // vid_remap(curr_tid);
    // SET_PTE(page_table, VIDEO_ADDR/SIZE_4KB, 0, VIDEO_ADDR/SIZE_4KB);
    // SET_PTE(page_table_video, 0, 1,(VIDEO_ADDR/SIZE_4KB));
    // flush_tlb(page_dir);
    
    data_len = PROG_END - IMAGE_ADDR;
    read_data((uint32_t)(dentry_buf.inode_num), (uint32_t)0, (uint8_t*)(IMAGE_ADDR), (uint32_t*)(&data_len));

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)(SIZE_8MB - (child_pid) * SIZE_TASK) - 4;

    /*Enabling the cursor also allows you to set the start and end scanlines, 
    the rows where the cursor starts and ends. 
    The highest scanline is 0 and the lowest scanline is the maximum scanline (usually 15).*/
    // enable_cursor(14, 15); 

    sti();

    asm volatile("                \n\
                pushl %0          \n\
                pushl %1          \n\
                pushfl            \n\
                pushl %2          \n\
                pushl %3          \n\
                iret              \n\
                "
                : /* no output */    \
                : "r" (USER_DS),     \
                  "r" (PROG_END - 4),\
                  "r" (USER_CS),     \
                  "r" (prog_buf)     \
                : "memory");
    // enable_irq(RTC_IRQ_NUM);
    return 0;
}
/* int32_t read (int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Outputs: 0 - success
 * Functionality: read a file
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    /* ==================CALCULATE PCB================== */
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
    pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    int32_t read_count = 0;
	if (fd < 0 || fd >= MAX_PCB_FILES || buf == NULL || nbytes <= 0 || !(cur_pcb->file_descs[fd].flags)) 
        return -1;
	read_count = cur_pcb->file_descs[fd].file_ops_ptr.read_f(fd, buf, nbytes);
    cur_pcb->file_descs[fd].file_pos += read_count;
    return read_count;
}

/* int32_t write (int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Outputs: 0 - success
 * Functionality: write a file
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    /* ==================CALCULATE PCB================== */
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
    pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);

    if (fd < 0 || fd >= MAX_PCB_FILES || buf == NULL || nbytes <= 0 ||!(cur_pcb->file_descs[fd].flags)) 
        return -1;
    return cur_pcb->file_descs[fd].file_ops_ptr.write_f(fd, buf, nbytes);
}

/* int32_t open (const uint8_t* filename)
 * Inputs: const uint8_t* filename
 * Outputs: 0 - success
 * Functionality: open a file
 */
int32_t open (const uint8_t* filename) {
    int i = 0;
    int fd;
    dentry_t dentry;
    /* if filaname invalid, -1*/
    if (fopen(filename) == -1) return -1; 

    /* ==================CALCULATE PCB================== */
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);

    while (i < MAX_PCB_FILES && cur_pcb->file_descs[i].flags)
        i++;
    if (i == MAX_PCB_FILES) return -1; // check valid file space

    fd = i;
    read_dentry_by_name(filename, &dentry);
    cur_pcb->file_descs[fd].flags = 1;
    cur_pcb->file_descs[fd].inode = dentry.inode_num;
    cur_pcb->file_descs[fd].file_pos = 0;
    switch (dentry.filetype)
    {
    case 0: // rtc
        cur_pcb->file_descs[fd].file_ops_ptr.open_f = rtc_open;
        cur_pcb->file_descs[fd].file_ops_ptr.close_f = rtc_close;
        cur_pcb->file_descs[fd].file_ops_ptr.read_f = rtc_read;
        cur_pcb->file_descs[fd].file_ops_ptr.write_f = rtc_write;
        break;
    case 1: // directory
        cur_pcb->file_descs[fd].file_ops_ptr.open_f = dopen;
        cur_pcb->file_descs[fd].file_ops_ptr.close_f = dclose;
        cur_pcb->file_descs[fd].file_ops_ptr.read_f = dread;
        cur_pcb->file_descs[fd].file_ops_ptr.write_f = dwrite;
        break;
    case 2: // file
        cur_pcb->file_descs[fd].file_ops_ptr.open_f = fopen;
        cur_pcb->file_descs[fd].file_ops_ptr.close_f = fclose;
        cur_pcb->file_descs[fd].file_ops_ptr.read_f = fread;
        cur_pcb->file_descs[fd].file_ops_ptr.write_f = fwrite;
        break;
    default:
        cur_pcb->file_descs[fd].flags = 0;
        return -1;
        break;
    }
    cur_pcb->file_descs[fd].file_ops_ptr.open_f(filename);
    return fd;
}
/* int32_t close (int32_t fd)
 * Inputs: int32_t fd
 * Outputs: 0 - success
 * Functionality: close a file
 */
int32_t close (int32_t fd){
    if (fd <= 1 || fd >= MAX_PCB_FILES) return -1;
    /* ==================CALCULATE PCB================== */
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);

    if (!cur_pcb->file_descs[fd].flags) return -1;

    cur_pcb->file_descs[fd].file_ops_ptr.close_f(fd);
    cur_pcb->file_descs[fd].flags = 0;

    return 0;
}
/* int32_t getargs (uint8_t* buf, int32_t nbytes)
 * Inputs: uint8_t* buf, int32_t nbytes
 * Outputs: 0 - success
 * Functionality: get arguments, from excute
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
    /* ==================CALCULATE PCB================== */
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);

    if(buf == NULL || nbytes <= 0 || (uint32_t)buf < PROG_START || cur_pcb == NULL || cur_pcb->args[0] == '\0')
        return -1;
    if (nbytes > NUM_CHARS) nbytes = NUM_CHARS;

    memcpy(buf, cur_pcb->args, nbytes);
    // puts((int8_t*)buf);
    // printf("\n");

    return 0;
}

/* int32_t vidmap (uint8_t** screen_start)
 * Inputs: uint8_t** screen_start
 * Outputs: 0 - success
 * Functionality: set video map
 */
int32_t vidmap (uint8_t** screen_start){
    
    if (screen_start == NULL || screen_start < (uint8_t**) PROG_START ||
        screen_start > (uint8_t**) (PROG_END - 1))
        return -1;
    /* Set new page directory entry from virtual video address to physical video memory */
    SET_PDE(page_dir, PROG_END/SIZE_4MB, 1, 0, 0, (uint32_t) (page_table_video) / SIZE_4KB); // index 132/4 = 33

    SET_PTE(page_table_video, 0, 1,(VIDEO_ADDR/SIZE_4KB));
    vid_tid = curr_tid;
    /* Flush TLB */
    flush_tlb(page_dir);
    
    *screen_start = (uint8_t*)(PROG_END);
    return 0;
}

/* int32_t set_handler (int32_t signum, void* handler_address)
 * Inputs: int32_t signum, void* handler_address
 * Outputs: 0 - success
 * Functionality: set handler
 */
int32_t set_handler (int32_t signum, void* handler_address){
    return -1;
}

/* int32_t sigreturn (void)
 * Inputs: none
 * Outputs: 0 - success
 * Functionality: return from sig
 */
int32_t sigreturn (void){
    return -1;
}
