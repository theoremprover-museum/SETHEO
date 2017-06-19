#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Dialog.h>

#include <stdio.h>


extern void PopupCentered();

/*	Function Name: PopupCentered
 *	Description: Pops up the window specified under the location passed
 *                   in the event, or under the cursor.
 *	Arguments: event - the event that we should use.
 *                 w - widget to popup.
 *                 mode - mode to pop it up in.
 *	Returns: none
 */

void
PopupCentered(event, w, mode)
XEvent * event;
Widget w;
XtGrabKind mode;
{
    Boolean get_from_cursor = FALSE;
    Arg args[3];
    Cardinal num_args;
    Dimension width, height, b_width;
    int x, y, max_x, max_y;

    XtRealizeWidget(w);

    if (event == NULL)
	get_from_cursor = TRUE;
    else {
	switch (event->type) {
	case ButtonPress:
	case ButtonRelease:
	    x = event->xbutton.x_root;
	    y = event->xbutton.y_root;
	    break;
	case KeyPress:
	case KeyRelease:
	    x = event->xkey.x_root;
	    y = event->xkey.y_root;
	    break;
	default:
	    get_from_cursor = TRUE;
	    break;
	}
    }

    if (get_from_cursor) {
	Window root, child;
	int win_x, win_y;
	unsigned int mask;
	
	XQueryPointer(XtDisplay(w), XtWindow(w),
		      &root, &child, &x, &y, &win_x, &win_y, &mask);
    }

    num_args = 0;
    XtSetArg(args[num_args], XtNwidth, &width); num_args++;
    XtSetArg(args[num_args], XtNheight, &height); num_args++;
    XtSetArg(args[num_args], XtNborderWidth, &b_width); num_args++;
    XtGetValues(w, args, num_args);

    width += 2 * b_width;
    height += 2 * b_width;

    x -= ((int) width/2);
    if (x < 0) 
	x = 0;
    if ( x > (max_x = (int) (XtScreen(w)->width - width)) )
	x = max_x;

    y -= ( (Position) height/2 );
    if (y < 0) 
	y = 0;
    if ( y > (max_y = (int) (XtScreen(w)->height - height)) )
	y = max_y;
  
    num_args = 0;
    XtSetArg(args[num_args], XtNx, x); num_args++;
    XtSetArg(args[num_args], XtNy, y); num_args++;
    XtSetValues(w, args, num_args);

    XtPopup(w, mode);
}


	
