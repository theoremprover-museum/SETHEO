#ifndef SIGHAND_H
#define SIGHAND_H

typedef struct dbcmd {
	char *name;
	int (*func)();
	char *descr;
	} command;

#endif
