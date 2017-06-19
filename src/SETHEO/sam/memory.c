/*************************************************************/
/*    S E T H E O                                            */
/*                                                           */
/* FILE:       memory.c                                      */
/* VERSION:                                                  */
/* DATE:                                                     */
/* AUTHOR:                                                   */
/* NAME OF FILE:                                             */
/* DESCR:                                                    */
/* MOD:                                                      */
/* BUGS:                                                     */
/*************************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"



#define DEBUG(X) 

#ifdef linux

static char *mem_space;
static char *mem_space_top;
static char *mem_ptr;
/*************************************************************/
/* malloc-functionality for Linux
/* to overcome the separation of real and virtual memory
/* spaces
/*************************************************************/

void sam_memory_init(int size)
{
//JSC-092016mem_space = (char *)calloc(size,sizeof(WORD));
mem_space = (char *)malloc(size*sizeof(WORD));

mem_space_top= mem_space + size *sizeof(WORD);

mem_ptr = mem_space;

if (!mem_space){
       sam_error("memory-space not created", NULL, 2);
	}

    if (ADDRESS_OUT_OF_RANGE(mem_space)) {
	sam_error("address for mem_space out of range",mem_space,3);
    }
DEBUG(printf("sam_memory_init(%d)=%lx\n",size,mem_space));
}

char *sam_malloc(unsigned size)
{
/* size in bytes */
char *mp;

#ifdef JSC_092016
if (mem_ptr + size >= mem_space_top){
       sam_error("out of memory-space", NULL, 2);
	}
mp=mem_ptr;
mem_ptr += size;

DEBUG(printf("sam_malloc(%d)=%lx\n",size,mp));
return mp;
#else

return malloc(size);
#endif
}

char *sam_calloc(int n, unsigned size)
{
/* size in bytes */
char *mp;

if (mem_ptr + n* size >= mem_space_top){
       sam_error("out of memory-space", NULL, 2);
	}
mp=mem_ptr;
while (mp < mem_ptr + n*size){
	*mp++ = '\0';
	}
mp=mem_ptr;
mem_ptr += n*size;

DEBUG(printf("sam_calloc(%d,%d)=%lx\n",n,size,mp));
return mp;
}

#endif
