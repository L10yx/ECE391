#include "filesys.h"
#include "lib.h"
#include "x86_desc.h"
#include "paging.h"
#include "system_calls.h"

// global ptrs for initializaition
boot_block_t* boot_block;
dentry_t* dentries;
inode_t* inodes;
data_block_t* data_blocks;

int dread_index = 0;


/*
 * filesys_init
 * Initialize the file system
 * input: uint32_t file_add -- the address of file img
 * output: None
 * side effect: boot_block, inodes, data_blocks, dentries will be changed
 */
void filesys_init(uint32_t file_add){
    boot_block = (boot_block_t*)file_add;   // store boot block
    dentries = boot_block->dentries;        // store dentries
    inodes = (inode_t*)(boot_block + 1);    // store inodes
    data_blocks = (data_block_t*)(inodes + boot_block->inode_num);  // store data blocks
}



/*
 * read_dentry_by_index
 * Read the content of a dentry with given index
 * input: uint8_t index, dentry_t* dentry
 * output: -1 for failure and 0 for success
 * side effect: dentry can be changed
 */
int32_t read_dentry_by_index (uint8_t index, dentry_t* dentry){
    if(index >= boot_block->dentry_num || index > MAX_FILE_NUM ){
        return -1;
    }
    dentry_t* dentry_ptr = &(dentries[index]);
    dentry->filetype = dentry_ptr->filetype;
    dentry->inode_num = dentry_ptr->inode_num;
    strncpy((int8_t*)dentry->filename, (int8_t*)dentry_ptr->filename, FNAME_MAX);
    return 0;
}

/*
 * read_dentry_by_name
 * Read the content of a dentry with given name
 * input: const uint8_t* fname, dentry_t* dentry
 * output: -1 for failure and 0 for success
 * side effect: dentry can be changed
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint8_t i;
    dentry_t* current_dentry;
    if (fname == NULL)  
        return -1;
    if (strlen((int8_t*)fname) > 32)
        return -1;
    for(i=0; i<boot_block->dentry_num; i++){
        current_dentry = &(dentries[i]);
        // puts((int8_t*)current_dentry->filename);
        // printf(" i = %d, fname: %s, dentry_filename: %s\n", i, fname, current_dentry->filename);
        if(strncmp((int8_t*)fname, (int8_t*)current_dentry->filename, FNAME_MAX)==0){
            /* The file name is the same, use read_index to assign value */
            // printf("\n index in dentries: %d\n", i);
            read_dentry_by_index(i, dentry);
            
            return 0;
        }
    }
    return -1;

}
/*
 * read_file_by_name
 * Read the content of a file with given name
 * Input: const uint8_t* fname, uint8_t* buf, uint32_t* length
 * Output: -1 for failure and 0 for success
 * Side effect: buf can be changed
 */
int32_t read_file_by_name(const uint8_t* fname, uint8_t* buf, uint32_t* length){
    dentry_t dentry;
    uint8_t index = 0;
    printf("file_name: ");
    printf((int8_t*) fname);
    read_dentry_by_name(fname, &dentry);
    index = dentry.inode_num;
    printf(" index:%d\n",index);
    return read_data(index, 0, buf, length);
}

/*
 * read_data
 * Read the content of a dentry with given index
 * Input:uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t* length
 * Output: bytes of data we have successfully read
 * Side effect: buf can be changed
 */
