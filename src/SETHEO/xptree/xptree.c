/*
  XProofTREE, Programm zur Beweisbaumausgabe
  Weiterentwicklung von viewres ...
  08.1992, Roland Handl 
  29/30.12.92, Roland Handl: Ueberlappungs-Bug entfernt, filename als Parameter
  15.07.93, Roland Handl: Umstellung von ANSI auf K&R-C
  27.07.93, Roland Handl: anhaengen von ".tree" an filename, falls nicht vorh.
  24.3.93, Schumann, infix operators
*/

#define VERSION "xptree V3.2\n(25 Mar 94)\nby Roland Handl\n& J. Schumann"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h> 
#include <X11/Xaw/SmeBSB.h> 
#include <X11/Xaw/SmeLine.h>    
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Porthole.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Panner.h>
#include <X11/Xaw/Tree.h>

// JSC extern char *malloc(), *calloc();
extern FILE *yyin;
#include "xptree.h"
#include "infix.h"

static Arg sensitiveargs[2] = {
  { XtNsensitive, (XtArgVal) FALSE },
  { XtNsensitive, (XtArgVal) TRUE }};


static XrmOptionDescRec Options[] = {
  { "-vert", "*Tree.Gravity", XrmoptionNoArg, (XPointer) "north" },
  { "-horiz", "*Tree.Gravity", XrmoptionNoArg, (XPointer) "West" }
};

static String fallback_resources[] = {
#include "xptree.ad.h"
  NULL
};

static void ActionQuit(), ActionSetOrientation(), ActionSaveButton();
extern void /*Action*/subtree_highlight();

static void set_orientation_menu();
#ifdef __ANSI__
static void create_td_popup(Widget w);
#else
static void create_td_popup();
#endif

#ifdef __ANSI__
void toggle_callback(Widget toggle, XtPointer closure, XtPointer data);
#else
void toggle_callback();
#endif
#ifdef __ANSI__
void about_callback(Widget aboutcmd, XtPointer closure, XtPointer data);
#else
void about_callback();
#endif
#ifdef __ANSI__
void aboutexit_callback(Widget aboutcmd, XtPointer closure, XtPointer data);
#else
void aboutexit_callback();
#endif
#ifdef __ANSI__
static void select_node(Widget w, Boolean b);
#else
static void select_node();
#endif
#ifdef __ANSI__
static Boolean isselected_node(Widget w);
#else
static Boolean isselected_node();
#endif
#ifdef __ANSI__
Boolean workproc(XtPointer dummy);
#else
Boolean workproc();
#endif
     
static XtActionsRec xptree_actions[] = {
  {"Quit", ActionQuit },
  {"SetOrientation", ActionSetOrientation },
  {"SaveButton", ActionSaveButton },
  {"subtree_highlight", subtree_highlight },
};

static Atom wm_delete_window;
     
#define VIEW_HORIZONTAL 0
#define VIEW_VERTICAL 1
#define VIEW_number 2

#define MAX_WIDTH 1000
#define MAX_HEIGHT 1000

enum selectEnum { 
  select_all, select_none, select_children, select_descendants,
  select_number };

Widget treeWidget, form, setval_pop, about_pop;
static Widget toplevel, pane;
static Widget quitButton, viewButton, viewMenu, treeButton, 
  treeMenu, aboutButton;
static Widget view_widgets[VIEW_number];
static Widget select_widgets[select_number];

static Arg false_args[1] = {
  { XtNstate, (XtArgVal) FALSE }};
static Arg true_args[1] = {
  { XtNstate, (XtArgVal) TRUE }};
     
static int save_button;
     
XtAppContext app_con;

/*
 * routines
 */

     /* ARGSUSED */
static void gravity_callback (gw, closure, data)
     Widget gw;
     XtPointer closure;			/* TRUE or FALSE */
     XtPointer data;
{
  set_subtree_bitmaps((XtGravity) closure);
  set_orientation_menu ((XtGravity) closure, True);
}


