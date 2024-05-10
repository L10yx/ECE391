#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "filesys.h"
#include "terminal_driver.h"
#include "keyboard.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here


/* 
 * divide_error_test
 * Assert that div 0 exception can be detected
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: divide_error exception, IDT
 * Files: idt.c/h
 */
int divide_error_test(){
	TEST_HEADER;

	int a = 0;
	int b;
	b = 1 / a;

	return FAIL;
}

/* 
 * paging_test
 * return PASS if successfully dereference all
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: paging
 * Files: idt.c/h
 */
int paging_test() {
	TEST_HEADER;
	unsigned char* video_addr = (unsigned char*) 0xB8000;
	unsigned char* video_addr_end  = video_addr + 0x1000;
	unsigned char* k_addr = (unsigned char*) 0x400000;
	unsigned char* k_addr_end = (unsigned char*) 0x800000;
	unsigned char* addr;
	unsigned char  test;
	

	for (addr = video_addr; addr < video_addr_end; addr++)
		test = (*addr);

	for (addr = k_addr; addr < k_addr_end; addr++)
		test = (*addr);

	return PASS;
}

/* 
 * breakpoint_test
 * Check if breakpoint exception can successfully occur
 * Inputs: None
 * Outputs: FAIL
 * Side Effects: None
 * Coverage: breakpoint exception, IDT
 * Files: idt.c/h
 */
int breakpoint_test(){
	TEST_HEADER;
	asm("int $3");
	return FAIL;
}

// /* 
//  * overflow_test
//  * Check if overflow exception can successfully occur
//  * Inputs: None
//  * Outputs: FAIL
//  * Side Effects: None
//  * Coverage: overflow exception, IDT
//  * Files: idt.c/h
//  */
// int overflow_test(){
// 
// 	return FAIL;
// }

// /* 
//  * bound_test
//  * Check if bound range exceeded exception can successfully occur
//  * Inputs: None
//  * Outputs: FAIL
//  * Side Effects: None
//  * Coverage: bound range exceeded exception, IDT
//  * Files: idt.c/h
//  */
// int bound_test(){
// 	asm("bound");
// 	return FAIL;
// }

int sim_test(){
    TEST_HEADER;
    float a, b;
    a = 0.0;
    b= 1.23;

    b = b / a;

    return FAIL;
}

int machine_test() {
    TEST_HEADER;

    asm("int $18");

    return FAIL;
}

int missalignment_test() {

    TEST_HEADER;

    asm("int $17");

    return FAIL;

}

int FPU_test() {
    TEST_HEADER;

    asm("int $16");

    return FAIL;
}

// int Gen_Prot_test() {
//     TEST_HEADER;

//     int("into");

//     return FAIL;
// }

int Stack_Seg_test() {
    TEST_HEADER;

    asm("int $12");

    return FAIL;
}

int Seg_NP() {
    TEST_HEADER;

    asm("int $11");

    return FAIL;
}

/* Checkpoint 2 tests */
int Terminal_Write_Test() {
	TEST_HEADER;
	char* buffer;

	buffer = "Hello World\n";
	printf("Printing Hello World with terminal write...\n");
	// terminal_write(0, buffer, 11);

	if (terminal_write(0, buffer, 11) != 11) return FAIL;
	if (terminal_write(0, buffer, 12) != 11) return FAIL;
	if (terminal_write(0, buffer, 5) != 5) return FAIL;

	return PASS;

}


int RTC_TEST(){
	printf("start write test");
	TEST_HEADER;
	// uint8_t buf[4];
	int i;
	// int x;
	
	// testing rtc_open
	printf("\nrtc_open: should print at 2 Hz\n");
	uint8_t* file = NULL;	// argument isnt used so initializing to NULL
	rtc_open(file);
	for(i = 0; i < 15; i++){
		while(rtc_read(0,NULL,0) != 1);
			putc('1');
	}
	printf("\n\n");


	// for(i = 0; i < 20; i++){
	// 	// printf("for %d", i);
	// 	while(rtc_read(0,NULL,0) != 1);
	// 	// while(!(num_interrupts_generated >= num_interrupts_needed));
	// 		putc('1');
	// }
	// printf("\n");

	printf("rtc_write: should print lines at each possible interrupt rate\n\n");
	uint32_t freq = 4;
	printf("4 Hz: ");
	rtc_write(0, &freq, 0);
	// printf("end write test");

	for(i = 0; i < 10; i++){
		while(rtc_read(0,NULL,0) != 1);
			putc('2');
	}
	printf("\n");

	
	freq = 8;
	printf("8 Hz: ");
	rtc_write(0, &freq, 0);
	for( i = 0; i < 20; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('3');
	}
	printf("\n");


	freq = 16;
	printf("16 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 30; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('4');
	}
	printf("\n");


	freq = 32;
	printf("32 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 40; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('5');
	}
	printf("\n");


	freq = 64;
	printf("64 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 50; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('6');
	}
	printf("\n");


	freq = 128;
	printf("128 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 60; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('7');
	}
	printf("\n");


	freq = 256;
	printf("256 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 70; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('8');
	}
	printf("\n");

	freq = 512;
	printf("512 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 72; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('9');
	}
	printf("\n");

	freq = 1024;
	printf("1024 Hz: ");
	rtc_write(0, &freq, 0)	;
	for( i = 0; i < 71; i++){
		if(rtc_read(0, NULL, 0) == 1);
		putc('0');
	}
	printf("\n");
	
	return PASS;

}



