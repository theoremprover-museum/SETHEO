/*
 * $XConsortium: Tree.c,v 1.42 91/02/20 20:06:07 converse Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 * Copyright 1989 Prentice Hall
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation.
 * 
 * M.I.T., Prentice Hall and the authors disclaim all warranties with regard
 * to this software, including all implied warranties of merchantability and
 * fitness.  In no event shall M.I.T., Prentice Hall or the authors be liable
 * for any special, indirect or cosequential damages or any damages whatsoever
 * resulting from loss of use, data or profits, whether in an action of
 * contract, negligence or other tortious action, arising out of or in
 * connection with the use or performance of this software.
 * 
 * Authors:  Jim Fulton, MIT X Consortium,
 *           based on a version by Douglas Young, Prentice Hall
 * 
 * This widget is based on the Tree widget described on pages 397-419 of
 * Douglas Young's book "The X Window System, Programming and Applications 
 * with Xt OSF/Motif Edition."  The layout code has been rewritten to use
 * additional blank space to make the structure of the graph easier to see
 * as well as to support vertical trees.
 */

/* Modified    Aug 92 by Roland Handl (new layout algorithm) */
/*          30 Dec 92    Roland Handl (overlapping bug removed) */
/*          04 Jan 93    Roland Handl (delete_node corrected) */
/*          15 Jul 93    Roland Handl (ANSI->K&R C Conversion) */

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ConstrainP.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/Cardinals.h>
#include "TreeP.h"
#include <stdio.h>

#define IsHorizontal(tw) ((tw)->tree.gravity == WestGravity || \
			  (tw)->tree.gravity == EastGravity)

typedef enum { left, right } direct;

/* widget class method */
static void             ClassInitialize();
static void             Initialize();
static void             ConstraintInitialize();
static void             ConstraintDestroy();
static Boolean          ConstraintSetValues();
static void             Destroy();
static Boolean          SetValues();
static XtGeometryResult GeometryManager();
static void             ChangeManaged();
static void             Redisplay();
static XtGeometryResult	QueryGeometry();

/* utility routines */
static void             insert_node();
static void             delete_node();
#ifdef __ANSI__
static void layout_tree (TreeWidget tw, Boolean insetvalues);
#else
static void layout_tree ();
#endif

#ifdef __ANSI__
void beautify(TreeWidget tw);
#else
void beautify();
#endif
#ifdef __ANSI__
void push_layers(Widget n, Position diff);
#else
void push_layers();
#endif
#ifdef __ANSI__
void shift_tree(Widget n, Position diff);
#else
void shift_tree();
#endif
#ifdef __ANSI__
Boolean siblings(Widget n);
#else
Boolean siblings();
#endif
#ifdef __ANSI__
Position meanpos(Widget n);
#else
Position meanpos();
#endif
#ifdef __ANSI__
Position dists(Widget n, direct dir, Position mindiff);
#else
Position dists();
#endif
#ifdef __ANSI__
void dist_right(Widget n, Layer l);
#else
void dist_right();
#endif
#ifdef __ANSI__
void balance_l(TreeWidget tw, Widget nl, Widget nr, int k);
#else
void balance_l();
#endif
#ifdef __ANSI__
void balance_r(TreeWidget tw, Widget nl, Widget nr, int k);
#else
void balance_r();
#endif


/*
 * resources of the tree itself
 */
static XtResource resources[] = {
  { XtNautoReconfigure, XtCAutoReconfigure, XtRBoolean, sizeof (Boolean),
      XtOffsetOf(TreeRec, tree.auto_reconfigure), XtRImmediate,
      (XtPointer) FALSE },
  { XtNhSpace, XtCHSpace, XtRDimension, sizeof (Dimension),
      XtOffsetOf(TreeRec, tree.hpad), XtRImmediate, (XtPointer) 0 },
  { XtNvSpace, XtCVSpace, XtRDimension, sizeof (Dimension),
      XtOffsetOf(TreeRec, tree.vpad), XtRImmediate, (XtPointer) 0 },
  { XtNforeground, XtCForeground, XtRPixel, sizeof (Pixel),
      XtOffsetOf(TreeRec, tree.foreground), XtRString,
      XtDefaultForeground},
  { XtNlineWidth, XtCLineWidth, XtRDimension, sizeof (Dimension),
      XtOffsetOf(TreeRec, tree.line_width), XtRImmediate, (XtPointer) 0 },
  { XtNgravity, XtCGravity, XtRGravity, sizeof (XtGravity),
      XtOffsetOf(TreeRec, tree.gravity), XtRImmediate,
      (XtPointer) WestGravity },
};


/*
 * resources that are attached to all children of the tree
 */
static XtResource treeConstraintResources[] = {
  { XtNtreeParent, XtCTreeParent, XtRWidget, sizeof (Widget),
      XtOffsetOf(TreeConstraintsRec, tree.parent), XtRImmediate, NULL },
  { XtNtreeGC, XtCTreeGC, XtRGC, sizeof(GC),
      XtOffsetOf(TreeConstraintsRec, tree.gc), XtRImmediate, NULL },
};


