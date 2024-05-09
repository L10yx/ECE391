#include "terminal_driver.h"

#define NUM_CHARS 128
int of_flag = 0;
int t1_flag = 0;
int t2_flag = 0;
extern uint8_t curr_tid;
typedef struct {
    char keys[NUM_CHARS];
    uint8_t first;
    uint8_t last;
}  key_buffer_t;

// typedef struct {
//     char keys[NUM_CHARS];
//     uint8_t first;
//     uint8_t last;
//     uint8_t count; // Track number of elements in the buffer
// } key_buffer_t;
key_buffer_t key_buffer;

// extern struct key_buffer_t* key_buffer;

/* 
 * terminal_open
 * Opens the terminal and clears the keyboard bugger
 * Inputs: None
 * Outputs: 0
 * Side Effects: Initializes terminal
 * Coverage:
 * Files: terminal_driver.c./h
 */
int terminal_open() {
    
    key_buffer_clear();
    return 0;
}

/* 
 * terminal_close
 * Closes the terminal
 * Inputs: None
 * Outputs: 0
 * Side Effects: Closes the terminal and returns 0
 * Coverage:
 * Files: terminal_driver.c./h
 */
int terminal_close() {
    return 0;
}

/* 
 * terminal_write
 * Write to the terminal by reading from the buffer
 * Inputs: None
 * Outputs: n - 
 * Side Effects: Closes the terminal and returns 0
 * Coverage:
 * Files: terminal_driver.c./h
 */
int terminal_write(int fd, char* buf, int n) {
    int i;
    cli();
    for (i = 0; i < n; i++) {
        if (buf[i] != (char) 0) putc(buf[i]);
        // else break;
    }
    sti();

    return i;
}

/* 
 * terminal_read
 * Reads from the keyboard buffer into the terminal buffer
 * Inputs: None
 * Outputs: c - bytes read
 * Side Effects: Writes into the terminal buffer
 * Coverage:
 * Files: terminal_driver.c./h
 */
int terminal_read(int fd, char* buf, int n) {
    int i;

    key_buffer_clear();
    // char *cmd = "pingpong\n";
    // for(i= 0; i < 9; i++) key_buffer.keys[i] = cmd[i];
    // key_buffer.last = 9;
    // char *cmd = "fish\n";
    // for(i= 0; i < 5; i++) key_buffer.keys[i] = cmd[i];
    // key_buffer.last = 5;
    //Wait until a command has been entered
    // char *cmd = "fish\n";
    // for(i= 0; i < 5; i++) key_buffer.keys[i] = cmd[i];
    // key_buffer.last = 5;

    while(1) if (key_buffer.keys[key_buffer.last - 1] == '\n') break;
    if (strncmp((int8_t*)key_buffer.keys,(int8_t*)"clear",5) == 0) {
        clear();
        key_buffer_clear();
    }
    //Copy KBD buffer into passed buffer
    for (i = 0; i < key_buffer.last; i++) {
        if (i == n - 1) break;  //Only read up to n characters
        buf[i] = key_buffer.keys[i];
        
    }
    key_buffer_clear();
    return i;
}

/* 
 * key_buffer_push
 * Push into the keyboard buffer
 * Inputs: None
 * Outputs: 0 - success
 *          -1 - fail
 * Side Effects: Writes into the keyboard buffer
 * Coverage:
 * Files: terminal_driver.c./h
 */
int key_buffer_push(char c) {
    //OVERFLOW CHECK
    if (of_flag == 0) {
        if (key_buffer.last == NUM_CHARS) {
            of_flag = 1;
            // key_buffer.last--;
            // printf("KEYBOARD BUFFER OVERFLOW! Clearing Buffer...\n");
            // key_buffer_clear();
            return -1;
        }

        //No Overflow, add character to KBD buffer
        else {
            putc(c);
            if(c == ASCII_BACKSPACE){
                if (key_buffer.last == 0) return 0;
                key_buffer.last--;
                key_buffer.keys[key_buffer.last] = '\0';
            }
            else {
                key_buffer.keys[key_buffer.last] = c;
                key_buffer.last++;
            }
        }
    } else {
        if(c == ASCII_BACKSPACE) {
            putc(c);
            if (key_buffer.last == 0) return 0;
            key_buffer.last--;
            key_buffer.keys[key_buffer.last] = '\0';
            of_flag = 0;
        }
        else if (c == ASCII_ENTER) {
            putc(c);
            key_buffer.last--;
            key_buffer.keys[key_buffer.last] = c;
            key_buffer.last++;
            of_flag = 0;
        }
    }
    return 0;
}


