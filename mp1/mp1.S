
.data					# section declaration

        # Useful offset constants for accessing members of a 
        # struct mp1_blink_struct structure
        LOCATION   = 0    /*unsigned short*/
        ON_CHAR    = 2          
        OFF_CHAR   = 3 
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12

        STRUCT_SIZE = 16

# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0

.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl

/*cmd value Core function       Description
  0         mp1 ioctl add       add a blinking location
  1         mp1 ioctl remove    remove a blinking location
  2         mp1 ioctl find      get information about a location
  3         mp1 ioctl sync      synchronize a new blink location with an existing one
  other - Any value other than 0-3 is an error. Return -1.
  */
ioctl_jump_table:           
        .long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find, mp1_ioctl_sync

# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX
mp1_poke:
        
	movl    vmem_base_addr(,1), %edx
	movb    %cl, (%edx,%eax,1)
	ret

/*
Every time an RTC interrupt is generated, mp1 rtc tasklet will becalled. 
tasklet will walk down the mp1_list_head list, examining each mp1_blink_struct structure.
*/

mp1_rtc_tasklet:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi

        movl    mp1_list_head, %edi   /*%edi - pointer to traverse the list*/ 
tasklet_loop:
        cmpl    $0, %edi               
        je      tasklet_tear_down    /* reach the tail of list if 0*/   
        decw    COUNTDOWN(%edi)         /*The function first decrements the countdown field of the structure*/    
        cmpw    $0, COUNTDOWN(%edi)    
        je      tasklet_status      /*If the countdown field has reached zero after the decrement, the tasklet will examine the status field*/
tasklet_next:
        movl    NEXT(%edi), %edi
        jmp     tasklet_loop

tasklet_status:
        cmpw    $0, STATUS(%edi)        
        je      tasklet_on              /*If equal to 0, that location currently has the off_char character*/
        
        movzwl  LOCATION(%edi), %eax    
        shll    %eax                    /*Each character on the text display comprises two bytes in memory*/
        movb    OFF_CHAR(%edi), %cl
        call    mp1_poke
        movw    OFF_LENGTH(%edi), %dx       /* set countdown to on_length and invert status */
        jmp     tasklet_update
tasklet_on:                             /* The tasklet should put the opposite character
        (i.e. interchange the status between on/off)out to video memory */              
        movzwl  LOCATION(%edi), %eax     
        shll    %eax                    /*Each character on the text display comprises two bytes in memory*/
        movb    ON_CHAR(%edi), %cl
        call    mp1_poke
        movw    ON_LENGTH(%edi), %dx      
tasklet_update:
        xorw    $1, STATUS(%edi) /* 0 <-> 1 */
        movw    %dx, COUNTDOWN(%edi) /* updates the countdown field by copying the value from the opposite length field to countdown*/
        jmp     tasklet_next  

tasklet_tear_down:        
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret
/*
int mp1_ioctl (unsigned long arg, unsigned long cmd);
This function serves as a “dispatcher” function. It uses the cmd argument to determine which of the next four functions to jump to. 
The table below gives a brief summary of cmd values, the corresponding core function, and a brief description of what that core function does. 
Each of the core functions are described in the section entitled “Core Functions.”
Note that you must check this cmd value; if it is an invalid command, return -1
*/
mp1_ioctl:
        movl    8(%esp), %eax    /*get cmd*/
        cmpl    $0, %eax          /*range left edge 0*/ 
        jb      ioctl_invalid   /*Any value other than 0-3 is an error. Return -1*/
        cmpl    $3, %eax         /*range right edge 3*/
        ja      ioctl_invalid
        jmp     *ioctl_jump_table(,%eax,4)    /* jump to M[EAX * 4 + ioctl_jump_table] */
ioctl_invalid:  
        movl    $-1, %eax          /* return -1 if invalid */
	ret

