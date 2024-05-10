#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H

#include "types.h"
#include "keyboard.h"
#include "lib.h"
#include "system_calls.h"
#include "paging.h"
#include "scheduler.h"

#define MAX_TERMINAL 3
#define MAX_CHARS 128
#define NUM_IN_PAGE 1024
#define NUM_COLS    80
#define NUM_ROWS    25

typedef struct terminal_desc {
	int screen_x;
	int screen_y;
	int cursor_x;
	int cursor_y;
	int tid;
} terminal_desc_t;

terminal_desc_t terminals[MAX_TERMINAL];
uint8_t terminal_pages[MAX_TERMINAL][4096];

int terminal_init(); // checkpoint 5
int terminal_open();
int terminal_close();
int terminal_write(int fd, char* buf, int n);
int terminal_read(int fd, char* buf, int n);
int key_buffer_push(char c);
void key_buffer_clear();

// terminal bad func
int32_t terminal_bad_open(const uint8_t* filename);
int32_t terminal_bad_close(int32_t fd);
int32_t terminal_bad_read(int32_t fd, void* buf, int32_t nbyte);
int32_t terminal_bad_write(int32_t fd, const void* buf, int32_t nbyte);

// terminal switch
void terminal_switch(uint8_t tid);
extern int of_flag;
// extern int t1_flag;
// extern int t2_flag;

#endif