/*
 * panner/porthole controls - called when the other changes
 */
/* ARGSUSED */
static void panner_callback (gw, closure, data)
     Widget gw;				/* panner widget */
     XtPointer closure;			/* porthole widget */
     XtPointer data;			/* report */
{
  XawPannerReport *rep = (XawPannerReport *) data;
  Arg args[2];
  
  if (!treeWidget) return;
  
  XtSetArg (args[0], XtNx, -rep->slider_x);
  XtSetArg (args[1], XtNy, -rep->slider_y);
  XtSetValues (treeWidget, args, TWO);	/* just assume... */
}

/* ARGSUSED */
static void porthole_callback (gw, closure, data)
     Widget gw;				/* porthole widget */
     XtPointer closure;			/* panner widget */
     XtPointer data;			/* report */
{
  Widget panner = (Widget) closure;
  XawPannerReport *rep = (XawPannerReport *) data;
  Arg args[6];
  Cardinal n = TWO;
  Dimension height;
  
  XtSetArg(args[0], XtNheight, &height);
  XtGetValues(gw, args, ONE);
  if (height > 500) {
    /**/
  }
  
  
  XtSetArg (args[0], XtNsliderX, rep->slider_x);
  XtSetArg (args[1], XtNsliderY, rep->slider_y);
  if (rep->changed != (XawPRSliderX | XawPRSliderY)) {
    XtSetArg (args[2], XtNsliderWidth, rep->slider_width);
    XtSetArg (args[3], XtNsliderHeight, rep->slider_height);
    XtSetArg (args[4], XtNcanvasWidth, rep->canvas_width);
    XtSetArg (args[5], XtNcanvasHeight, rep->canvas_height);
    n = SIX;
  }
  XtSetValues (panner, args, n);
}

