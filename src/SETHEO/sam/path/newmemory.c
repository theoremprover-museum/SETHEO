/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    newmemory.c                                * */
/* *                                                        * */
/* *  VERSION:   1.1                                        * */
/* *  MODIFIED:  May 2  1993 by Weidenbach                  * */
/* *                                                        * */
/* *  AUTHORS:   P. Graf (graf@mpi-sb.mpg.de)               * */
/* *             C. Weidenbach (weidenb@mpi-sb.mpg.de)      * */
/* *             MPI fuer Informatik                        * */
/* *             Im Stadtwald                               * */
/* *             6600 Saarbruecken                          * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#include "path/newmemory.h"

/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  DEFINITION OF LOCAL VALUES                            * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

int memory_PAGESIZE;
static int memory__EOF = EOF;

/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  MEMORY MANAGEMENT FUNCTIONS                           * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#define DEBUG 0

#if DEBUG
void memory_Init()
/**************************************************************
INPUT:   None.
RETURNS: None.
SUMMARY: Initializes the memory management.
***************************************************************/
{
  register int i;
  
  memory_FREEDBYTES = 0;
  memory_NEWBYTES   = 0;
  memory_PAGESIZE   = 
#		ifdef SUNOS4
		syscall(SYS_getpagesize);
#		else
		PAGESIZE;
#		endif

  for (i=0; i<memory__MAXSIZE; i++) {
    memory_ARRAY[i].free    = &memory__EOF;
    memory_ARRAY[i].new     = &memory__EOF;
    memory_ARRAY[i].page    = &memory__EOF;
    memory_ARRAY[i].size    = 
      memory__MARKSIZE + i + ((i % memory__ALIGN) ? (memory__ALIGN - (i % memory__ALIGN)) : 0);
    memory_ARRAY[i].offset  = 
      (memory_PAGESIZE / memory_ARRAY[i].size - 1) * memory_ARRAY[i].size + memory__MARKSIZE;
  }
}



POINTER memory_Malloc(Bytes)
/**************************************************************
INPUT:   A number which corresponds to the size of a memory
         requirement.
RETURNS: A pointer to a block of "bytes" bytes.
***************************************************************/
unsigned Bytes;
{
  char *NewMemory;

  if (Bytes > (memory__MAXSIZE-1) || Bytes == 0) {
    printf("\n\n Tried to allocate %d Bytes , which is not supported by memory_Malloc\n", Bytes);
    DumpCore();
  }

  /*jsc*/printf("Malloc: %d Bytes request NewPointer: %lx \n", Bytes, (int)memory_ARRAY[Bytes].new);

  if (*((int *)memory_ARRAY[Bytes].free) == EOF) {
    
    if (*((int *)memory_ARRAY[Bytes].new) == EOF) {      /* Allocate a new page of data. */

      NewMemory  = (char *)sam_calloc(memory_PAGESIZE, sizeof(char));
      
      if (NewMemory==NULL)
	Error__Out(("No more memory available"));

      *((POINTER *)NewMemory) = memory_ARRAY[Bytes].page; /* First Block used to chain pages */
      memory_ARRAY[Bytes].page = NewMemory;
      memory_NEWBYTES += memory_ARRAY[Bytes].size;

      memory_ARRAY[Bytes].new = NewMemory + 2 * memory_ARRAY[Bytes].size + memory__MARKSIZE;
      /*printf("Bytes: %d Block Size: %d New Memory: %d Page Size: %d LastBlock: %d\n",
	     Bytes, memory_ARRAY[Bytes].size, (int)NewMemory, memory_PAGESIZE, 
	     (int)(NewMemory + memory_ARRAY[Bytes].offset));*/

      *((int *)(NewMemory + memory_ARRAY[Bytes].offset)) = EOF;

      return(NewMemory + memory_ARRAY[Bytes].size + memory__MARKSIZE);
    }
    else {                                    /* Use new block from actual page */

      NewMemory = (char *)(memory_ARRAY[Bytes].new);
      memory_ARRAY[Bytes].new = NewMemory + memory_ARRAY[Bytes].size;
      memory_NEWBYTES += memory_ARRAY[Bytes].size;
      
      if ((int)NewMemory >= (int)memory_ARRAY[Bytes].page + (int)memory_ARRAY[Bytes].offset) {
	printf("\n\n Address Overflow\n");
	DumpCore();
      }

      return(NewMemory);
    }}
  else {                                   /* Use already freed block */
      
    NewMemory = (char *)(memory_ARRAY[Bytes].free);

    *((int *)(NewMemory - memory__MARKSIZE)) = 0;

    memory_ARRAY[Bytes].free = *((POINTER *)(memory_ARRAY[Bytes].free));
    memory_FREEDBYTES -= memory_ARRAY[Bytes].size;

    return(NewMemory);
  }
}
#else
void memory_Init()
/**************************************************************
INPUT:   None.
RETURNS: None.
SUMMARY: Initializes the memory management.
***************************************************************/
{
  int i;
  
  memory_FREEDBYTES = 0;
  memory_NEWBYTES   = 0;
  memory_PAGESIZE   =
#		ifdef SUNOS4
		syscall(SYS_getpagesize);
#		else
		PAGESIZE;
#		endif

  for (i=1; i<memory__MAXSIZE; i++) {
    memory_ARRAY[i].free    = &memory__EOF;
    memory_ARRAY[i].new     = &memory__EOF;
    memory_ARRAY[i].page    = &memory__EOF;
    memory_ARRAY[i].size    = 
      i + ((i % memory__ALIGN) ? (memory__ALIGN - (i % memory__ALIGN)) : 0);
    memory_ARRAY[i].offset  = 
      (memory_PAGESIZE / memory_ARRAY[i].size - 1) * memory_ARRAY[i].size;
  }
}