/*
int mp1 ioctl add(unsigned long arg);
The add ioctl takes as its argument a user-level pointer to a mp1 blink struct structure. 
First, dynamically allocate memory using the mp1 malloc function to store a copy of the structure. 
Copy the entire mp1 blink struct
from the user-space structure to the newly-allocated memory (use mp1 copy from user). 
Then set the countdown field to be equal to the on_length field, and set the status field to 1. 
Then insert this structure at the head of the linked list using the mp1 list head pointer. 
Finally, make a call to mp1 poke with the correct register parameters to immediately display the character on the text-mode video screen. 
This effectively turns the location “on.” 
After countdown RTC interrupts have elapsed, your mp1 rtc tasklet will turn the location “off.” 
This function should return 0 if a successful add was performed.
Your function must handle errors. 
If there is a memory allocation error (in which case mp1 malloc returns NULL), return -1. 
Remember the semantics of mp1 copy from user. If it could not copy all the bytes requested, it will return
the number of bytes it was not able to copy. 
If this function returns anything other than 0, the copy has failed, and the function should return -1. 
If the location is outside the valid range of 0 to 80*25-1, this function should return -1.
Finally, your error handling must prevent memory leaks. 
If you have allocated any memory using mp1 malloc, and you find that there is an error condition, you must free the memory using mp1 free
*/
mp1_ioctl_add:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi

        pushl   $STRUCT_SIZE      
        call    mp1_malloc      /* dynamically allocate memory using the mp1_malloc function 
                                to store a copy of the structure */
        add     $4, %esp
        cmpl    $0, %eax        /*check if malloc succeed*/     
        je      add_malloc_error
  
        movl    %eax, %esi      /* %esi to store address to newly allocated memory*/
        movl    8(%ebp), %ebx   /* get arg: a user-level pointer to a mp1 blink struct structure */
        cmpl    $0, %ebx        /* check if pointer is null */   
        je      add_error

        pushl   $STRUCT_SIZE     /* call copy_from_user function, size = 16*/
        pushl   %ebx
        pushl   %esi
        call    mp1_copy_from_user 
        /*mp1_copy_from_user (void * to, const void * from, unsigned long n);*/
        add     $12, %esp       
  
        cmpl    $0, %eax        /* check if copy fail */ 
        jne     add_error       /* If this function returns anything other than 0, the copy has failed,*/

        cmpw    $0, LOCATION(%esi)       /* check if the location is outside the valid range of 0 to 80*25-1 */
        jb      add_error
        cmpw    $80*25, LOCATION(%esi)
        jae     add_error

        movw    ON_LENGTH(%esi), %ax    /* Then set the countdown field to be equal to the on length field */
        movw    %ax, COUNTDOWN(%esi)
        movw    $1, STATUS(%esi)        /* set the status field to 1*/

        movl    mp1_list_head, %eax     /* insert this structure at the head of the linked list using the mp1 list head pointer */      
        movl    %eax, NEXT(%esi)
        movl    %esi, mp1_list_head

        movzwl  LOCATION(%esi), %eax     /* make a call to mp1 poke with the correct register parameters
                                        to immediately display the character on the text-mode video screen */                  
        shll    %eax                    /*Each character on the text display comprises two bytes in memory*/
        movb    ON_CHAR(%esi), %cl      /* The byte you wish to write */
        call    mp1_poke

        movl    $0, %eax        /*This function should return 0 if a successful add was performed.*/
        jmp     add_tear_down     

add_error:                      /* if error(except malloc), free memory*/
        pushl   %esi 
        call    mp1_free
        add     $4, %esp

add_malloc_error:               /* return -1 if fail*/
        movl    $-1, %eax       

add_tear_down:        
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret
/*
int mp1_ioctl_remove(unsigned long arg) 
The remove ioctl takes an integer location as its parameter.
Traverse the mp1_list_head list, looking for an element whose location field matches the argument given to this function. 
If there is such an element, remove it from the linked list and free its memory with a call to mp1_free, and return 0. 
If there is no element whose location matches, return -1.
regs: 
      %bx - target location
      %edx - pointer to previous node
      %edi - pointer to target node
      %esi - next node of target node
*/        
mp1_ioctl_remove:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi

        movw    8(%ebp), %bx    /* %bx to store target location */
        pushw   %bx             /* location is an unsigned short*/
        call    list_search     /* search the mp1 list head list for an element that matches the location */
        addl    $2, %esp        /* pop location */
        cmpl    $-1, %eax       /* check if found*/
        je      remove_error

        movl    %eax, %edi      /* %edi to store pointer to target node */
        movl    NEXT(%edi), %esi   /* %esi to store next node of target node */
        cmpl    $0, %edx           /* check if target node is head node */
        je      remove_head
        movl    %esi, NEXT(%edx)     /* set NEXT of previous node to next node */
        jmp     remove_free

