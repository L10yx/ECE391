#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"

#define BLOCK_SIZE          4096  // Each block is 4096 bytes
#define MAX_FILE_NUM 63 // Boot block can hold 64 entries including statistic part at most. So the biggest index is 63
#define FNAME_MAX           32 // Longest file name is 32 bytes
#define SIZE_4B             4
#define MAX_FILE_SIZE       4*36200
typedef struct dentry_t {
    uint8_t  filename[FNAME_MAX];
    uint32_t filetype;
    uint32_t inode_num;
    uint32_t reserved[6];  // 24bytes reserved, 24/4 = 6
} dentry_t;

typedef struct inode_t {
    uint32_t len; // 4 bytes
    uint32_t data_block[(BLOCK_SIZE - SIZE_4B) / SIZE_4B];
} inode_t;

typedef struct boot_block_t {
    uint32_t dentry_num;
    uint32_t inode_num;
    uint32_t data_blocks_num;
    uint32_t reserved[13]; // 52bytes reserved, 52/4 = 13
    dentry_t dentries[MAX_FILE_NUM];
} boot_block_t;

typedef struct data_block_t {
    uint8_t data[BLOCK_SIZE];
} data_block_t;


/*initilize filesystem*/
void filesys_init(uint32_t file_add);

/*The three routines provided by the file system module return -1 on failure, indicating a non-existent file or invalid
index in the case of the first two calls, or an invalid inode number in the case of the last routine. Note that the directory
entries are indexed starting with 0. Also note that the read data call can only check that the given inode is within the
valid range. It does not check that the inode actually corresponds to a file (not all inodes are used). However, if a bad
data block number is found within the file bounds of the given inode, the function should also return -1*/

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint8_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t* length);


int32_t read_file_by_name(const uint8_t* fname, uint8_t* buf, uint32_t* length);
int32_t read_directory(uint8_t* buf, int index, uint32_t* ftype, uint32_t* fsize);


/*for file*/
int32_t fopen(const uint8_t* fname);
int32_t fclose(int32_t fd);
int32_t fread(int32_t fd, void* buf, int32_t nbytes);
int32_t fwrite(int32_t fd, const void* buf, int32_t nbytes);
/*for dir*/
int32_t dopen(const uint8_t* fname);
int32_t dclose(int32_t fd);
int32_t dread(int32_t fd, void* buf, int32_t n_bytes);
int32_t dwrite(int32_t fd, const void* buf, int32_t nbytes);

#endif
