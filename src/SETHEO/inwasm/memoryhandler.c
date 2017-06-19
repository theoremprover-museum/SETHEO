/*****************************************************************************
;;; MODULE NAME : memoryhandler.c
;;;
;;; PARENT      : 
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : 
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 06.03.98
;;;
;;; DESCRIPTION : kind of simple garbage collection and memory count
;;;
;;; REMARKS     :
******************************************************************************/

#include "types.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/memoryhandler.c)                                        */
/*****************************************************************************/

/*****************************************************************************/
/* cnt_malloc()                                                              */
/* Description: allocates memory and update the variable memoryused          */
/*****************************************************************************/
double *cnt_malloc(size)
int size;
{
  double *pointer;
  pointer = (double *)malloc(size);
  if (inwasm_flags.memorylimit && pointer != NULL) {
    memoryused = memoryused + (double)size;
    /* printf("  MEM: %12.0f Byte / %10.2f MByte : (+%d)\n", 
       memoryused, memoryused/1048576, size); */
    if (memoryused > memorylimit) {
      fprintf(stderr, "  Error: Specified memorylimit (%1.0f Byte) reached.\n", memorylimit);
      exit(80);
    }
  } 

  return pointer;
}

void free_termtype(term) 
termtype *term;
{
  int size = sizeof(termtype);

  if (term->t_list != NULL)
    free_termtype(term->t_list);

  if (term->next != NULL)
    free_termtype(term->next);

  free(term);
  if (inwasm_flags.memorylimit) {
    memoryused = memoryused - size;
    /* printf("  MEM: %12.0f Byte / %10.2f MByte : (-%d)\n", 
       memoryused, memoryused/1048576, size); */
  }

  return;
}
