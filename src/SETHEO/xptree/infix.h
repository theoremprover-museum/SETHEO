
#include <stdio.h>

/*********************************************************/
/* structure for a node in the term tree
/*********************************************************/
typedef struct expr {
	char 	*name;
	struct expr	*next;	
	struct expr	*down;	
	} *exprptr;

/*********************************************************/
/* type for an operator
/*********************************************************/
typedef enum {infix,prefix,postfix} operator_type;

/*********************************************************/
/* structure for the symbol table (conversion table)
/* for the operators
/*********************************************************/
typedef struct st_symbtabel {
	char *parsename;
	char *printname;
	int  bp;
	operator_type type ;
	} symbtabel, *symbtab;

/*********************************************************/
/* function declarations
/*********************************************************/
exprptr mkleaf();
exprptr mknode();
exprptr mkcons();
exprptr mknode3();

struct label_str convert_to_string();

/*********************************************************/
/* externals
/*********************************************************/
extern symbtab conversiontable;
extern int nrentries;

/*********************************************************/
/* maximal number of entries in the conversion table
/*********************************************************/
#define MAXENTRIES_CONVERSIONTABLE 100

#define CONVENTER(I,PARSENAME,PRINTNAME,BP,TYPE) \
	conversiontable[I].parsename=PARSENAME; \
	conversiontable[I].printname=PRINTNAME; \
	conversiontable[I].bp=BP; \
	conversiontable[I].type=TYPE; 
