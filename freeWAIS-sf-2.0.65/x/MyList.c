/* Based on: */
/* XConsortium: List.c,v 1.34 91/09/27 18:35:07 converse Exp */

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


static char MyList_c_rcsid[] =
  "$Id: MyList.c,v 2.0.1.1 1995/09/15 09:56:01 pfeifer Exp $";

#include "../ir/cdialect.h"
#include <stdio.h>
/* #include <ctype.h> */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include "MyListP.h"

static char MyListP_h_rcsid[] =
  MyListP_id;

/*
 * Full class record constant
 */

static void Notify();

static XtActionsRec actions[] = {
  { "Notify",	Notify}
};

static char defaultTranslations[] =
  "<Btn1Down>:	Set()\n<Btn1Up>:	Notify()";

MyListClassRec myListClassRec = {
{
  (WidgetClass)&listClassRec,	/* superclass */
  "MyList",			/* class name */
  sizeof(MyListRec),		/* widget size */
  0,				/* class initialize */
  0,				/* class part initialize */
  FALSE,			/* class inited */
  0,				/* initialize */
  0,				/* initialize hook */
  XtInheritRealize,		/* realize */
  actions,			/* actions */
  XtNumber(actions),		/* num actions */
  0,				/* resources */
  0,				/* num resources */
  NULLQUARK,			/* xrm class */
  TRUE,				/* compress motion */
  FALSE,			/* compress exposure */
  TRUE,				/* compress enterleave */
  FALSE,			/* visible interest */
  0,				/* destroy */
  XtInheritResize,		/* resize */
  XtInheritExpose,		/* expose */
  0,				/* set values */
  0,				/* set values hook */
  XtInheritSetValuesAlmost,	/* set values almost */
  0,				/* get values hook */
  XtInheritAcceptFocus,		/* accept focus */
  XtVersion,			/* version */
  0,				/* callback private */
  defaultTranslations,		/* default translations */
  XtInheritQueryGeometry	/* query geometry */
  },
{
  XtInheritChangeSensitive,	/* change sensitive */
}
};

WidgetClass myListWidgetClass = (WidgetClass)&myListClassRec;

/* From List.c... */

/*	Function Name: CvtToItem
 *	Description: Converts Xcoord to item number of item containing that
 *                   point.
 *	Arguments: w - the list widget.
 *                 xloc, yloc - x location, and y location.
 *	Returns: the item number.
 */

static int
CvtToItem(w, xloc, yloc, item)
Widget w;
int xloc, yloc;
int *item;
{
    int one, another;
    ListWidget lw = (ListWidget) w;
    int ret_val = OKAY;

    if (lw->list.vertical_cols) {
        one = lw->list.nrows * ((xloc - (int) lw->list.internal_width)
	    / lw->list.col_width);
        another = (yloc - (int) lw->list.internal_height) 
	        / lw->list.row_height;
	 /* If out of range, return minimum possible value. */
	if (another >= lw->list.nrows) {
	    another = lw->list.nrows - 1;
	    ret_val = OUT_OF_RANGE;
	}
    }
    else {
        one = (lw->list.ncols * ((yloc - (int) lw->list.internal_height) 
              / lw->list.row_height)) ;
	/* If in right margin handle things right. */
        another = (xloc - (int) lw->list.internal_width) / lw->list.col_width;
	if (another >= lw->list.ncols) {
	    another = lw->list.ncols - 1; 
	    ret_val = OUT_OF_RANGE;
	}
    }  
    if ((xloc < 0) || (yloc < 0))
        ret_val = OUT_OF_RANGE;
    if (one < 0) one = 0;
    if (another < 0) another = 0;
    *item = one + another;
    if (*item >= lw->list.nitems) return(OUT_OF_RANGE);
    return(ret_val);
}


/*	Function Name: Notify
 *	Description: Notifies the user that a button has been pressed, and
 *                   calles the callback, if the XtNpasteBuffer resource
 *                   is true then the name of the item is also put in the
 *                   X cut buffer ( buf (0) ).
 *	Arguments: w - the widget that the notify occured in.
 *                 event - event that caused this notification.
 *                 params, num_params - not used.
 *	Returns: none.
 */

/* ARGSUSED */
static void
Notify(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal *num_params;
{
    ListWidget lw = ( ListWidget ) w;
    int item, item_len;
    XawListReturnStruct ret_value;

/* 
 * Find item and if out of range then unhighlight and tell the
 * client that we have done so, via the callback.
 * 
 * If the current item is unhighlighted then the user has aborted the
 * notify, so unhighlight and return.
 */

    if ( ((CvtToItem(w, event->xbutton.x, event->xbutton.y, &item))
	  == OUT_OF_RANGE) || (lw->list.highlight != item) ) {
        XawListUnhighlight(w);

	ret_value.string = NULL;
	ret_value.list_index = -1;
	XtCallCallbacks( w, XtNcallback, (XtPointer)&ret_value);
        return;
    }

    item_len = strlen(lw->list.list[item]);

    if ( lw->list.paste )	/* if XtNpasteBuffer set then paste it. */
        XStoreBytes(XtDisplay(w), lw->list.list[item], item_len);

/* 
 * Call Callback function.
 */

    ret_value.string = lw->list.list[item];
    ret_value.list_index = item;
    
    XtCallCallbacks( w, XtNcallback, (XtPointer) &ret_value);
}