/* ARGSUSED */
#ifdef __ANSI__
void toggle_callback(Widget toggle, XtPointer closure, XtPointer data) {
#else
void toggle_callback(toggle, closure, data)
  Widget toggle;
  XtPointer closure;
  XtPointer data;
{
#endif
  Arg args[5];
  Cardinal num, numn, i;
  
  Widget box = (Widget) closure;
  Widget l0, l1, w;
  WidgetList wl, wln;
  Position x;
  Dimension width;
  
  if (data) {
    XtUnmapWidget(form);

    XtSetArg(args[0], XtNchildren, &wl);
    XtSetArg(args[1], XtNnumChildren, &num);
    XtGetValues(box, args, TWO);
    
    if (save_button == 1) {   /* Zusatzinfo dazu oder weg */
      for (i=0; i<num; i++) {
	if (streq(XtName(wl[i]), NODE_INFO)) 
          if (XtIsManaged(wl[i])) {
            XtUnmanageChild(wl[i]);
          } else {
            XtManageChild(wl[i]);
          }
      }
    } else if (save_button == 2) {  /* Normal oder Abgekuerzt */
      for (; num; num--, wl++) {
	if (streq(XtName(*wl), LABEL0)) l0 = *wl;
	if (streq(XtName(*wl), LABEL1)) l1 = *wl;
      }
      if (XtIsManaged(l0)) {
	XtUnmanageChild(l0); XtManageChild(l1);
      } else if (XtIsManaged(l1)) {
	XtUnmanageChild(l1); XtManageChild(l0);
      }
    } else if (save_button == 3) {  /* Baueme (ent-)falten */
      fold(box);
      XtAppAddWorkProc(app_con, workproc, 0);
    }

    XawTreeForceLayout(treeWidget); 
    if (save_button !=3) XtMapWidget(form);  

    XtSetArg(args[0], XtNstate, FALSE);  /* Toggle zuruecksetzen */
    XtSetValues(toggle, args, ONE);
  }
}

/* ARGSUSED */
#ifdef __ANSI__
void about_callback(Widget about, XtPointer closure, XtPointer data) {
#else
void about_callback(about, closure, data)
  Widget about;
  XtPointer closure;
  XtPointer data;
{
#endif
  Arg args[5];
  Cardinal num, numn, i;
  
  PopupCentered(NULL, about_pop, XtGrabNone);
}

/* ARGSUSED */
#ifdef __ANSI__
void aboutexit_callback(Widget aboutcmd, XtPointer closure, XtPointer data) {
#else
void aboutexit_callback(aboutcmd, closure, data)
  Widget aboutcmd;
  XtPointer closure;
  XtPointer data;
{
#endif
  Arg args[5];

  XtPopdown(about_pop); 
}

/* ARGSUSED */
#ifdef __ANSI__
static void termdepth_callback(Widget sw, XtPointer closure, XtPointer data) {
#else
static void termdepth_callback(sw, closure, data)
  Widget sw;
  XtPointer closure;
  XtPointer data;
{
#endif
  printf("set term depth\n");
}

/* ARGSUSED */
#ifdef __ANSI__
static void select_callback(Widget sw, XtPointer closure, XtPointer data) {
#else
static void select_callback(sw, closure, data)
  Widget sw;
  XtPointer closure;
  XtPointer data;
{
#endif
  enum selectEnum se = (enum selectEnum) closure;
  WidgetList wl;
  Boolean *selected, done;
  Widget w;
  Cardinal n, m, num;
  Arg args[3];
  
  XUnmapWindow(XtDisplay(treeWidget), XtWindow(treeWidget));
  
  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(treeWidget, args, TWO);
  
  switch (se) {
  case select_all:
    for (n=0; n<num; n++) select_node(wl[n], TRUE);
    break;
  case select_none:
    for (n=0; n<num; n++) select_node(wl[n], FALSE);
    break;
  case select_children:
    selected = (Boolean *) XtMalloc(num * sizeof(Boolean));
    for (n=0; n< num; n++) selected[n]=isselected_node(wl[n]);
    XtSetArg(args[0], XtNtreeParent, &w);
    for (n=0; n<num; n++) if (!selected[n]) {
      XtGetValues(wl[n], args, ONE);
      for (m=0; m<num; m++) 
	if (w==wl[m] && selected[m]) select_node(wl[n], TRUE);
    }
    break;
  case select_descendants:
    selected = (Boolean *) XtMalloc(num * sizeof(Boolean));
    do {
      done = TRUE;
      for (n=0; n< num; n++) selected[n]=isselected_node(wl[n]);
      XtSetArg(args[0], XtNtreeParent, &w);
      for (n=0; n<num; n++) if (!selected[n]) {
	XtGetValues(wl[n], args, ONE);
	for (m=0; m<num; m++) 
	  if (w==wl[m] && selected[m]) {
	    select_node(wl[n], TRUE);
	    done = FALSE;
	  }
      }
    } while (!done);
    break;
    
  }
  
  XawTreeForceLayout(treeWidget);
  XMapWindow(XtDisplay(treeWidget), XtWindow(treeWidget));
  
}

#ifdef __ANSI__
static void select_node(Widget w, Boolean b) {
#else
static void select_node(w, b)
  Widget w;
  Boolean b;
{
#endif
  Arg args[3];
  Cardinal n, num;
  WidgetList wl;

  if (streq("root", XtName(w))) return;
  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(w, args, 2);
  
  for (n=0; n<num; n++)
    if (streq(XtName(wl[n]), NODE_INFO))
      if (b) {
	XtManageChild(wl[n]);
      } else {
	XtUnmanageChild(wl[n]);
      }
}

#ifdef __ANSI__
static Boolean isselected_node(Widget w) {
#else
static Boolean isselected_node(w)
  Widget w;
{
#endif
  Arg args[3];
  Cardinal n, num;
  WidgetList wl;
  
  XtSetArg(args[0], XtNchildren, &wl);
  XtSetArg(args[1], XtNnumChildren, &num);
  XtGetValues(w, args, 2);
  
  for (n=0; n<num; n++)
    if (!XtIsSubclass(wl[n], toggleWidgetClass) && XtIsManaged(wl[n]))
      return (TRUE);
  return (FALSE);
}

#ifdef __ANSI__
void insert_node1(struct label_str lab, char *info, direct d) {
#else
void insert_node1(lab, info, d)
  struct label_str lab;
  char *info;
  direct d;
{
#endif
  static Widget father[1000];
  static int stackpointer=0;
  char *label0, *label1;
  Widget box, w;
  Arg args[3];			/* need to set super node */
  Cardinal n;				/* count of args */
  static XtCallbackRec callback_rec[2] = {{ toggle_callback, NULL },
					    { NULL, NULL }};
  /*  fathers[0]=NULL; implizit gesetzt, da fathers static */
  if (d==up) {
    stackpointer--;
    return; }

  /* printf("i_n1: %s -- %s \n", lab.exp, lab.abbr); */
  label0 = (tdmax > 0) ? lab.abbr : lab.exp; 
  label1 = lab.exp;

  n = 0;
  XtSetArg(args[n], XtNtreeParent, father[stackpointer]); n++;
  box = XtCreateManagedWidget(lab.exp, boxWidgetClass, treeWidget, args, n);
  
  n = 0;
  XtSetArg(args[n], XtNlabel, label0); n++; 
  callback_rec[0].closure= (XtPointer) box;
  XtSetArg(args[n], XtNcallback, callback_rec); n++;
  w=XtCreateManagedWidget(LABEL0, toggleWidgetClass, box, args, n);

  n = 0;
  XtSetArg(args[n], XtNlabel, label1); n++; 
  callback_rec[0].closure= (XtPointer) box;
  XtSetArg(args[n], XtNcallback, callback_rec); n++;
  XtCreateWidget(LABEL1, toggleWidgetClass, box, args, n);

  n = 0;
  XtSetArg(args[n], XtNlabel, info); n++; 
  XtCreateWidget(NODE_INFO, labelWidgetClass, box, args, n);
  
  if (d==down) {
    if (++stackpointer>=1000) {
      fprintf(stderr, "ERROR: stack overflow"); 
      exit(-1);
    } else {
      father[stackpointer]=box;
    }
  }
}


static void set_orientation_menu (grav, dosetvalues)
     XtGravity grav;
     Boolean dosetvalues;
{
#define CHOOSE(val) (sensitiveargs + (grav != (val)))
  XtSetValues (view_widgets[VIEW_HORIZONTAL], CHOOSE(WestGravity), ONE);
  XtSetValues (view_widgets[VIEW_VERTICAL], CHOOSE(NorthGravity), ONE);
#undef CHOOSE
  
  if (dosetvalues) {
    Arg args[1];
    

    XtSetArg (args[0], XtNgravity, grav);

    XUnmapWindow (XtDisplay(treeWidget), XtWindow(treeWidget)); 
    XtSetValues (treeWidget, args, ONE); 
    XMapWindow (XtDisplay(treeWidget), XtWindow(treeWidget));
    
  }
}

#ifdef __ANSI__
static void create_td_popup(Widget w) {
#else
static void create_td_popup(w)
  Widget w;
{
#endif
  Widget form, td_label;
  Arg args[5];

/*  setval_pop = XtCreatePopupShell("setvalPopup", transientShellWidgetClass, 
               w, NULL, ZERO);
  form = XtCreateManagedWidget("form", formWidgetClass, 
				 setval_popup, NULL, ZERO);
  td_label = XtCreateManagedWidget("label", labelWidgetClass,
				  form, args, 1);
  XtSetArg(args[0], XtNfromVert, td_label);
*//*  XtCreateManagedWidget("termdepth", asciiTextWidgetClass,
						  form, args, 1);
*/
}


/*****************************************************************************
 *****************************************************************************/

main (argc, argv)
     int argc;
     char **argv;
{
  Widget box, porthole, panner, tmp;
  Arg args[6];
  Dimension canvasWidth, canvasHeight, sliderWidth, sliderHeight;
  Dimension tlwidth, tlheight;
  static XtCallbackRec callback_rec[2] = {{ NULL, NULL }, { NULL, NULL }};
  XtGravity grav;
  int i, j;
  char title[255], *b, filename[255];
  FILE *file;
  char *convfile = NULL;

  sprintf(title, "xptree");
  if (argc>1 && *argv[argc-1] != '-') {  /* filename angegeben */
    strcpy(filename, argv[argc-1]);
    if (strcmp(filename+(strlen(filename)-5), ".tree")) 
      strcpy(filename+strlen(filename), ".tree"); 
    file = fopen(filename, "r");
    if (!file) {
      fprintf(stderr, "xptree: cannot open file '%s'\n", filename);
      exit(1);
    }
    for (b=filename, j=0; *b; b++, j++);
    for (; j && *b != '/'; b--, j--);
    if (*b=='/') b++;
    sprintf(title, "xptree: %s", b);

  } else { file = NULL; }

  tdmax = 10000;  /* maximale Termtiefe = unendlich (sonst 1, 2, ...) */
  symbs = 1;  /* neue Symbole fuer Variablen einfuehren */
  for (i=1; i < argc && *argv[i]=='-'; i++) {
    if (!strcmp(argv[i], "-td") && i+1 < argc) {/* maximal Termtiefe setzten */
	tdmax=atoi(argv[i+1]); i++;
      continue;
    }
    if (!strcmp(argv[i], "-origvars")) {  /* keine neuen Var-namen */
      symbs = 0;
    }
    if (!strcmp(argv[i], "-ctab") && i+1 <argc) { 
      convfile = argv[i+1]; i++;
    }
  }

  init_conversion_table();
  if (convfile) read_conversion_table(convfile);

  XtSetArg (args[0], XtNtitle, title);
  toplevel = XtAppInitialize (&app_con, "xptreeShell", 
			      Options, XtNumber (Options),
			      &argc, argv, fallback_resources, 
			      args, ONE);
  
  XtAppAddActions (app_con, xptree_actions, XtNumber (xptree_actions));
  XtOverrideTranslations
    (toplevel, XtParseTranslationTable ("<Message>WM_PROTOCOLS: Quit()"));
  
  pane = XtCreateManagedWidget ("pane", panedWidgetClass, toplevel,
				NULL, 0);
  
  box = XtCreateManagedWidget ("buttonbox", boxWidgetClass, pane,
			       (ArgList) NULL, ZERO);
  quitButton = XtCreateManagedWidget ("quit", commandWidgetClass, box,
				      (ArgList) NULL, ZERO);
  
  /*
   * Format menu
   */
  XtSetArg (args[0], XtNmenuName, "viewMenu");
  viewButton = XtCreateManagedWidget ("view", menuButtonWidgetClass, box,
				      args, ONE);
  viewMenu = XtCreatePopupShell ("viewMenu", simpleMenuWidgetClass, 
				 viewButton, (ArgList) NULL, ZERO);
  XtSetArg (args[0], XtNcallback, callback_rec);
#define MAKE_VIEW(n,v,name) \
  callback_rec[0].closure = (XtPointer) v; \
    view_widgets[n] = XtCreateManagedWidget (name, smeBSBObjectClass, \
					     viewMenu, args, ONE)
      callback_rec[0].callback = (XtCallbackProc) gravity_callback;
  MAKE_VIEW (VIEW_HORIZONTAL, WestGravity, "layoutHorizontal");
  MAKE_VIEW (VIEW_VERTICAL, NorthGravity, "layoutVertical");
#undef MAKE_VIEW
  
  
  XtSetArg (args[0], XtNmenuName, "treeMenu");
  treeButton = XtCreateManagedWidget ("tree", menuButtonWidgetClass, box,
					args, ONE);
  treeMenu = XtCreatePopupShell ("treeMenu", simpleMenuWidgetClass, 
				   treeButton, (ArgList) NULL, ZERO);
  XtSetArg (args[0], XtNcallback, callback_rec);
  callback_rec[0].callback = (XtCallbackProc) select_callback;
#define MAKE_SELECT(v,name) \
  callback_rec[0].closure = (XtPointer) v; \
    select_widgets[v] = XtCreateManagedWidget (name, smeBSBObjectClass, \
					       treeMenu, args, ONE)
  MAKE_SELECT (select_all, "selectAll");
  MAKE_SELECT (select_none, "selectNone");
  MAKE_SELECT (select_children, "selectChildren");
  MAKE_SELECT (select_descendants, "selectDescendants");
#undef MAKE_SELECT
  XtCreateManagedWidget("line", smeLineObjectClass, treeMenu, NULL, 0);
  callback_rec[0].callback = (XtCallbackProc) termdepth_callback;
  tmp = XtCreateManagedWidget("changeTermdepth", smeBSBObjectClass,
					       treeMenu, args, ONE);
  create_td_popup(tmp);

  callback_rec[0].closure = NULL;
  callback_rec[0].callback = about_callback;
  XtSetArg (args[0], XtNcallback, callback_rec);
  aboutButton = XtCreateManagedWidget ("about", commandWidgetClass, box,
				      args, 1);

  about_pop = XtCreatePopupShell("aboutPopup", transientShellWidgetClass, 
              aboutButton, NULL, ZERO);

  callback_rec[0].closure = (XtPointer) about_pop;
  callback_rec[0].callback = aboutexit_callback;
  XtSetArg (args[0], XtNcallback, callback_rec);
  XtSetArg (args[1], XtNlabel, VERSION);
  XtCreateManagedWidget ("aboutexit", commandWidgetClass, about_pop, args, 2);


  XtSetArg (args[0], XtNshowGrip, False);
  form = XtCreateManagedWidget ("treeform", formWidgetClass, pane,
				args, 1);
  /*
   * create the panner and the porthole and then connect them with the
   * callbacks (passing the other widget each callback)
   */
  XtSetArg (args[0], XtNbackgroundPixmap, None);  /* faster updates */
  porthole = XtCreateManagedWidget ("porthole", portholeWidgetClass, form,
				    args, ONE);
  panner = XtCreateManagedWidget ("panner", pannerWidgetClass, form,
				  (ArgList) NULL, ZERO);
  
  callback_rec[0].callback = (XtCallbackProc) panner_callback;
  callback_rec[0].closure = (XtPointer) porthole;
  XtSetArg (args[0], XtNreportCallback, callback_rec);
  XtSetValues (panner, args, ONE);
  
  callback_rec[0].callback = (XtCallbackProc) porthole_callback;
  callback_rec[0].closure = (XtPointer) panner;
  XtSetArg (args[0], XtNreportCallback, callback_rec);
  XtSetValues (porthole, args, ONE);
  
  /*
   * now that the panner and porthole are set up, insert the tree and 
   * fix up the menu, fill in the nodes
   */
/*  box = XtCreateManagedWidget ("placeholder", boxWidgetClass, porthole,
			       (ArgList) NULL, ZERO);
 */ /* Diese Box ist nur dazu da, einen Rand um den Baum zu erhalten.
     (wird in xptree.ad gesetzt) */
  XtSetArg (args[0], XtNautoReconfigure, TRUE);  
  treeWidget = XtCreateManagedWidget ("tree", treeWidgetClass,
				      porthole, args, ONE);
  
  XtSetArg (args[0], XtNgravity, &grav);
  XtGetValues (treeWidget, args, ONE);
  set_orientation_menu (grav, FALSE);
 
  /* EINGABE */
  if (file) yyin=file;
  yyparse();

  XtSetArg(args[0], XtNmappedWhenManaged, FALSE);
  XtSetValues(toplevel, args, ONE);
  
  /*
   * Realize the tree, but do not map it (we set mappedWhenManaged to 
   * false up above).  Get the initial size of the tree so that we can
   * size the panner appropriately.
   */
  XtRealizeWidget (toplevel);
  
  XtSetArg(args[0], XtNheight, &tlheight);
  XtSetArg(args[1], XtNwidth, &tlwidth);
  XtGetValues(toplevel, args, TWO);
  
  if (tlwidth > MAX_WIDTH) tlwidth=MAX_WIDTH;
  if (tlheight > MAX_HEIGHT) tlheight=MAX_HEIGHT;
  
  XtSetArg(args[0], XtNheight, tlheight);
  XtSetArg(args[1], XtNwidth, tlwidth);
  XtSetArg(args[2], XtNmappedWhenManaged, TRUE);
  XtSetValues(toplevel, args, THREE);
  
  wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				 False);
  (void) XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
			  &wm_delete_window, 1);
  
  XtSetArg (args[0], XtNwidth, &canvasWidth);
  XtSetArg (args[1], XtNheight, &canvasHeight);
  XtGetValues (treeWidget, args, TWO);
  
  XtSetArg (args[0], XtNwidth, &sliderWidth);
  XtSetArg (args[1], XtNheight, &sliderHeight);
  XtGetValues (porthole, args, TWO);
  
  XtSetArg (args[0], XtNcanvasWidth, canvasWidth);
  XtSetArg (args[1], XtNcanvasHeight, canvasHeight);
  XtSetArg (args[2], XtNsliderWidth, sliderWidth);
  XtSetArg (args[3], XtNsliderHeight, sliderHeight);
  XtSetValues (panner, args, FOUR);
  
  XRaiseWindow (XtDisplay(panner), XtWindow(panner));
  XtMapWidget(toplevel); 
/*  XtRealizeWidget(toplevel); */
  XtAppAddWorkProc(app_con, workproc, 0);
  XtAppMainLoop(app_con);
}

#ifdef __ANSI__
Boolean workproc(XtPointer dummy) {
#else
Boolean workproc(dummy)
  XtPointer dummy;
{
#endif
    XtMapWidget(form);
    return TRUE;  /* worcproc wieder entfernen */
}


/*****************************************************************************
 *                                                                           *
 *		   xptree translation table action routines                 *
 *                                                                           *
 *****************************************************************************/

/* ARGSUSED */
static void ActionQuit (w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  exit (0);
}

/* ARGSUSED */
static void ActionSaveButton (w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  int i;

  /* Mousebutton merken, damit der toggle_callback weiss, was er tun soll */
  
  if (*num_params==1 && **params >= '0' && **params <= '9')
    save_button = **params - '0';
}

/* ARGSUSED */
static void ActionSetOrientation (w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XtGravity newgrav = ForgetGravity;
  
  if (*num_params < 1) {
    Arg arg;
    XtGravity oldgrav = ForgetGravity;
    
    XtSetArg (arg, XtNgravity, &oldgrav);
    XtGetValues (treeWidget, &arg, ONE);
    switch (oldgrav) {
    case WestGravity:  newgrav = NorthGravity; break;
    case NorthGravity:  newgrav = WestGravity; break;
    case EastGravity:  newgrav = SouthGravity; break;
    case SouthGravity:  newgrav = EastGravity; break;
    default:
      return;
    }
  } else {
    XrmValue fromval, toval;
    
    fromval.size = sizeof (String);
    fromval.addr = (XPointer) params[0];
    toval.size = sizeof (XtGravity);
    toval.addr = (XPointer) &newgrav;
    XtConvertAndStore (treeWidget, XtRString, &fromval,
		       XtRGravity, &toval);
  }
  
  switch (newgrav) {
  case WestGravity: case NorthGravity: case EastGravity: case SouthGravity:
    break;
  default:
    XBell (XtDisplay(w), 0);
    return;
  }
  
  set_orientation_menu (newgrav, TRUE);
  return;
}


