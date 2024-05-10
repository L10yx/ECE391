#ifndef PAGING_H
#define PAGING_H
#include "types.h"
#include "system_calls.h"

#define TABLE_NUM   1024        // 2 ^ 10 = 1024
#define PAGE_NUM    1024        // 2 ^ 10 = 1024
// #define OFFSET_MAX  4096        // 2 ^ 12 = 4096

#define SIZE_4KB        4096        // 4 * 1024 = 4096
#define SIZE_8KB        8192
#define SIZE_4MB        0x400000    // 4 * 1024 * 1024 = 4194304 = 2 ^ 22
#define SIZE_8MB        0x800000
#define SIZE_12MB       (SIZE_4MB + SIZE_8MB)
#define SIZE_128MB      0x08000000
#define SIZE_32MB       (8 * SIZE_4MB)


#define VIDEO_ADDR      0xB8000

#define KERNEL_ADDR     SIZE_4MB
#define USER_ADDR       SIZE_8MB
#define USER_IMAGE      0x08048000  // The program image itself is linked to execute at virtual address 0x08048000

/*The way to get this working is to set up a single 4 MB page directory entry that maps virtual address 0x08000000 (128 MB) to the right
physical memory address (either 8 MB or 12 MB)*/
#define PROG_START      0x8000000
#define PROG_END        (PROG_START + SIZE_4MB) // 132MB
#define VIDEO_START     PROG_END // 132MB
#define IMAGE_ADDR      0x8048000
#define VIDEO_GAP       2

#define SET_PDE(_page_dir, _i, _privilege, _page_size, _global, _addr) do { \
    (_page_dir)[(_i)].present = 1;                                          \
    (_page_dir)[(_i)].read_write = 1;                                       \
    (_page_dir)[(_i)].user_supervisor = (_privilege);                       \
    (_page_dir)[(_i)].write_through = 0;                                    \
    (_page_dir)[(_i)].cache_disable = 0;                                    \
    (_page_dir)[(_i)].accessed = 0;                                         \
    (_page_dir)[(_i)].reserved = 0;                                         \
    (_page_dir)[(_i)].page_size = (_page_size);                             \
    (_page_dir)[(_i)].global_page = (_global);                              \
    (_page_dir)[(_i)].availiable = 0;                                       \
    (_page_dir)[(_i)].page_table_addr = (_addr);                            \
} while (0)

#define SET_PTE(_page_table, _i, _privilege, _addr) do {                    \
    (_page_table)[(_i)].present = 1;                                        \
    (_page_table)[(_i)].read_write = 1;                                     \
    (_page_table)[(_i)].user_supervisor = (_privilege);                     \
    (_page_table)[(_i)].write_through = 0;                                  \
    (_page_table)[(_i)].cache_disable = 0;                                  \
    (_page_table)[(_i)].accessed = 0;                                       \
    (_page_table)[(_i)].dirty = 0;                                          \
    (_page_table)[(_i)].pat_index = 0;                                      \
    (_page_table)[(_i)].global_page = 0;                                    \
    (_page_table)[(_i)].availiable = 0;                                     \
    (_page_table)[(_i)].page_addr = (_addr);                                \
} while (0)

typedef struct __attribute__((packed)) pde_desc { // Page-Directory Entry (Vol.3 3-24 p90)
    uint32_t present            : 1;    // Indicates whether the page or page table being pointed to by the entry is currently loaded in physical memory.
    uint32_t read_write         : 1;    // Specifies the read-write _privileges for a page or group of pages: 0 - read only; 1 - read and write
    uint32_t user_supervisor    : 1;    // 0 - supervisor, 1 - user
    uint32_t write_through      : 1;    // 0 - write back, 1 - write through
    uint32_t cache_disable      : 1;    // 0 - can be cached, 1 - cache is prevented
    uint32_t accessed           : 1;
    uint32_t reserved           : 1;    // (set to 0)
    uint32_t page_size          : 1;    // (0 indicates 4 KBytes)
    uint32_t global_page        : 1;    // (Ignored)
    uint8_t  availiable         : 3;    // bit 11 - 9
    uint32_t page_table_addr    : 20;   // bit 31 - 12
}pde_desc_t;

typedef struct __attribute__((packed)) pte_desc { // Page-Table Entry (Vol.3 3-24 p90)
    uint32_t present            : 1;
    uint32_t read_write         : 1;
    uint32_t user_supervisor    : 1;
    uint32_t write_through      : 1;
    uint32_t cache_disable      : 1;
    uint32_t accessed           : 1;
    uint32_t dirty              : 1;
    uint32_t pat_index          : 1;    // page attribute table index, bit 7 in ptes for 4KB pages and bit 12 in pdes for 4MB pages
    uint32_t global_page        : 1;
    uint8_t  availiable         : 3;    // bit 11 - 9
    uint32_t page_addr          : 20;   // bit 31 - 12
}pte_desc_t;


// to innitialize paging (used in kernel.c)
void paging_init(void);

void vid_remap(uint8_t tid);

/*To align things in C: int some_variable __attribute__((aligned (BYTES_TO_ALIGN_TO)));*/
pde_desc_t page_dir[TABLE_NUM] __attribute__((aligned (SIZE_4KB)));
pte_desc_t page_table[PAGE_NUM] __attribute__((aligned (SIZE_4KB)));
pte_desc_t page_table_kernel[PAGE_NUM] __attribute__((aligned (SIZE_4KB)));
pte_desc_t page_table_video[PAGE_NUM] __attribute__((aligned (SIZE_4KB)));
#endif
