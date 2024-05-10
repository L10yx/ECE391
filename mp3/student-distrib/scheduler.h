#ifndef SCHEDULE_H
#define SCHEDULE_H
#include "types.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal_driver.h"
#include "filesys.h"
#include "system_calls.h"

#define MAX_SCHEDULE 3
extern void scheduler_init();
extern void scheduler_rr();


int schedules_rr[MAX_SCHEDULE];


#endif
