#ifdef _NO_X_
#else
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Tree.h>
#endif

typedef char bool;
typedef enum {up, down, stay} direct;
struct label_str {char *exp; char *abbr;};

#ifdef __ANSI__
void insert_node1(struct label_str label, char *info, direct d);
#else
void insert_node1();
#endif
#ifdef __ANSI__
char *symb(char * var);
#else
char *symb();
#endif
#ifdef __ANSI__
bool streq(char *s1, char *s2);
#else
bool streq();
#endif
#ifdef __ANSI__
void fold(Widget node);
#else
void fold();
#endif
#ifdef __ANSI__
void toggle_callback(Widget, XtPointer, XtPointer);
#else
void toggle_callback();
#endif
#ifdef __ANSI__
void set_subtree_bitmaps(XtGravity);
#else
void set_subtree_bitmaps();
#endif
extern bool symbs;
extern int tdmax;
#ifndef _NO_X_
extern Widget treeWidget, form;
extern XtAppContext app_con;
#ifdef __ANSI__
extern Boolean workproc(XtPointer dummy);
#else
extern Boolean workproc();
#endif
#endif

#define NODE_INFO "node_info"
#define LABEL0 "label0"
#define LABEL1 "label1"
     