int32_t read_data(uint32_t inode_index, uint32_t offset, uint8_t* buf, uint32_t* length){
    //int k;
    uint32_t i,j;
    inode_t* inode;
    // data_block_t* cur_datablock;   // Pointer to current data block
    uint32_t cur_db_index;          // index of current data block
    uint32_t buf_count;


    // uint32_t first_block;  // Which data block we should start to read
    // uint32_t first_offset;  // Which position in the start block we should start to read
    // uint32_t last_block;  // Which data block we should end reading
    // uint32_t last_offset;  // Which position in the last block we should end reading

    if(offset >= inodes[inode_index].len) return 0; 
    if(inode_index > (boot_block->inode_num-1)) return 0;

    //printf("length: %d, inode_len: %d\n", *length, inodes[inode_index].len);
    if (*length + offset > inodes[inode_index].len) *length = inodes[inode_index].len - offset;
    // for (k = 0; k < 64; k++) {
    //     printf("i: %d, len: %d; ",k,inodes[k].len);
    //     if (k %4 == 0) printf("\n");
    // }
    if (*length <= 0) return 0;
    //printf("length: %d\n", *length);
    inode = &(inodes[inode_index]);
    // buf_count = 0;

    /* Calculate how many data blocks are used and start/end positions */
    // first_block = offset / BLOCK_SIZE;
    // first_offset = offset % BLOCK_SIZE;
    // last_block = (*length - 1 + offset) / BLOCK_SIZE;
    // last_offset = (*length - 1 + offset) % BLOCK_SIZE;


    i = offset / BLOCK_SIZE;
    j = offset % BLOCK_SIZE;
    for (buf_count = 0; buf_count < *length; buf_count++, j++, buf++) {
        if(j >= BLOCK_SIZE){
            j = 0;
            i++;
        }
        cur_db_index = (inodes[inode_index].data_block)[i];
        if(cur_db_index >= boot_block->data_blocks_num){
            return buf_count;
        }
        // cur_datablock = &(data_blocks[cur_db_index]);
        // memcpy(buf, &(cur_datablock->data[j]), 1);
        *buf = data_blocks[cur_db_index].data[j];
    }




    // printf("first_block:%d, first_offset:%d\n",first_block,first_offset);
    // printf("last_block: %d, last_offset:%d\n",last_block,last_offset);
    
    // for(i=first_block; i<=last_block; i++){
    //     cur_db_index = (inodes[inode_index].data_block)[i];
    //     // printf("cur_db_index: %d\n",cur_db_index);
    //     cur_datablock = &(data_blocks[cur_db_index]);

    //     if(i == first_block){
    //         //printf("copy to buf...\n");
    //         if(first_block==last_block){
    //             memcpy(buf, &(cur_datablock->data[first_offset]), *length);
    //             buf += *length;
    //             buf_count += *length;
    //             // for (j = first_offset; j < (*length - 1 + offset); j++) {
    //             //     memcpy(buf, &(cur_datablock->data[j]), 1);
    //             //     buf++;
    //             //     buf_count++;
    //             // }
    //             // printf("last data block reached\n");
    //         }
    //         else{
    //             memcpy(buf, &(cur_datablock->data[first_offset]), BLOCK_SIZE-first_offset); 
    //             buf += BLOCK_SIZE - first_offset;
    //             buf_count += BLOCK_SIZE - first_offset;
    //         }
    //         continue;
    //     }
    //     if(i == last_block){
    //         memcpy(buf, cur_datablock->data, last_offset + 1);
    //         buf += last_offset + 1;
    //         buf_count += last_offset + 1;
    //         // printf("last data block reached\n");
    //         break;
    //     }
    //     memcpy(buf, cur_datablock->data, BLOCK_SIZE); // buf all block
    //     buf += BLOCK_SIZE;
    //     buf_count += BLOCK_SIZE;
    // }


    //     if(i == first_block){
    //         //printf("copy to buf...\n");
    //         for (j = first_offset; j < 4095; j++) {
    //             *buf = (cur_datablock->data[j]);
    //             buf++;
    //             buf_count++;
    //         }
    //         continue;
    //     }
    //     if(i == last_block){
    //         for (j = 0; j < last_offset; j++) {
    //             *buf = (cur_datablock->data[j]);
    //             buf++;
    //             buf_count++;
    //         }
    //         // printf("last data block reached\n");
    //         break;
    //     }
    //     for (j = 0; j < 4095; j++) {
    //         *buf = (cur_datablock->data[j]);
    //         buf++;
    //         buf_count++;
    //     }
    // }

    return buf_count;
}