/* 
 * key_buffer_clear
 * Clears the keyboard buffer
 * Inputs: None
 * Outputs: None
 * Side Effects: Removes all values from the keyboard buffer
 * Coverage:
 * Files: terminal_driver.c./h
 */
void key_buffer_clear() {
    int i;
    for (i = 0; i < NUM_CHARS; i++) {key_buffer.keys[i] = '\0';}
    key_buffer.last = 0;
}

// terminal bad func open
int32_t terminal_bad_open(const uint8_t* filename) {
	return -1;
}
// terminal bad func close
int32_t terminal_bad_close(int32_t fd) {
	return -1;
}
// terminal bad func read
int32_t terminal_bad_read(int32_t fd, void* buf, int32_t nbyte){
    return -1;
}
// terminal bad func write
int32_t terminal_bad_write(int32_t fd, const void* buf, int32_t nbyte){
    return -1;
}

/* 
 * terminal_switch
 * switch terminal by terminal id (tid)
 * Inputs: tid
 * Outputs: None
 * Side Effects: switch to new terminal, reserve old
 */
void terminal_switch(uint8_t tid){

    if(tid >= MAX_TERMINAL || tid == curr_tid) return;


    register uint32_t ebp_value asm("ebp");
    register uint32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    pcb_t* next_pcb;
    cur_pcb->ebp_s = ebp_value;

    memcpy((void*)terminal_pages[curr_tid],(void*)VIDEO_ADDR, SIZE_4KB);
    memcpy((void*)VIDEO_ADDR, (void*)terminal_pages[tid], SIZE_4KB); 
    
    curr_tid = tid;

    // if (terminals[curr_tid].screen_x == 0 && terminals[curr_tid].screen_y == 0) printf("====== TERMINAL %d ======\n391OS> ", curr_tid);
    update_cursor(terminals[curr_tid].cursor_x, terminals[curr_tid].cursor_y);
    int i;

    if(tid == 1 && t1_flag == 0){
        t1_flag = 1;
        for (i = 0; i < NUM_ROWS; i++) printf("\n");
        terminals[tid].screen_x = 0;
        terminals[tid].screen_y = 0;
        terminals[tid].cursor_x = 0;
        terminals[tid].cursor_y = 0;
        execute((uint8_t*)"shell");

    }

     if(tid == 2 && t2_flag == 0){
        t2_flag = 1;
        for (i = 0; i < NUM_ROWS; i++) printf("\n");
        terminals[tid].screen_x = 0;
        terminals[tid].screen_y = 0;
        terminals[tid].cursor_x = 0;
        terminals[tid].cursor_y = 0;
        execute((uint8_t*)"shell");

    }

    int32_t next_pid;
    int j;
    for (j = 0; j < MAX_PROCESS; j++) {
        if (tid_pids[curr_tid][j] == -1)
            break;
    }
    next_pid = tid_pids[curr_tid][j-1];

    
    next_pcb = (pcb_t*)(SIZE_8MB - (next_pid+ 1) * SIZE_8KB);

    // map virtual program image (128MB) to physical user program (shell# 8MB-12MB-16MB)
    SET_PDE(page_dir, PROG_START/SIZE_4MB, 1, 1, 0, (SIZE_8MB + (next_pid * SIZE_4MB)) / SIZE_4KB);

    /* Flush TLB */
    flush_tlb(page_dir);
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
/* 
 * terminal_init
 * initialize the terminal struct
 * Inputs: None
 * Outputs: 0
 * Side Effects: initialize the terminal and returns 0
 */
int terminal_init() {
    int i;
    for(i = 0; i < MAX_TERMINAL; i++) {
        terminals[i].tid = i;
        terminals[i].screen_x = 0;
        terminals[i].screen_y = 0;
        terminals[i].cursor_x = 0;
        terminals[i].cursor_y = 1;
    }
    
    return 0;
}
