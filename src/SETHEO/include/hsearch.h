
#ifndef __LINUX__

#include <search.h>

#else

typedef struct entry { char *key, *data; } ENTRY;


#if defined(__STDC__)

int hcreate(size_t);
void hdestroy(void);
ENTRY *hsearch(ENTRY, int ACTION);
#if defined(__EXTENSIONS__) || !defined(_XOPEN_SOURCE) || defined(_XPG4_2)
void insque(void *, void *);
void remque(void *);
#endif

#endif


#endif