/*
 * read_directory
 * read a directory
 * Inputs: uint8_t* buf, int index, uint32_t *ftype, uint32_t *fsize
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can read a directory by index
 */
int32_t read_directory(uint8_t* buf, int index, uint32_t *ftype, uint32_t *fsize){

    int i = 0;              // for index to store at most 32 length file name
    uint32_t fname_len;     // length for filename to buf
    dentry_t dentry;        // temp dir entry
    dentry = dentries[index];
    if (index >= boot_block->dentry_num) // if index out of range
        return -1;
    memset(buf, 32, FNAME_MAX); // initialize to spaces

   
    fname_len = strlen((int8_t*)dentry.filename);
    if (fname_len > FNAME_MAX) fname_len = FNAME_MAX;           // control length less than or equal 32
    read_dentry_by_name((uint8_t*)&(dentry.filename), &dentry);
    for (i = 0; i <= fname_len; i++) {
        memset(buf + i, dentry.filename[i], 1); // copy first 32 bytes to buf
    }
    *ftype = dentry.filetype;
    *fsize = inodes[dentry.inode_num].len;

    return fname_len;
}


/*
 * fopen
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t fopen(const uint8_t* fname){
    dentry_t dentry;
    if (read_dentry_by_name(fname, &dentry))
        return -1;
    else
        return 0;
}

/*
 * fread
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t fread(int32_t fd, void* buf, int32_t n_bytes) {
    if (n_bytes < 0 || buf == NULL || fd < 0 || fd > MAX_FILE_NUM) {
        printf("fread fail...\n");
        return -1;
    }

    uint32_t length = n_bytes;
    register int32_t esp_value asm("esp");
    uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    uint32_t inode_num = cur_pcb->file_descs[fd].inode;
    uint32_t offset =  cur_pcb->file_descs[fd].file_pos;
    //printf("inode_num = %d, offset = %d, buf = %s, length = %d\n",inode_num, offset, buf, length);
    return read_data(inode_num, offset, buf, &length);
}

/*
 * fclose
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t fclose(int32_t fd){
    return 0;
}

/*
 * fwrite
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t fwrite(int32_t fd, const void* buf, int32_t n_bytes){
    return 0;
}

/*
 * dopen
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t dopen(const uint8_t* filename){
    return 0;
}

/*
 * dread
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t dread(int32_t fd, void* buf, int32_t n_bytes){
    if (n_bytes < 0 || buf == NULL || fd < 0 || fd > MAX_FILE_NUM) {
        printf("dread fail...\n");
        return -1;
    }
    int32_t length = 0;
    // register int32_t esp_value asm("esp");
    // uint32_t cur_pid = (SIZE_8MB - esp_value) / SIZE_8KB;
	// pcb_t* cur_pcb = (pcb_t*)(SIZE_8MB - (cur_pid + 1) * SIZE_8KB);
    // uint32_t inode_num = cur_pcb->file_descs[fd].inode;
    // uint32_t offset =  cur_pcb->file_descs[fd].file_pos;
    uint32_t ftype;
    uint32_t fsize;
    length = read_directory(buf, dread_index, &ftype, &fsize);
    // printf("fd = %d, buf = %s, n_bytes = %d\n",fd, buf, n_bytes);
    // printf("%s\n",buf);
    if (length == -1){
        dread_index = 0;
        return 0;
    } else {
        dread_index++;
    }
    return length;
}

/*
 * dwrite
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t dwrite(int32_t fd, const void* buf, int32_t n_bytes){
    return 0;
}

/*
 * dclose
 * Open a file with its filename
 * Inputs: fname
 * Output: 0 if success -1 if fail
 * Side Effect: None
 * Coverage: Check whether we can open the file
 */
int32_t dclose(int32_t fd){
    return 0;
}