remove_head:
        movl    %esi, mp1_list_head /* Update head node*/

remove_free:              
        pushl   %edi
        call    mp1_free            /* free target node */ 
        add     $4, %esp
             
        movl    $0, %eax            /* return 0 if success*/
        jmp     remove_tear_down

remove_error:
        movl    $-1, %eax       /* return -1 if fail*/

remove_tear_down:  
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret

/*
int mp1 ioctl find(unsigned long arg)
The find ioctl takes a pointer to a mp1 blink struct, like add. 
The only parameter it is concerned with as an input is the location parameter, 
but you must validate that the pointer refers to a valid structure before reading from the structure. 
After extracting the location parameter from the user-level structure, search the mp1 list head list for an element that matches the location. 
Then copy the entire element, which is a mp1 blink struct, to the user-level structure pointed to by the parameter that was passed in (use mp1 copy to user).
In this way it uses the parameter as both an input and an output. If there is no matching location in the list, return -1,
otherwise return 0. Similar error conditions apply to this function as in the previous two.
regs: %edi - a matched pointer to node in list 
      %ebx - a user-level pointer to a mp1 blink struct structure
      %esi - address to newly allocated memory
*/        
mp1_ioctl_find:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi

        pushl   $STRUCT_SIZE      
        call    mp1_malloc      /* dynamically allocate memory using the mp1_malloc function 
                                to store a copy of the structure */
        add     $4, %esp
        cmpl    $0, %eax        /*check if malloc succeed*/     
        je      find_malloc_error  

        movl    %eax, %esi      /* %esi to store address to newly allocated memory*/
        movl    8(%ebp), %ebx   /* get arg: a user-level pointer to a mp1 blink struct structure */
        cmpl    $0, %ebx        /* check if pointer is null */   
        je      find_error

        pushl   $STRUCT_SIZE    /* call copy_from_user function, size = 16*/
        pushl   %ebx
        pushl   %esi
        call    mp1_copy_from_user 
        /*mp1_copy_from_user (void * to, const void * from, unsigned long n);*/
        add     $12, %esp       
  
        cmpl    $0, %eax        /* check if copy fail */ 
        jne     find_error      /* If this function returns anything other than 0, the copy has failed,*/

        cmpw    $0, LOCATION(%esi)  /* check if the location is outside the valid range of 0 to 80*25-1 */
        jb      find_error
        cmpw    $80*25, LOCATION(%esi)
        jae     find_error



        pushw   LOCATION(%esi)  /*location is an unsigned short*/
        call    list_search     /* search the mp1 list head list for an element that matches the location */
        addl    $2, %esp        /* pop location */
        cmpl    $-1, %eax       /* check if found*/
        je      find_error

        movl    %eax, %edi      /* store return value from list_search to edi*/

        pushl   $STRUCT_SIZE    /* call copy_to_user function, size = 16*/
        pushl   %edi            /* %edi matched pointer to node in list */
        pushl   %ebx            /* %ebx a user-level pointer to a mp1 blink struct structure */
        call    mp1_copy_to_user
        /* unsigned long mp1_copy_to_user(void *to, const void *from, unsigned long n); */
        add     $12, %esp
  
        cmpl    $0, %eax        /* check if copy fail */ 
        jne     find_error      /* If this function returns anything other than 0, the copy has failed,*/

        movl    $0, %eax        /*This function should return 0 if a successful add was performed.*/
        jmp     find_tear_down  

find_error:                     /* if error(except malloc), free memory*/
        pushl   %esi 
        call    mp1_free
        add     $4, %esp

