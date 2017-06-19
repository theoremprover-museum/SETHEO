#ifndef INIT_H
#define INIT_H

void  init_seth_mem ();
/* void  init_seth_mem (
 *#ifdef CONSTRAINTS
 *       int c_stack_size, 
 *       int pu_trail_size,
 *#endif
 *       int stacksize,
 *       int heapsize,
 *       int codesize,
 *       int trailsize,
 *       int symb_size)
 *
 * allocate space for the memory areas of the SAM
 * Note: 
 * 1) all areas into which tagged pointers are 
 * pointing (stack,heap,constrain_stack), must
 * start on addresses in the area
 * between 0x00000000 .. 0x0fffffff for sun or 
 * between 0xT0000000 .. 0xT0ffffff for hpux 
 * where T is the PTR_MASK set in tags.h
 * (this requirement is checked by ADDRESS_OUT_OF_RANGE(...) )
 * 2) the addresses in heap must be LOWER than that
 * on the stack
 */

void  init_reg ();
/* void  init_reg (); 
 *
 * initialize setheo registers                        
 */

void alloc_code();
/* void alloc_code(int codesize);
 *
 *      allocate memory for code-area              
 *	  size = size in WORDs                       
 */

void alloc_symbtab();
/* void alloc_symbtab(int size);
 *
 *      allocate memory for symbol-table            
 *      size in syel's                            
 */

#endif
