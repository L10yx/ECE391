#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include "lib.h"
#include "terminal_driver.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesys.h"
#include "paging.h"
#include "x86_desc.h"
#include "exceptions_linkage.h"

#define MAX_PCB_FILES 8
#define NUM_CHARS 128
#define SIZE_TASK (SIZE_8KB)

#define MAX_SHELL 3
#define MAX_PROCESS 6

extern inode_t* inodes;
/* file operations ptr to corresponding functions */
typedef int32_t (*read_ptr)(int32_t, void*, int32_t);
typedef int32_t (*write_ptr)(int32_t, const void*, int32_t);
typedef int32_t (*open_ptr)(const uint8_t*);
typedef int32_t (*close_ptr)(int32_t);

typedef struct file_ops_jumptable { // The file operations jump table
    open_ptr open_f;
    close_ptr close_f;
    read_ptr read_f;
    write_ptr write_f; 
} file_ops_t;

typedef struct file_desc {  // file descriptor
    file_ops_t file_ops_ptr;   // file operations table pointer
    uint32_t inode;         // index node: the inode number for this file. This is only valid for data files, and should be 0 for directories and the RTC device file
    int32_t file_pos;       // file position: A “file position” member that keeps track of where the user is currently reading from in the file. Every read system call should update this member.
    int32_t flags;          // A “flags” member for, among other things, marking this file descriptor as “in-use.” 0 - not, 1 - in use
} file_desc_t;


typedef struct pcb { // Process Control Block
    file_desc_t file_descs[MAX_PCB_FILES];
    int8_t args[NUM_CHARS + 1]; // Consider '\0', so +1
    uint32_t esp;
    uint32_t ebp;
    uint32_t esp_s; // for scheduler
    uint32_t ebp_s; // for scheduler
    int32_t parent_pid;
    int32_t curr_pid;
    int32_t pid_in_t;
    // pcb_t* parent;
} pcb_t;

extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t signum, void* handler_address);
extern int32_t sigreturn (void);
void pid_init();
extern uint8_t vid_tid;
extern uint8_t curr_schedule;
int32_t pids[MAX_PROCESS];
int32_t tid_pids[MAX_SHELL][MAX_PROCESS];

#endif