POINTER memory_Malloc(Bytes)
/**************************************************************
INPUT:   A number which corresponds to the size of a memory
         requirement.
RETURNS: A pointer to a block of "bytes" bytes.
***************************************************************/
unsigned Bytes;
{
  char *NewMemory;

  if (*((int *)memory_ARRAY[Bytes].free) == EOF) {
    
    if (*((int *)memory_ARRAY[Bytes].new) == EOF) {      /* Allocate a new page of data. */

      NewMemory  = (char *)sam_calloc(memory_PAGESIZE, sizeof(char));
      
      if (NewMemory==NULL)
	Error__Out(("No more memory available"));

      *((POINTER *)NewMemory) = memory_ARRAY[Bytes].page; /* First Block used to chain pages */
      memory_ARRAY[Bytes].page = NewMemory;
      memory_NEWBYTES += memory_ARRAY[Bytes].size;

      memory_ARRAY[Bytes].new = NewMemory + 2 * memory_ARRAY[Bytes].size;

      *((int *)(NewMemory + memory_ARRAY[Bytes].offset)) = EOF;

      return(NewMemory + memory_ARRAY[Bytes].size);
    }
    else {                                    /* Use new block from actual page */

      NewMemory = (char *)(memory_ARRAY[Bytes].new);
      memory_ARRAY[Bytes].new = NewMemory + memory_ARRAY[Bytes].size;
      memory_NEWBYTES += memory_ARRAY[Bytes].size;
      
      return(NewMemory);
    }}
  else {                                   /* Use already freed block */
      
    NewMemory = (char *)(memory_ARRAY[Bytes].free);

    memory_ARRAY[Bytes].free = *((POINTER *)(memory_ARRAY[Bytes].free));
    memory_FREEDBYTES -= memory_ARRAY[Bytes].size;

    return(NewMemory);
  }
}
#endif

void memory_Print()
/**************************************************************
INPUT:   None.
RETURNS: None.
SUMMARY: Prints the number of used bytes to the screen.
***************************************************************/
{
  int Pages;
  int i;
  POINTER ActPage;

  Pages = 0;
  for(i=1;i<memory__MAXSIZE;i++) {
    ActPage = memory_ARRAY[i].page;
    while (*((int *)ActPage) != EOF) {
      Pages++;
      ActPage = *((POINTER *)ActPage);
    }
  }
    
  printf("\n###\n");
  printf("### Pagesize: %d\n",memory_PAGESIZE);
  printf("### Marksize: %d\n",memory__MARKSIZE);
  printf("### Memory demanded:  %d Bytes\n", memory_NEWBYTES);
  printf("### Memory freed:     %d Bytes\n", memory_FREEDBYTES);
  printf("### Memory remaining: %d Bytes\n", memory_NEWBYTES-memory_FREEDBYTES);
  printf("### Pages allocated:  %d Pages\n", Pages);
  printf("###\n");
}

void memory_PrintDetailed()
/**************************************************************
INPUT:   None.
RETURNS: None.
SUMMARY: Prints the number of used bytes to the screen.
***************************************************************/
{
  int i;
  POINTER ActPage, ActData, ActFree;

  printf("EOF Pointer: %d\n", (int)(&memory__EOF));

  for (i=1; i<memory__MAXSIZE; i++) {
    printf("\n\n Entry: %d Size: %d\n", i , memory_ARRAY[i].size);
    ActPage  = memory_ARRAY[i].page;
    ActData  = memory_ARRAY[i].new;
    ActFree  = memory_ARRAY[i].free;

    if (*((int *)ActPage) == EOF)
      if (*((int *)ActData) == EOF)
	printf("   No request so far\n");
      else {
	Error__Out(("No Page entry but New entry"));
      }
    else {
      printf("   Current Page: %d Current New: %d\n", (int)ActPage, (int)ActData);
      while(*((int *)ActPage) != EOF) {
	printf("\n   Page: %d Next Page: %d\n", (int)ActPage, *((int *)ActPage));

#if DEBUG
	ActData = ((char *)ActPage + memory__MARKSIZE);
#else
	ActData = ActPage;
#endif

	printf("   Data:");
	while (*((int *)ActData) != EOF) {
	  printf("%d ", (int)ActData);
	  ActData = (char *)ActData + memory_ARRAY[i].size;
	}
	ActPage = *((POINTER *)ActPage);
      }
      if (*((int *)ActFree) == EOF)
	printf("\n   No freed memory\n");
      else {
	printf("\n   Free: ");
	while(*((int *)ActFree) != EOF) {
	  printf("%d ", (int)ActFree);
	  ActFree = *((POINTER *)ActFree);
	}
      }  
    }
  }
}



/**************************************************************/
/*                      END: newmemory.c                      */
/**************************************************************/

