#include "xptree.h"
#include "treeh.xbm"
#include "treeh_hl.xbm"
#include "treev.xbm"
#include "treev_hl.xbm"
#include <stdlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Porthole.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Panner.h>
#include <X11/Xaw/Tree.h>
#include <X11/Xmu/Converters.h>

struct node_contents {
  char *label0, *label1, *node_info;
  bool label0_ismanaged;
};
typedef struct str_subtree *subtree;
struct str_subtree {
  subtree *sons;
  bool is_node;
  union {
    subtree subt;
    struct node_contents node;
  } st_nd;
};

#define SUBTREE "subtree"
typedef struct str_wd_st {Widget box; subtree subt;} * wd_st;

#ifdef EXT_MALLOC
extern char *malloc(), *calloc();
#endif

#ifdef __ANSI__
static subtree Widget2List(Widget node);
#else
static subtree Widget2List();
#endif
#ifdef __ANSI__
static void subtree_callback(Widget, XtPointer, XtPointer);
#else
static void subtree_callback();
#endif
#ifdef __ANSI__
static void unfold(subtree *sons, Widget node);
#else
static void unfold();
#endif
#ifdef __ANSI__
static void create_st_node(Widget box, wd_st ws);
#else
static void create_st_node();
#endif
#ifdef __ANSI__
void set_subtree_bitmaps(XtGravity closure);
#else
void set_subtree_bitmaps();
#endif



static String buff;
static Pixmap treeh_pm, treeh_hl_pm, treev_pm, treev_hl_pm;