/* 
 * open_file_test
 * return PASS if buffer file
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: file system
 * Files: filesys.c/h
 */
int open_file_test() {
	if(fopen((uint8_t*)"ls") == 0) return PASS;
	return FAIL;
}
/* 
 * close_file_test
 * return PASS if buffer file
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: file system
 * Files: filesys.c/h
 */
int close_file_test() {
	int32_t fd;
	if(fclose(fd) == 0) return PASS;
	return FAIL;
}


/* 
 * read_file_test
 * return PASS if buffer file
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: file system
 * Files: filesys.c/h
 */
int read_file_test(){
	
	TEST_HEADER;
	int i = 0;
	uint32_t len = 40960;
	uint8_t buf[40960];
	// uint8_t* fname = (uint8_t*)"frame0.txt";
	// uint8_t* fname = (uint8_t*)"frame1.txt";
	//uint8_t* fname = (uint8_t*)"grep";
	// uint8_t* fname = (uint8_t*)"ls";
	uint8_t* fname = (uint8_t*)"verylargetextwithverylongname.tx";
    if (read_file_by_name(fname, buf, &len))

		clear();
		for (i = 0; i < len; i++) {
			//printf("%c",buf[i]); // works for \0
			putc(buf[i]);
		}
		// puts((int8_t*)buf); 
		printf("\n");
		printf("file_name: ");
		puts((int8_t*)fname);
		printf("\n");
		return PASS;
	return FAIL;

}


/* 
 * read_directory_test
 * return PASS if read dir
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: file system
 * Files: filesys.c/h
 */
int read_directory_test(){
	TEST_HEADER;
	int i,j;
	
	uint8_t buf[33];
	
	// 63 is the max directory number in filesystem
	printf("\n");
	for (i = 0; i < 63; i++){
		int count = 0;
		int temp = 0;
		uint32_t ftype;
		uint32_t fsize;
		if (read_directory(buf, i, &ftype, &fsize) == -1)
			break;
		printf("file_name:");
		buf[32] = '\0'; // indicate an end
		printf((int8_t*)buf);
		temp = fsize;
		while (temp / 10)
		{
			count++;
			temp /= 10;
		}
		// output entry
		printf(", file_type:%d, file_size:", ftype);
		count = 7 - count; // align
		for (j = 0; j < count; j++) printf(" ");
		printf("%d", fsize);
		printf("\n");
	}
	return PASS;
}


// int Terminal_Write_Incorrect_Bits() {
// 	TEST_HEADER;
// 	char* buffer;
// 	buffer = "hello";
// 	terminal_open();
// 	if (Terminal_Write(0, buffer, 99999) != 5) return FAIL;
// 	if (Terminal_Write(0, buffer, 1) != 1) return FAIL;
// 	terminal_close();

// 	return PASS;
// }

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	// TEST_OUTPUT("divide_error_test", divide_error_test());
	// TEST_OUTPUT("paging_test", paging_test());
	// TEST_OUTPUT("breakpoint_test", breakpoint_test());
	//TEST_OUTPUT("overflow_test", overflow_test());
	// TEST_OUTPUT("bound_test", bound_test());
	// TEST_OUTPUT("Seg_NP_test", Seg_NP());
	// TEST_OUTPUT("SIM TEST", sim_test());
	// TEST_OUTPUT("Machine Test", machine_test());
	// TEST_OUTPUT("Misallighment Test", missalignment_test());
	// TEST_OUTPUT("FPU Test", FPU_test());
	// TEST_OUTPUT("Protection", Gen_Prot_test());
	// TEST_OUTPUT("Stack Segment Test", Stack_Seg_test());
	// TEST_OUTPUT("Segment Not presnt", Seg_NP());

	//-----------CHECKPOINT 2------------//
	//// TEST_OUTPUT("Terminal Write Incorrect Bits Test", Terminal_Write_Incorrect_Bits());

	/* Checkpoint 2 tests */

	//TEST_OUTPUT("open file test", open_file_test());
	//TEST_OUTPUT("close file test", close_file_test());
	TEST_OUTPUT("read file test", read_file_test());
	// TEST_OUTPUT("read directory test", read_directory_test());



	// TEST_OUTPUT("rtc test", RTC_TEST());
	// TEST_OUTPUT("Basic Terminal Write", Terminal_Write_Test());

}
