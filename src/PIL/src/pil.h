/******************************************************
 *     PIL System
 *     $Id: pil.h,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: pil.h,v $
 *     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
 *     new CVS project
 *
******************************************************/

#define NSYMS 50    /* max number of symbols */
#define STRLEN 100  /* max length of identifier */


extern int yylex_line;
extern int yydebug;
// JSC port to PPClinux extern char  yytext[];
extern char  *yytext;
extern int state;   /* parser in ojects-section or not? */

extern int sorts_flag;         /* sort-info */
extern int sorts_minimal_flag; /* sort-info */
extern int sorts2_flag;

extern struct strlist *strlist;
extern struct symlist *symlist; /*pointer to symboltable */
extern struct strlist *strlist_proto, *strlist_assump, *strlist_conject; /*stringlist of ... */


#define DEFS_BEGIN 0 /* used by global variable state */
#define INIT_BEGIN 1 /* used by golbal variable state */

struct symtab {
	char *name;        /* pure name */
	char *sorts;       /* name with sortinformation, if optional sorts */
        char *sorts2;      /* pure sortinformation, if optional sorts */
        char *ilf;         /* latex information for ilf */
	int type;          /* symbol-type: NONCE, PRIVKEY, etc. */
	int ground;        /* with variable-attribute ? */
};

extern struct symtab  symtab[NSYMS]; /* symboltable */

extern struct symtab empty;          

struct symlist {                     /* list of symbolpointers */
	struct symtab  *symp;
	struct symlist *next;
};


struct strlist {                     /* list of stringpointers */
	char *string;
	int first;
	struct strlist *next;
	int number;
	int depth_cons;
	int depth_believes;
	int depth_terms;
	int ground;
};


int symtype;                        /* global symboltype */
int variable;                       /* global variable-attribute */

/* which logic is used ? */
enum  which_logic {
	BAN,
	AUTLOG
	} ;

extern enum which_logic used_logic;

#define TRUE 1
#define FALSE 0

#ifndef LIBDIR
#define LIBDIR "../lib"
#endif
#define default_rulesfile_BAN "ban-rules.lop"
#define default_rulesfile_AUTLOG "autlog-rules.lop"
#define default_skelfile  "Makefile.skel"
#define default_makefile  "Makefile."
#define default_pil_extension ".pil"
#define default_lop_extension ".lop"
#define default_ilf_extension ".ilf"


#define string_ilf_name            "struct 500   %s   :- \"%s\"\n"
#define string_ilf_name_name       "struct 500   %s   :- \"%s_%s\"\n"
#define string_ilf_name_name_name  "struct 500   %s   :- \"%s_{%s,%s}\"\n"
#define max_length_string_ilf strlen(string_ilf_name_name_name)

#define privkey_sign "_"
#define privkey_string "inv(%s)"
struct symtab *symlook();
struct symtab *keysymlook();

struct symlist *new_list_of_symp();
struct symlist *add_list_of_symp();
void del_list_of_symp();

struct strlist *new_list_of_strp(int d_cons,int d_believes,int d_terms,int ground,char *fmt, ...);
struct strlist *mod_list_of_strp(int d_cons,int d_believes,int d_terms,int ground,struct strlist *strlist, char *fmt, ...);
struct strlist *add_list_of_strp();
void del_list_of_strp();




struct lop_formula {
  char *formula;
  int cons;
  int believes;
  int terms;
  int ground;
};

extern struct lop_formula ID_bel_formula;
extern struct lop_formula ID_sees_formula;
extern struct lop_formula ID_said_formula;
extern struct lop_formula ID_said_recently_formula;
extern struct lop_formula ID_controls_formula;

extern struct lop_formula holds_sees_formula;
struct strlist *mod_statement_ID (struct symtab *symp , struct strlist *strlist, struct lop_formula formula);


extern struct lop_formula holds_formula;
extern struct lop_formula fresh_formula;
extern struct lop_formula recognizes_formula;
struct strlist *mod_statement (struct strlist *strlist, struct lop_formula formula);


extern struct lop_formula sk_formula;
extern struct lop_formula pub_formula;
extern struct lop_formula ss_formula;
struct strlist *new_statement_key (struct symtab *symp1, struct symtab *symp2,
				   struct symtab *symp3, struct lop_formula formula);

extern struct lop_formula holds_bel_formula;
struct strlist *new_statement_ID_list (struct symtab *symp , struct strlist *strlist, struct lop_formula formula);


extern struct lop_formula nonce_formula;
extern struct lop_formula principal_formula;
struct strlist *new_statement_nonce (struct  symtab *symp, struct lop_formula formula);



struct strlist *process_strlist (struct strlist *strlist);

struct strlist *mod_protocol_ID_list (struct symtab *symp , struct strlist *strlist, struct lop_formula formula);


extern struct lop_formula encr_formula;
extern struct lop_formula comb_formula;

struct strlist *mod_protocol_list_ID ( struct strlist *strlist,struct symtab *symp , struct lop_formula formula);



