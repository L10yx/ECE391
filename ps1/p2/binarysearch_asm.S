/*
 * tab:2
 *
 * search_asm.S - Implementation of Assembly Recursive DFS
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Aamir Hasan
 * Version:       1
 * Creation Date: Fri Aug 30 2020
 * Filename:      search_asm.S
 * History:
 *    AS    1    Fri Aug 30 2020
 *        First written.
 */

.global binarySearch_asm

# Search ASM (Assembly)
# Searches for an element in a BST
# Declaration is in search.h
#
# Registers:
#   eax - Return Value
# 
binarySearch_asm:
  pushl %ebp
  movl %esp, %ebp

  #--- YOUR CODE HERE ---
  subl $4, %esp          # reserve 4 bytes on stack for 1 local vars called mid
  pushl	%edi
  pushl	%esi
  pushl	%ecx
  pushl	%ebx

  movl 8(%ebp), %ebx      # load arr into EBX 
  movl 12(%ebp), %ecx     # load md into ECX
  movl 16(%ebp), %esi      # load low into ESI 
  movl 20(%ebp), %edi     # load high into EDI

  cmpl	%edi, %esi      # load low - high into flag
  ja		not_exist       # if low > high return -1

  movl	$0, %eax      # set eax to 0   
  addl  %esi,%eax
  addl	%edi, %eax    # low + high -> eax
  shrl	%eax            # eax = (low+high)/2
  
  cmpl	%ecx,(%ebx,%eax,4) # arr[mid] - md
  je		found
  ja		mid_larger_than_md
  movl	%eax, %esi  # mid -> esi
  incl  %esi        # new lower bound is mid+1

  pushl	%edi
  pushl	%esi
  pushl	%ecx
  pushl	%ebx
  call	binarySearch_asm
  popl	%ebx
  popl	%ecx
  popl	%esi
  popl	%edi
  
  

mid_larger_than_md:
  movl	%eax, %edi
  decl  %edi

  pushl	%edi
  pushl	%esi
  pushl	%ecx
  pushl	%ebx
  call	binarySearch_asm
  popl	%ebx
  popl	%ecx
  popl	%esi
  popl	%edi

found: 
  movl	$1, %eax
  jmp		finish_search
  
  
not_exist:
  movl	$-1, %eax
  
finish_search:
  popl	%ebx
  popl	%ecx
  popl	%esi
  popl	%edi
  addl	$4, %esp

  #----------------------

  leave
  ret