find_malloc_error: 
        movl    $-1, %eax       /* return -1 if fail*/
find_tear_down:        
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret
/*
int mp1 ioctl sync(unsigned long arg)
The sync ioctl’s unsigned long argument is really two two-byte unsigned short integers, packed into one four-byte argument. 
The first integer is stored in the upper 16 bits of arg, and the second integer is stored in the lower 16 bits. 
You must extract these two integers from the single argument.
*/
mp1_ioctl_sync:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi

        movl    8(%ebp), %ebx   /* get arg: a user-level pointer to a mp1 blink struct structure */
        cmpl    $0, %ebx        /* check if pointer is null */   
        je      sync_error

        pushw   %bx             /* the second integer is stored in the lower 16 bits */
        call    list_search
        addl    $2, %esp        /* pop location */
        cmpl    $-1, %eax       /* check if found*/
        je      sync_error

        movl    %eax, %edi      /* %edi to store second integer as sync destination */

        shrl    $16, %ebx       /* The first integer is stored in the upper 16 bits of arg */
        pushw   %bx             
        call    list_search
        addl    $2, %esp        /* pop location */
        cmpl    $-1, %eax       /* check if found*/
        je      sync_error   

        movl    %eax, %esi      /* %esi to store first integer as sync source*/
        
        /* Then copy the timing information (the on length, off length, countdown, and status
            4 fields) from the first element to the second element*/
        movw    ON_LENGTH(%esi), %ax
        movw    %ax, ON_LENGTH(%edi)
        movw    OFF_LENGTH(%esi), %ax
        movw    %ax, OFF_LENGTH(%edi)
        movw    COUNTDOWN(%esi), %ax
        movw    %ax, COUNTDOWN(%edi)
        movw    STATUS(%esi), %ax
        movw    %ax, STATUS(%edi)

        cmpw    $0, %ax       
        je      sync_off         /*If equal to 0, that location currently has the off_char character*/

        /*After copying these fields, call mp1 poke to immediately update the display using the correct character 
        (that is, either the on char or the off char, depending on status) to the screen for the second location. 
        This function should return 0 on success, and -1 on failure. Similar failure cases apply*/
sync_on:  
        movzwl  LOCATION(%edi), %eax     
        shll    %eax                    /*Each character on the text display comprises two bytes in memory*/
        movb    ON_CHAR(%edi), %cl
        call    mp1_poke
        jmp     sync_success
sync_off:
        movzwl  LOCATION(%edi), %eax     
        shll    %eax                    /*Each character on the text display comprises two bytes in memory*/
        movb    OFF_CHAR(%edi), %cl
        call    mp1_poke


sync_success:                 /* return 0 if success*/
        movl    $0, %eax
        jmp     sync_tear_down

sync_error:
        movl    $-1, %eax    /* return -1 if fail*/

sync_tear_down:        
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret
/*  *int list_search (unsigned short arg)
func: search the mp1 list head list for an element that matches the location
regs: %esi - pointer to each node
input: %ax - target arg
output: %eax - pointer to the node if success or -1 if fail
        %edx - pointer to previous node
*/
list_search:
        pushl   %ebp            /* calling convention */
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi


        movw    8(%ebp), %ax     /* %ax to store target location */
        movl    mp1_list_head, %esi   /* %esi to store pointer to each node  */
        movl    $0, %edx        /* 0 to indacate no previous node */

search_loop:                    /* traverse list to match target location */
        cmpl    $0, %esi           /* fail if reach tail*/
        je      search_fail

        cmpw    %ax, LOCATION(%esi) /* check if match */
        je      search_sucess

        movl    %esi, %edx          /* %edx to store previous node pointer */
        movl    NEXT(%esi), %esi    /* check next node*/
        jmp     search_loop

search_sucess: 
        movl    %esi, %eax
        jmp     search_tear_down

search_fail:
        movl    $-1, %eax
      
search_tear_down:
        popl    %edi            /*calling convention*/
        popl    %esi
        popl    %ebx
        leave
	ret
.end
