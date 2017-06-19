#ifndef CMDLINE_H
#define CMDLINE_H

typedef enum {req,opt}			optional;

typedef enum {none,string,integer}	argtype;

typedef enum {	ok 	=0,
	noarg	= -1,
	absarg	= -2,
	argnotfound		= -3,
	argmissing		= -4,
	intexpected		= -5
	} cmdline_retval;

typedef	struct {
	int		ident;		/* identifier		*/
	optional	opt;		/* optional parameter?	*/
	char		*name;		/* name of parameter	*/
	argtype		arg_type;	/* type of argument	*/
	optional	opt_arg;	/* is it optional?	*/
	char 		*defstr;	/* default for string	*/
	int		defint;		/* default for int	*/
	} argentry;



extern argentry argtab[];

extern int size_argtab;

int parsearg();
/* int parsearg(char** argvp[], int* argcp, int* intvalp, char** stringvalp, int* negated, char* negstring); */

void  usage();
/* void  usage(FILE* fp, char* av0, char* trailer); */

char *cmd_line_errtxt();
/* char *cmd_line_errtxt(cmdline_retval res); */

#endif