TreeClassRec treeClassRec = {
  {
    /* core_class fields  */
    (WidgetClass) &constraintClassRec,	/* superclass         */
    "Tree",				/* class_name         */
    sizeof(TreeRec),			/* widget_size        */
    ClassInitialize,			/* class_init         */
    NULL,				/* class_part_init    */
    FALSE,				/* class_inited       */	
    Initialize,				/* initialize         */
    NULL,				/* initialize_hook    */	
    XtInheritRealize,			/* realize            */
    NULL,				/* actions            */
    0,					/* num_actions        */	
    resources,				/* resources          */
    XtNumber(resources),		/* num_resources      */
    NULLQUARK,				/* xrm_class          */
    TRUE,				/* compress_motion    */	
    TRUE,				/* compress_exposure  */	
    TRUE,				/* compress_enterleave*/	
    TRUE,				/* visible_interest   */
    Destroy,				/* destroy            */
    NULL,				/* resize             */
    Redisplay,				/* expose             */
    SetValues,				/* set_values         */
    NULL,				/* set_values_hook    */	
    XtInheritSetValuesAlmost,		/* set_values_almost  */
    NULL,				/* get_values_hook    */	
    NULL,				/* accept_focus       */
    XtVersion,				/* version            */	
    NULL,				/* callback_private   */
    NULL,				/* tm_table           */
    QueryGeometry,			/* query_geometry     */	
    NULL,				/* display_accelerator*/
    NULL,				/* extension          */
  },
  {
    /* composite_class fields */
    GeometryManager,			/* geometry_manager    */
    ChangeManaged,			/* change_managed      */
    XtInheritInsertChild,		/* insert_child        */	
    XtInheritDeleteChild,		/* delete_child        */	
    NULL,				/* extension           */
  },
  { 
    /* constraint_class fields */
    treeConstraintResources,		/* subresources        */
    XtNumber(treeConstraintResources),	/* subresource_count   */
    sizeof(TreeConstraintsRec),		/* constraint_size     */
    ConstraintInitialize,		/* initialize          */
    ConstraintDestroy,			/* destroy             */
    ConstraintSetValues,			/* set_values          */
    NULL,				/* extension           */
  },
  {
    /* Tree class fields */
    0,					/* ignore              */	
  }
};

WidgetClass treeWidgetClass = (WidgetClass) &treeClassRec;


/*****************************************************************************
 *                                                                           *
 *			     tree utility routines                           *
 *                                                                           *
 *****************************************************************************/

static GC get_tree_gc (w)
     TreeWidget w;
{
  XtGCMask valuemask = GCBackground | GCForeground;
  XGCValues values;
  
  values.background = w->core.background_pixel;
  values.foreground = w->tree.foreground;
  if (w->tree.line_width != 0) {
    valuemask |= GCLineWidth;
    values.line_width = w->tree.line_width;
  }
  
  return XtGetGC ((Widget) w, valuemask, &values);
}

/* connect_tree baut rekursiv alle restlichen, notwendigen ver-pointer-ungen
   auf (noetig nach insert, delete, Unterbaum einhaengen mit 
   ConstraintSetValues */