#ifdef __ANSI__
void fold(Widget node) {
#else
void fold(node)
  Widget node;
{
#endif
  static Arg args[5];
  Cardinal num, n;
  Widget w;
  WidgetList wl;
  static XtCallbackRec callback_rec[2] = {{ subtree_callback, NULL },
					    { NULL, NULL }};
  wd_st ws;

  buff = (String) malloc(1000*sizeof(char));

  
  ws = (wd_st) malloc(sizeof(struct str_wd_st));
  ws->subt = Widget2List(node);
  ws->box = node;

  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(node, args, 2);
  for (n=0; n<num; n++) XtDestroyWidget(wl[n]);

  create_st_node(node, ws);
}

  /* SUBTREE einfuegen */
#ifdef __ANSI__
static void create_st_node(Widget box, wd_st ws) {
#else
static void create_st_node(box, ws)
  Widget box;
  wd_st ws;
{
#endif
  static XtCallbackRec callback_rec[2] = {{ subtree_callback, NULL },
					    { NULL, NULL }};

  static Arg args[5];
  XtGravity grav;
  Cardinal n;

  /* Box resourcen setzen */
  XtSetArg(args[0], XtNvSpace, 0);
  XtSetArg(args[1], XtNborderWidth, 0);
  XtSetValues(box, args, 2);

  if (!treeh_pm) {
    treeh_pm = XCreateBitmapFromData(XtDisplay(treeWidget),
      RootWindowOfScreen(XtScreen(treeWidget)),
      treeh_bits, treeh_width, treeh_height);
    treeh_hl_pm = XCreateBitmapFromData(XtDisplay(treeWidget),
      RootWindowOfScreen(XtScreen(treeWidget)),
      treeh_hl_bits, treeh_hl_width, treeh_hl_height);
    treev_pm = XCreateBitmapFromData(XtDisplay(treeWidget),
      RootWindowOfScreen(XtScreen(treeWidget)),
      treev_bits, treev_width, treev_height);
    treev_hl_pm = XCreateBitmapFromData(XtDisplay(treeWidget),
      RootWindowOfScreen(XtScreen(treeWidget)),
      treev_hl_bits, treev_hl_width, treev_hl_height);
  }

  XtSetArg(args[0], XtNgravity, &grav);
  XtGetValues(treeWidget, args, 1); 

  callback_rec[0].closure= (XtPointer) ws; 
  n=0;
  XtSetArg(args[n], XtNcallback, callback_rec); n++;
  XtSetArg(args[n], XtNheight,
	   (grav == NorthGravity) ? treev_hl_height : treeh_hl_height);  n++;
/*  XtSetArg(args[n], XtNwidth,
	   (grav == NorthGravity) ? treev_hl_width : treeh_hl_width); n++; */
  XtSetArg(args[n], XtNbitmap, 
	   (grav == NorthGravity) ? treev_hl_pm : treeh_hl_pm); n++;
  XtCreateManagedWidget(SUBTREE, toggleWidgetClass, box, args, n);
}


/* Widget2List loescht Widget-Unterbaum und gibt die darin enthaltene 
   Information in der rekursiven Datenstruktur 'subtree' zurueck */
#ifdef __ANSI__
static subtree Widget2List(Widget node) {
#else
static subtree Widget2List(node)
  Widget node;
{
#endif
  Arg args[5];
  Cardinal num, n, i;
  Widget w;
  WidgetList wl;
  subtree s;
  XtCallbackList callback_rec;
  subtree stbuff[100];
  wd_st ws;

  s = (subtree) malloc(sizeof(struct str_subtree));

  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(node, args, 2);
  for (n=0; n<num; n++) { 
    if (streq(XtName(wl[n]), SUBTREE)) {
      s->is_node = 0;
      XtSetArg(args[0], XtNcallback, &callback_rec);
      XtGetValues(wl[n], args, 2);
      ws = (wd_st) callback_rec[0].closure;
      s->st_nd.subt = ws->subt;
      break;
    } else if (streq(XtName(wl[n]), LABEL0)) {
      s->is_node = 1;
      s->st_nd.node.label0_ismanaged = XtIsManaged(wl[n]);
      XtSetArg(args[0], XtNlabel, &buff);
      XtGetValues(wl[n], args, ONE);
      s->st_nd.node.label0 = strdup(buff);
    } else if (streq(XtName(wl[n]), LABEL1)) {
      XtSetArg(args[0], XtNlabel, &buff);
      XtGetValues(wl[n], args, ONE);
      s->st_nd.node.label1 = strdup(buff);
    } else if (streq(XtName(wl[n]), NODE_INFO)) {
      XtSetArg(args[0], XtNlabel, &buff);
      XtGetValues(wl[n], args, ONE);
      s->st_nd.node.node_info = strdup(buff);
    } 
  }
  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(treeWidget, args, 2);
  
  if (s->is_node) {
    for (n=0, i=0; n<num; n++) {
      XtSetArg(args[0], XtNtreeParent, &w);
      XtGetValues(wl[n], args, ONE);
      if (node == w) {
	stbuff[i++] = Widget2List(wl[n]);
	XtDestroyWidget(wl[n]);  
      }
    }
    if (!i) {
      s->sons = NULL;
    } else {
      s->sons = (subtree *) malloc((i+1)*sizeof(subtree));
      for (n=0; n<i; n++) s->sons[n] = stbuff[n];
      s->sons[n] = NULL;
    }
  }
  return (s);
}

/* subtree_callback: unfold */
#ifdef __ANSI__
static void subtree_callback(Widget toggle, XtPointer closure, XtPointer data) {
#else
static void subtree_callback(toggle, closure, data)
  Widget toggle;
  XtPointer closure;
  XtPointer data;
{
#endif
  wd_st ws = (wd_st) closure;
  Widget node, l;
  subtree st;
  Arg args[3];
  Cardinal num, n;
  WidgetList wl;
  static XtCallbackRec callback_rec[2] = {{ toggle_callback, NULL },
                                            { NULL, NULL }};

  node = ws->box;
  st = ws->subt;
  XtUnmapWidget(form);

  /* Box resourcen setzen */
  XtSetArg(args[0], XtNborderWidth, 1);
  XtSetValues(node, args, 1);
      

  callback_rec[0].closure= (XtPointer) node;
  XtSetArg(args[0], XtNlabel, st->st_nd.node.label0);
  XtSetArg(args[1], XtNcallback, callback_rec);
  if (st->st_nd.node.label0_ismanaged) {  
    XtCreateManagedWidget(LABEL0, toggleWidgetClass, node, args, 2);
    XtSetArg(args[0], XtNlabel, st->st_nd.node.label1);
    XtCreateWidget(LABEL1, toggleWidgetClass, node, args, 2);
  } else {
    XtCreateWidget(LABEL0, toggleWidgetClass, node, args, 2);
    XtSetArg(args[0], XtNlabel, st->st_nd.node.label1);
    XtCreateManagedWidget(LABEL1, toggleWidgetClass, node, args, 2);
  }

  XtSetArg(args[0], XtNlabel, st->st_nd.node.node_info);
  XtCreateWidget(NODE_INFO, labelWidgetClass, node, args, 1);

  unfold(st->sons, node);
  XtDestroyWidget(toggle);  /* SUBTREE toggle loeschen */
  XtAppAddWorkProc(app_con, workproc, 0);

}

#ifdef __ANSI__
static void unfold(subtree *sons, Widget node) {
#else
static void unfold(sons, node)
  subtree *sons;
  Widget node;
{
#endif
  Arg args[3];
  Cardinal num, n;
  WidgetList wl;
  Widget box;
  static XtCallbackRec callback_rec[2] = {{ NULL, NULL },
                                            { NULL, NULL }};
  struct node_contents *nd;
  wd_st ws;
  
  if (!sons) return;

  for (; *sons; sons++) {
    XtSetArg(args[0], XtNtreeParent, node);
    
    box = XtCreateManagedWidget("dummy", boxWidgetClass, treeWidget, args, 1);
    if ((*sons)->is_node) {
      callback_rec[0].callback = toggle_callback;
      nd = &((*sons)->st_nd.node);
      XtSetArg(args[0], XtNlabel, nd->label0);
      callback_rec[0].closure= (XtPointer) box;
      XtSetArg(args[1], XtNcallback, callback_rec);
      if (nd->label0_ismanaged) {
	XtCreateManagedWidget(LABEL0, toggleWidgetClass, box, args, 2);
	XtSetArg(args[0], XtNlabel, nd->label1);
	XtCreateWidget(LABEL1, toggleWidgetClass, box, args, 2);
      } else {
	XtCreateWidget(LABEL0, toggleWidgetClass, box, args, 2);
	XtSetArg(args[0], XtNlabel, nd->label1);
	XtCreateManagedWidget(LABEL1, toggleWidgetClass, box, args, 2);
      }
      XtSetArg(args[0], XtNlabel, nd->node_info);
      XtCreateWidget(NODE_INFO, labelWidgetClass, box, args, 1);

      unfold((*sons)->sons, box);
    } else {
      callback_rec[0].callback = subtree_callback;
      ws = (wd_st) malloc(sizeof(struct str_wd_st));
      ws ->subt = (*sons)->st_nd.subt;
      ws->box = box;
      callback_rec[0].closure = (XtPointer) ws;

      create_st_node(box, ws);
    }
  }
}
  

/* horizontale Dreiecke gegen vertikale austauschen */
#ifdef __ANSI__
void set_subtree_bitmaps(XtGravity grav) {
#else
void set_subtree_bitmaps(grav)
  XtGravity grav;
{
#endif
  Arg args[3];
  Cardinal num, n, num1, n1;
  WidgetList wl, wl1;
  Pixmap tree_pm;

  if (grav == NorthGravity) tree_pm=treev_pm; else tree_pm=treeh_pm;

  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(treeWidget, args, 2);
  for (n=0; n<num; n++) {  /* alle boxen */
    XtSetArg(args[0], XtNchildren, &wl1);
    XtSetArg(args[1], XtNnumChildren, &num1);
    XtGetValues(wl[n], args, 2);
    for (n1=0; n1<num1; n1++) {
      if (streq(XtName(wl1[n1]), SUBTREE)) {
#ifdef DEBUG
  printf("subtree bitmap\n");
#endif
	XtSetArg(args[0], XtNbitmap, tree_pm);
	XtSetValues(wl1[n1], args, 1);
      }
    }
  }
}

/* ARGSUSED */
void subtree_highlight(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  Arg args[3];
  WidgetList wl;
  Cardinal num, n;
  XtGravity grav;

  if (*num_params!=1 || **params < '0' || **params > '1') return;

  /* Pixmap vom widget w austauschen (highlight an/aus) */

  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(w, args, 2);

  XtSetArg(args[0], XtNgravity, &grav);
  XtGetValues (treeWidget, args, 1);

  for (n=0; n<num; n++)
    if (streq(SUBTREE, XtName(wl[n]))) {
      XtSetArg(args[0], XtNbitmap,(**params == '0') ? 
	       ((grav == NorthGravity) ? treev_pm : treeh_pm) :
	       ((grav == NorthGravity) ? treev_hl_pm : treeh_hl_pm));
      XtSetValues (wl[n], args, 1);
    }
}