/* connect_tree wird in folgendem Kontext aufgerufen :
    if (!tw->tree.connected) {
      for (l=tw->tree.top_layer->next; l; l=l->next) 
        l->first = NULL;
      connect_tree(tw->tree.top_layer->first, tw->tree.top_layer); 
      tw->tree.connected = TRUE;
    }
*/
#ifdef __ANSI__
static void connect_tree(Widget n, Layer layer) {
#else
static void connect_tree(n, layer)
  Widget n;
  Layer layer;
{
#endif
  Widget m;
  TreeConstraintsPart *nt, *mt;
  Layer child_layer;

  nt = &(T_C(n)->tree);

  nt->layer = layer;
  if (!layer->first) {
    layer->first = n;
    nt->prev = NULL;
  } else if (nt->parent) {
    m = layer->last;
    mt = &(T_C(m)->tree);
    mt->next = n; nt->prev = m;
  }
  layer->last = n;
  
  if (nt->first_child) {  /* falls Knoten n Kinder hat */
    if (!layer->next) { /* neue Schicht nach unten */
      child_layer = (Layer) XtMalloc(sizeof(struct _Layer));
      child_layer->prev = layer;
      layer->next = child_layer;
      child_layer->next = NULL;
      child_layer->first = NULL;
    } else {  /* schon Schicht vorhanden */
      child_layer = layer->next;
    }
    for (m = nt->first_child; m && siblings(m); m = mt->next) {
      mt = &(T_C(m)->tree);
      connect_tree(m, child_layer);
    }
    if (m) {
      mt = &(T_C(m)->tree);
      mt->next=NULL;
      connect_tree(m, child_layer);
    }
  }
}

#ifdef __ANSI__
static void insert_node(Widget parent, Widget node) {
#else
static void insert_node(parent, node)
  Widget parent;
  Widget node;
{
#endif
  TreeConstraintsPart *nt, *pt, *mt;
  TreeWidget tw;
  Widget m;
  
  tw = (TreeWidget) node->core.parent;
  nt = &(T_C(node)->tree);
  
  nt->parent = parent;
  if (!parent) {  /* -> root */
    tw->tree.top_layer->first = tw->tree.top_layer->last = node;
    nt->prev = nt->next = NULL;
  } else {
    pt = &(T_C(parent)->tree);
    if (pt->first_child) {  /* schon Geschwister vorhanden */
      m = pt->last_child;
      mt = &(T_C(m)->tree);
      nt->prev = m; mt->next = node;
    } else { /* erstes Kind */
      pt->first_child = node;
    }
    pt->last_child = node;
  }
  tw->tree.connected = FALSE;
}

static void delete_node (parent, node)
     Widget parent, node;
{
  TreeConstraintsPart *nt, *pt, *mt, *n1, *n2;
  TreeWidget tw;
  Widget m;

#ifdef DEBUG
  printf("deletenode\n"); 
#endif
  tw = (TreeWidget) node->core.parent;
  nt = &(T_C(node)->tree);
  
  /* Make sure the parent exists. */
  if (!parent) return;  
  
  pt = &(T_C(parent)->tree);
  /* is node a child of parent ? */
  for (m = pt->first_child; m && siblings(m); m = mt->next) {
    mt = &(T_C(m)->tree);
    if (m==node) break;
  }
  if (m!=node) return;

  /* erst mal Knoten node aus Knoten parent aushaengen ... */
  if (pt->first_child == pt->last_child) { /* is lonely */
    pt->first_child = pt->last_child = NULL;
  } else {
    if (node == pt->first_child) pt->first_child = nt->next;
    if (node == pt->last_child) pt->last_child = nt->prev;
    if (nt->prev) (T_C(nt->prev)->tree).next = nt->next;
    if (nt->next) (T_C(nt->next)->tree).prev = nt->prev;
  }
  /* ... und dann dessen Kinder in seine Vater einhaengen (hintendran) */
  if (nt->first_child) {
    if (pt->first_child) {
      n1 = &(T_C(pt->last_child)->tree);
      n1->next = nt->first_child;
      mt = &(T_C(nt->first_child)->tree);
      mt->prev = pt->last_child;
      pt->last_child=nt->last_child;
    } else {
      pt->first_child = nt->first_child;
      pt->last_child = nt->last_child;
    }
    for (m=nt->first_child; siblings(m); m = mt->next) {
      mt=&(T_C(m)->tree); mt->parent = parent; }
    mt=&(T_C(m)->tree); mt->parent = parent;
    
  }
  tw->tree.connected = FALSE;
}

/* test proc */
#ifdef __ANSI__
print_tree(TreeWidget tw) {
#else
print_tree(tw)
  TreeWidget tw;
{
#endif
  TreeConstraintsPart *wt;
  Layer l;
  Widget w;
  int i;

  for (l=tw->tree.top_layer, i=0; l; l=l->next, i++) {
    printf("%d: ", i);
    for (w=l->first; w; w = wt->next) {
      wt = &(T_C(w)->tree);
      printf("'%s'[%s, %s] ", XtName(w), 
	     T_C(w)->tree.first_child ? 
	     XtName(T_C(w)->tree.first_child) : "NULL",
	     T_C(w)->tree.last_child ? 
	     XtName(T_C(w)->tree.last_child) : "NULL");
      if (wt->prev) printf("(%s) ", XtName(wt->prev));
    }
    printf("\n");
  }
  printf("***\n");
}

static void check_gravity (tw, grav)
     TreeWidget tw;
     XtGravity grav;
{
  switch (tw->tree.gravity) {
  case WestGravity: case NorthGravity: case EastGravity: case SouthGravity:
    break;
  default:
    tw->tree.gravity = grav;
    break;
  }
}


/*****************************************************************************
 *                                                                           *
 * 			      tree class methods                             *
 *                                                                           *
 *****************************************************************************/

static void ClassInitialize ()
{
  XawInitializeWidgetSet();
  XtAddConverter (XtRString, XtRGravity, XmuCvtStringToGravity,
		  (XtConvertArgList) NULL, (Cardinal) 0);
}


static void Initialize (grequest, gnew)
     Widget grequest, gnew;
{
  TreeWidget request = (TreeWidget) grequest, new = (TreeWidget) gnew;
  Arg args[2];
  Layer top_lay;
  
  /*
   * Make sure the widget's width and height are 
   * greater than zero.
   */
  if (request->core.width <= 0) new->core.width = 5;
  if (request->core.height <= 0) new->core.height = 5;
  
  /*
   * Set the padding according to the orientation
   */
  if (request->tree.hpad == 0 && request->tree.vpad == 0) {
    if (IsHorizontal (request)) {
      new->tree.hpad = TREE_HORIZONTAL_DEFAULT_SPACING;
      new->tree.vpad = TREE_VERTICAL_DEFAULT_SPACING;
    } else {
      new->tree.hpad = TREE_VERTICAL_DEFAULT_SPACING;
      new->tree.vpad = TREE_HORIZONTAL_DEFAULT_SPACING;
    }
  }
  
  /*
   * Create a graphics context for the connecting lines.
   */
  new->tree.gc = get_tree_gc (new);
  
  top_lay = (Layer) XtMalloc(sizeof(struct _Layer));
  top_lay->prev = top_lay->next = NULL;
  top_lay->first = top_lay->last = NULL;
  new->tree.top_layer = top_lay;
  
  new->tree.leastdist=0.0; /**/
  new->tree.relnb=3.0; /**/
  
  /*
   * make sure that our gravity is one of the acceptable values
   */
  check_gravity (new, WestGravity);
} 


/* ARGSUSED */
static void ConstraintInitialize (request, new)
     Widget request, new;
{
  TreeConstraints tc = T_C(new);
  TreeWidget tw = (TreeWidget) new->core.parent;
  
  /*
   * Initialize the widget to have no sub-nodes.
   */
  tc->tree.first_child = tc->tree.last_child = NULL;
  
  insert_node (tc->tree.parent, new);
} 


/* ARGSUSED */
static Boolean SetValues (gcurrent, grequest, gnew)
     Widget gcurrent, grequest, gnew;
{
  TreeWidget current = (TreeWidget) gcurrent, new = (TreeWidget) gnew;
  Boolean redraw = FALSE;
  
  /*
   * If the foreground color has changed, redo the GC's
   * and indicate a redraw.
   */
  if (new->tree.foreground != current->tree.foreground ||
      new->core.background_pixel != current->core.background_pixel ||
      new->tree.line_width != current->tree.line_width) {
    XtReleaseGC (gnew, new->tree.gc);
    new->tree.gc = get_tree_gc (new);
    redraw = TRUE;     
  }
  
  /*
   * If the minimum spacing has changed, recalculate the
   * tree layout. layout_tree() does a redraw, so we don't
   * need SetValues to do another one.
   */
  if (new->tree.gravity != current->tree.gravity) {
    check_gravity (new, current->tree.gravity);
  }
  
  if (IsHorizontal(new) != IsHorizontal(current)) {
    if (new->tree.vpad == current->tree.vpad &&
	new->tree.hpad == current->tree.hpad) {
      new->tree.vpad = current->tree.hpad;
      new->tree.hpad = current->tree.vpad;
    }
  }
  
  if (new->tree.vpad != current->tree.vpad ||
      new->tree.hpad != current->tree.hpad ||
      new->tree.gravity != current->tree.gravity) {
    layout_tree (new, TRUE);
    redraw = FALSE;
  }
  return redraw;
}


/* ARGSUSED */
static Boolean ConstraintSetValues (current, request, new, args, num_args)
     Widget current, request, new;
     ArgList args;
     Cardinal *num_args;
{
  TreeConstraints newc = T_C(new);
  TreeConstraints curc = T_C(current);
  TreeWidget tw = (TreeWidget) new->core.parent;
  
  /*
   * If the parent field has changed, remove the widget
   * from the old widget's children list and add it to the
   * new one.
   */
  if (curc->tree.parent != newc->tree.parent){
    if (curc->tree.parent)
      delete_node (curc->tree.parent, new);
    if (newc->tree.parent)
      insert_node(newc->tree.parent, new);
    
    /*
     * If the Tree widget has been realized, 
     * compute new layout.
     */
    if (XtIsRealized((Widget)tw))
      layout_tree (tw, FALSE);
  }
  return False;
}


static void ConstraintDestroy (w) 
     Widget w;
{ 
  TreeConstraints tc = T_C(w);
  TreeWidget tw = (TreeWidget) XtParent(w);
  int i;
  
#ifdef DEBUG
  printf("constraintdestroy\n");
#endif

  /* 
   * Remove the widget from its parent's sub-nodes list and
   * make all this widget's sub-nodes sub-nodes of the parent.
   */
  
  delete_node (tc->tree.parent, (Widget) w);
  /***/
  layout_tree ((TreeWidget) (w->core.parent), FALSE);
}

/* ARGSUSED */
static XtGeometryResult GeometryManager (w, request, reply)
     Widget w;
     XtWidgetGeometry *request;
     XtWidgetGeometry *reply;
{
  
  TreeWidget tw = (TreeWidget) w->core.parent;
  
  /*
   * No position changes allowed!.
   */
  if ((request->request_mode & CWX && request->x!=w->core.x)
      ||(request->request_mode & CWY && request->y!=w->core.y))
    return (XtGeometryNo);
  
  /*
   * Allow all resize requests.
   */
  
  if (request->request_mode & CWWidth)
    w->core.width = request->width;
  if (request->request_mode & CWHeight)
    w->core.height = request->height;
  if (request->request_mode & CWBorderWidth)
    w->core.border_width = request->border_width;
  
  if (tw->tree.auto_reconfigure) layout_tree (tw, FALSE);
  return (XtGeometryYes);
}

static void ChangeManaged (gw)
     Widget gw;
{
  layout_tree ((TreeWidget) gw, FALSE);
}


static void Destroy (gw)
     Widget gw;
{
  TreeWidget w = (TreeWidget) gw;
  
  XtReleaseGC (gw, w->tree.gc);
}


/* ARGSUSED */
static void Redisplay (tw, event, region)
     TreeWidget tw;
     XEvent *event;
     Region region;
{
  
  /*
   * If the Tree widget is visible, visit each managed child.
   */
  if (tw->core.visible) {
    int i, j;
    Display *dpy = XtDisplay (tw);
    Window w = XtWindow (tw);
    
    Layer l; 
    register Widget n, cn;
    register TreeConstraintsPart *nt, *cnt;
    int srcx, srcy;
    GC gc;
    Boolean was_last;
    
    for (l=tw->tree.top_layer; l; l=l->next) {
      for (n=l->first; n; n=nt->next) {
	nt = &(T_C(n)->tree);
	
	srcx = n->core.x + n->core.border_width;
	srcy = n->core.y + n->core.border_width;
	
	switch (tw->tree.gravity) {
	case WestGravity:
	  srcx += n->core.width + n->core.border_width;
	  /* fall through */
	case EastGravity:
	  srcy += n->core.height / 2;
	  break;
	  
	case NorthGravity:
	  srcy += n->core.height + n->core.border_width;
	  /* fall through */
	case SouthGravity:
	  srcx += n->core.width / 2;
	  break;
	}
	
	
	was_last = 0;
	for (cn=nt->first_child; cn && !was_last; cn=cnt->next) {
	  was_last = !siblings(cn);
	  cnt = &(T_C(cn)->tree);
	  
	  gc = (nt->gc ? nt->gc : tw->tree.gc);
	  
	  switch (tw->tree.gravity) {
	  case WestGravity:
	    /*
	     * right center to left center
	     */
	    XDrawLine (dpy, w, gc, srcx, srcy,
		       (int) cn->core.x - tw->tree.hpad, srcy);
	    XDrawLine (dpy, w, gc, 
		       (int) cn->core.x - tw->tree.hpad, srcy,
		       (int) cn->core.x,
		       (cn->core.y + ((int) cn->core.border_width) +
			((int) cn->core.height) / 2));
	    break;
	    
	  case NorthGravity:
	    /*
	     * bottom center to top center
	     */
	    XDrawLine (dpy, w, gc, srcx, srcy,
		       srcx, (int) cn->core.y - tw->tree.vpad);
	    XDrawLine (dpy, w, gc, srcx, (int) cn->core.y - tw->tree.vpad,
	               cn->core.x + cn->core.border_width +
		       cn->core.width/2,
		       (int) cn->core.y);
	    /* printf("%d, %d, %s -  %d, %d, %s\n", srcx, nt->x, XtName(n), 	
	       cn->core.x + cn->core.border_width + cn->core.width/2,
	       cnt->x, XtName(cn));
	       */	
	    break;
	    
	  case EastGravity:
	    /*
	     * left center to right center
	     */
	    XDrawLine (dpy, w, gc, srcx, srcy,
		       (cn->core.x +
			(((int) cn->core.border_width) << 1) +
			(int) cn->core.width),
		       (cn->core.y + ((int) cn->core.border_width) +
			((int) cn->core.height) / 2));
	    break;
	    
	  case SouthGravity:
	    /*
	     * top center to bottom center
	     */
	    XDrawLine (dpy, w, gc, srcx, srcy,
		       (cn->core.x + ((int) cn->core.border_width) +
			((int) cn->core.width) / 2),
		       (cn->core.y +
			(((int) cn->core.border_width) << 1) +
			(int) cn->core.height));
	    break;
	  }
	}
      }
    }
  }
}

static XtGeometryResult QueryGeometry (w, intended, preferred)
     Widget w;
     XtWidgetGeometry *intended, *preferred;
{
  register TreeWidget tw = (TreeWidget) w;
  
  preferred->request_mode = (CWWidth | CWHeight);
  preferred->width = tw->tree.maxwidth;
  preferred->height = tw->tree.maxheight;
  
  if (((intended->request_mode & (CWWidth | CWHeight)) ==
       (CWWidth | CWHeight)) &&
      intended->width == preferred->width &&
      intended->height == preferred->height)
    return XtGeometryYes;
  else if (preferred->width == w->core.width &&
	   preferred->height == w->core.height)
    return XtGeometryNo;
  else
    return XtGeometryAlmost;
}


/*****************************************************************************
 *                                                                           *
 *			     tree layout algorithm                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#define ydim(w) (horiz ? w->core.width : w->core.height + 2*w->core.border_width)
#define xdim(w) (!horiz ? w->core.width : w->core.height + 2*w->core.border_width)

#ifdef __ANSI__
void beautify(TreeWidget tw) {
#else
void beautify(tw)
  TreeWidget tw;
{
#endif
  TreeConstraintsPart *nt, *pnt, *mt, *nmt;
  Widget n, m;
  Layer l;
  Boolean horiz;
  Dimension xpadb, xpadn, ypad, bw;
  Position max_ydim, max_x, x;
  
  Position p, mindiff, diff, maxdiff, leastdiff=0;
  float leastdist = tw->tree.leastdist;
  int i;
  
/* print_tree(tw); */

  horiz=IsHorizontal(tw);
  ypad = horiz ? tw->tree.hpad : tw->tree.vpad;
  xpadb = !horiz ? tw->tree.hpad : tw->tree.vpad;
  xpadn = xpadb * tw->tree.relnb;  
  
  for (l=tw->tree.top_layer; l->next; l=l->next);  /* in die letzte Schicht */
  n=l->first; nt = &(T_C(n)->tree);
  nt->x = 0;
  l->y = 0;
  max_ydim=ydim(n);
  
  for (n=nt->next; n; n=nt->next) {  /* letzte Schicht vorpositionieren */
    nt = &(T_C(n)->tree);
    pnt = &(T_C(nt->prev)->tree);
    nt->x = pnt->x + xdim(nt->prev) + (siblings(nt->prev) ? xpadb:xpadn); 
    if (ydim(n) > max_ydim) max_ydim = ydim(n);
  }
  for (l=l->prev; l; l=l->prev) {  /* bottom up */
    l->y = l->next->y + ypad + max_ydim; 
    n=l->first; nt = &(T_C(n)->tree);
    nt->x = 0; max_ydim = 0;
    while (n) {  /* links -> rechts */
      if (ydim(n) > max_ydim) max_ydim = ydim(n);
      if (nt->prev) {  /* Knoten vorpositionieren */
	pnt = &(T_C(nt->prev)->tree);
	nt->x = pnt->x + xdim(nt->prev) + 
	  ((leastdiff > (siblings(nt->prev) ? xpadb:xpadn))
	   ? leastdiff : (siblings(nt->prev) ? xpadb:xpadn));
      }
      if (nt->first_child) {
	p=meanpos(nt->first_child); diff=nt->x - p;
	if (diff>0) {  /* Knoten schon weiter rechts => Soehne nach rechts */
	  mindiff=dists(nt->first_child, right, 0);
	  if (diff < mindiff) shift_tree(nt->first_child, diff);
	  else {
	    if (mindiff > 0) shift_tree(nt->first_child, mindiff);
	    push_layers(nt->first_child, diff-mindiff); 
	  }
	}
	if (diff<0) {  /* Soehne noch weiter rechts => Knoten nach rechts */
	  nt->x=p;      /* und Brueder links nachziehen (nach rechts) */
	  m=nt->prev; 
	  while (m && siblings(m)) {
	    mt = &(T_C(m)->tree);
	    nmt = &(T_C(mt->next)->tree);
	    mindiff=nmt->x - mt->x - xdim(m) - (siblings(m) ? xpadb:xpadn);
	    if (leastdiff>0) 
	      mindiff= nmt->x - mt->x - leastdiff;
	    if (mt->first_child) 
	      mindiff=dists(mt->first_child, right, mindiff);
	    if (mindiff>0) {
	      mt->x += mindiff; shift_tree(mt->first_child, mindiff);
	      p=mt->x; m=mt->prev;
	    } else m=NULL;
	  }
	}
      }
      if (nt->parent && !siblings(n)) {  /* letzter Bruder: */
        /* linkester u. rechtester Bruder fest => mittl. Brueder ausgleichen */
	for (i=1, m=nt->prev; m && siblings(m); i++, m=mt->prev)
	  mt = &(T_C(m)->tree);
	balance_l(tw, T_C(nt->parent)->tree.first_child, n, i); 
      }
      
      /* folgendes nur, falls Option -l<n> an und mind. 3 Brueder */
      if (leastdist>0.0 && !siblings(n) && nt->parent 
	  && nt->prev && siblings(nt->prev)) {
	printf("leastdist > 0.0 !!!\n");   
        m=nt->prev; mt = &(T_C(m)->tree);
        /* Maximal-/Minimalabstand zwischen Bruedern ermitteln */
	maxdiff=mindiff=nt->x - mt->x;
        for (m=mt->prev; m && siblings(m); m=mt->prev) {
	  mt = &(T_C(m)->tree);
	  diff = T_C(mt->next)->tree.x - mt->x;
	  if (diff > maxdiff) maxdiff = diff;
	  if (diff < mindiff) mindiff = diff;
	}
        if (maxdiff-1 > leastdist * mindiff) {
	  if (leastdiff>0) {
	    fprintf(stderr, "INTERNAL ERROR: leastdiff=%f\n", leastdiff);
	    exit(-1); }
	  leastdiff=maxdiff / leastdist;
	  
	  /* falls Kriterium erf., Knoten zurueckschieben und nochmal */
	  m = T_C(nt->parent)->tree.first_child;
	  for (; siblings(m); m=mt->next) {
	    mt = &(T_C(m)->tree);
	    if (mt->first_child) 
	      shift_tree(mt->first_child, -dists(mt->first_child, left, 0));
	  }
	  if (m && mt->first_child) 
	    shift_tree(mt->first_child, -dists(mt->first_child, left, 0));
          for (n=nt->prev; nt->prev && siblings(nt->prev); n=nt->prev)
	    nt = &(T_C(m)->tree);
	} else { 
	  leastdiff = 0; if (n=nt->next) nt = &(T_C(n)->tree);}
      } else { if (n=nt->next) nt = &(T_C(n)->tree); }
    }
  }
  tw->tree.maxheight = tw->tree.top_layer->y + ypad + max_ydim;
  for (l=tw->tree.top_layer, max_x=0; l; l=l->next) { /* Ausmasse */
    x = T_C(l->last)->tree.x + xdim(l->last);
    if (x > max_x) max_x = x;
  }
  tw->tree.maxwidth = max_x;
}

/* push_layers: Knoten und rechte Nachbarn mitsamt allen Soehnen verschieben */
#ifdef __ANSI__
void push_layers(Widget n, Position diff) {
#else
void push_layers(n, diff)
  Widget n;
  Position diff;
{
#endif
  Widget next;
  TreeConstraintsPart *nt;
  
  do {
    for (next=NULL; n; n=nt->next) {
      nt = &(T_C(n)->tree);
      if (!next && nt->first_child) next=nt->first_child;
      nt->x+=diff;
    }
    n=next;  /* next = linkester Sohn */
  } while (next);
}

#ifdef __ANSI__
void shift_tree(Widget n, Position diff) {
#else
void shift_tree(n, diff)
  Widget n;
  Position diff;
{
#endif
   TreeConstraintsPart *nt;
  
  if (n) {
    nt = &(T_C(n)->tree);
    while (siblings(n)) {
      nt->x += diff;
      shift_tree(nt->first_child, diff);
      n=nt->next; nt = &(T_C(n)->tree);
    }
    nt->x += diff;
    shift_tree(nt->first_child, diff);
  }
}

#ifdef __ANSI__
Position meanpos(Widget n) {
#else
Position meanpos(n)
  Widget n;
{
#endif
  float sum;  /* float, sonst zu starke Rundungsfehler */
  int k;
  Boolean horiz = IsHorizontal((TreeWidget) n->core.parent);
  
  sum = T_C(n)->tree.x + (float) xdim(n)/2;
  for (; siblings(n); n=T_C(n)->tree.next) ;
  sum += T_C(n)->tree.x + (float) xdim(n)/2;
  
  return ((sum - xdim(T_C(n)->tree.parent))/2);
}

#ifdef __ANSI__
Boolean siblings(Widget n) {
#else
Boolean siblings(n)
  Widget n;
{
#endif
  TreeConstraintsPart *nt;
  
  nt = &(T_C(n)->tree);
  
  return (nt->parent ? 
	  (T_C(nt->parent)->tree.last_child != n)
	  : FALSE);
}

/* dists liefert den minimalen Abstand eines Unterbaumes nach links/rechts
   (abhaengig von 'dir'). Befindet sich in einer Ebene kein Nachbar, so wird
   0 zurueckgegeben */
#ifdef __ANSI__
Position dists(Widget n, direct dir, Position mindiff) {
#else
Position dists(n, dir, mindiff)
  Widget n;
  direct dir;
  Position mindiff;
{
#endif
  Layer l, l0;
  
  for (l0=l=((TreeWidget)n->core.parent)->tree.top_layer; l; l=l->next) 
    l->used=0; /* Markierungen loeschen */
  dist_right(n, l0);  /* "vermessen" */
  if  (dir == right) {
    if (mindiff == 0) mindiff=l0->rightdiff;
    for (l=l0; l && l->used; l=l->next)  /* Minimum suchen */
      if (l->rightdiff < mindiff) mindiff=l->rightdiff;
  } else {
    if (mindiff == 0) mindiff=l0->leftdiff;
    for (l=l0; l && l->used; l=l->next)  /* Minimum suchen */
      if (l->leftdiff < mindiff) mindiff=l->leftdiff;
  }
  return (mindiff);
}

#ifdef __ANSI__
void dist_right(Widget n, Layer l) {
#else
void dist_right(n, l)
  Widget n;
  Layer l;
{
#endif
  Position diff;
  TreeConstraintsPart *nt, *nt2;
  Dimension xpadb, xpadn;
  TreeWidget tw;
  Boolean horiz;
  
  tw = (TreeWidget) n->core.parent;
  horiz = IsHorizontal(tw);
  nt = &(T_C(n)->tree);
  xpadb = IsHorizontal(tw) ? tw->tree.vpad : tw->tree.hpad;
  xpadn = xpadb * tw->tree.relnb;  
  
  if (nt->prev) {
    nt2 = &(T_C(nt->prev)->tree);
    l->leftdiff = nt->x - nt2->x - xdim(nt->prev) - 
      (siblings(nt->prev) ? xpadb:xpadn);
  } else l->leftdiff = 0;
  
  while (siblings(n)) {
    if (nt->first_child) dist_right(nt->first_child, l->next);
    n=nt->next; nt = &(T_C(n)->tree);
  }
  if (nt->first_child) dist_right(nt->first_child, l->next);
  if (nt->next) {  /* aber kein Bruder */
    if (l->used) {
      if (nt->x > l->right) {
	l->right=nt->x;
	nt2 = &(T_C(nt->next)->tree);
	l->rightdiff=nt2->x - nt->x - xdim(n) - 
	  (siblings(n) ? xpadb:xpadn); }
    } else {
      l->used=1;
      l->right=nt->x;
      nt2 = &(T_C(nt->next)->tree);
      l->rightdiff=nt2->x - nt->x - xdim(n) - (siblings(n) ? xpadb:xpadn); 
    }
  } else { l->used=1; 
	   l->right=nt->x; l->rightdiff=0; }
}

#ifdef __ANSI__
void balance_l(TreeWidget tw, Widget nl, Widget nr, int k)  {
#else
void balance_l(tw, nl, nr, k)
  TreeWidget tw;
  Widget nl;
  Widget nr;
  int k;
{
#endif
  float width, avrg, d0, dr;
  int i;
  Widget n;
  TreeConstraintsPart *nt;
  Boolean horiz;
  Dimension xpadb, xpadn;
  
  if (k <= 2) return;
  horiz=IsHorizontal(tw);
  xpadb = !horiz ? tw->tree.hpad : tw->tree.vpad;
  width = T_C(nr)->tree.x - T_C(nl)->tree.x - xdim(nl) - xpadb;
  for (i=2, n=T_C(nl)->tree.next; i<k; n=nt->next, i++) {
    nt = &(T_C(n)->tree);
    width -= xdim(n) + xpadb; }
  avrg = width/(k-1);
  for (i=2, n=T_C(nl)->tree.next; i<k; n=nt->next, i++) {
    nt = &(T_C(n)->tree);
    d0 = nt->x - T_C(nt->prev)->tree.x - xdim(nt->prev) - xpadb;
    if (d0 > avrg) {
      if (nt->first_child) {
	dr=dists(nt->first_child, left, d0);
	if (d0-avrg <= dr) {
	  nt->x -= d0-avrg; shift_tree(nt->first_child, -(d0-avrg));
	} else {
	  nt->x -= dr; shift_tree(nt->first_child, -dr);
	  balance_r(tw, nl, n, i);
	  balance_l(tw, n, nr, k-i+1);
	  return;
	}
      } else nt->x += avrg-d0;
    } else if (d0 < avrg) {
      while (T_C(nt->next)->tree.x - nt->x - xdim(n) - xpadb < avrg) {
	n=nt->next; 
	nt = &(T_C(n)->tree);
	i++;}
      balance_l(tw, nl, n, i);
      balance_l(tw, n, nr, k-i+1);
      return;
    }
  }
}

#ifdef __ANSI__
void balance_r(TreeWidget tw, Widget nl, Widget nr, int k)  {
#else
void balance_r(tw, nl, nr, k)
  TreeWidget tw;
  Widget nl;
  Widget nr;
  int k;
{
#endif
  float width, avrg, d0, dr;
  int i;
  Widget n;
  TreeConstraintsPart *nt;
  Boolean horiz;
  Dimension xpadb;

  if (k <= 2) return;
  horiz=IsHorizontal(tw);
  xpadb = !horiz ? tw->tree.hpad : tw->tree.vpad;

  width = T_C(nr)->tree.x - T_C(nl)->tree.x - xdim(nl) - xpadb;
  
  for (i=2, n=T_C(nl)->tree.next; i<k; n=nt->next, i++) {
    nt = &(T_C(n)->tree);
    width -= xdim(n) + xpadb; }
  avrg = width/(k-1);
  for (i=k-1, n=T_C(nr)->tree.prev; i>1; n=nt->prev, i--) {
    nt = &(T_C(n)->tree);
    d0 = T_C(nt->next)->tree.x - nt->x - 
      xdim(n) - xpadb;
    if (d0 > avrg) {
      if (nt->first_child) {
	dr=dists(nt->first_child, right, d0);
	if (d0-avrg <= dr) {
	  nt->x += d0-avrg; shift_tree(nt->first_child, d0-avrg);
	} else {
	  nt->x += dr; shift_tree(nt->first_child, dr);
	  balance_r(tw, nl, n, i);
	  balance_l(tw, n, nr, k-i+1);
	  return;
	}
      } else nt->x += d0-avrg;
    } else {
      while (nt->x - T_C(nt->prev)->tree.x - xdim(nt->prev) - xpadb < avrg) {
	n=nt->prev; 
	nt = &(T_C(n)->tree);
	i--;}
      balance_r(tw, nl, n, i);
      balance_r(tw, n, nr, k-i+1);
      return;
    }
  }
}

/* end tree layout procedures */


#ifdef __ANSI__
static void set_positions(TreeWidget tw) {
#else
static void set_positions(tw)
  TreeWidget tw;
{
#endif
  Layer l;
  Widget w;
  TreeConstraintsPart *t;
  Position y;
  
  for (l=tw->tree.top_layer; l; l=l->next) {
    y = l->prev ? l->prev->y : tw->tree.maxheight;
    for (w=l->first; w; w = T_C(w)->tree.next) {
      
      t = &(T_C(w)->tree);
      
      /* mirror if necessary */
      switch (tw->tree.gravity) {
      case NorthGravity:
	t->y = (Position) tw->tree.maxheight - y;
	break;
      case WestGravity:
	t->y = t->x;
	t->x = (Position) tw->tree.maxheight - y;
	break;
      case EastGravity:
	t->x = (((Position) tw->tree.maxwidth) -
		((Position) w->core.width) - t->x);
	break;
      case SouthGravity:
	t->y = y;
	break;
      }
      
      /* Move the widget into position. */
      XtMoveWidget (w, t->x, t->y);
    }
  }
}

static void set_tree_size (tw, insetvalues, width, height)
     TreeWidget tw;
     Boolean insetvalues;
     Dimension width, height;
{
  if (insetvalues) {
    tw->core.width = width;
    tw->core.height = height;
  } else {
    Dimension replyWidth = 0, replyHeight = 0;
    XtGeometryResult result = XtMakeResizeRequest ((Widget) tw,
						   width, height,
						   &replyWidth,
						   &replyHeight);
    /*
     * Accept any compromise.
     */
    if (result == XtGeometryAlmost)
      XtMakeResizeRequest ((Widget) tw, replyWidth, replyHeight,
			   (Dimension *) NULL, (Dimension *) NULL);
  }
  return;
}


#ifdef __ANSI__
static void layout_tree (TreeWidget tw, Boolean insetvalues) {
#else
static void layout_tree (tw, insetvalues)
  TreeWidget tw;
  Boolean insetvalues;
{
#endif
  int i;
  Dimension *dp;
  Layer l;
  
  if (!tw->tree.top_layer->first) return; /* kein Baum ! */

#ifdef DEBUG
  printf("layout_tree\n");  
#endif
  if (!tw->tree.connected) {
    for (l=tw->tree.top_layer->next; l; l=l->next) 
      l->first = NULL;
    connect_tree(tw->tree.top_layer->first, tw->tree.top_layer); 
    tw->tree.connected = TRUE; 
    /* nach delte_node: evtl. fehlt letzte Schicht */
    for (l=tw->tree.top_layer->next; l && l->next; l=l->next);  
    if (l && !l->first) {
      l->prev->next=NULL; free(l);
    }
  }

  beautify(tw);

  /* Move each widget into place. */
  set_positions (tw);
  if (IsHorizontal(tw)) 
    set_tree_size (tw, insetvalues, tw->tree.maxheight, tw->tree.maxwidth);
  else
    set_tree_size (tw, insetvalues, tw->tree.maxwidth, tw->tree.maxheight);
  
  /* And redisplay. */
  if (XtIsRealized ((Widget) tw)) {
    XClearArea (XtDisplay(tw), XtWindow((Widget)tw), 0, 0, 0, 0, True);
  }
}


/*****************************************************************************
 *                                                                           *
 * 				Public Routines                              *
 *                                                                           *
 *****************************************************************************/

void
#if NeedFunctionPrototypes
  XawTreeForceLayout (Widget tree)
#else
XawTreeForceLayout (tree)
     Widget tree;
#endif
{
  layout_tree ((TreeWidget) tree, FALSE);
}
