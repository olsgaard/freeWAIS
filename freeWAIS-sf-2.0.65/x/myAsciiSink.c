/* Borrowed by Jonny Goldman. */
/* $XConsortium: AsciiSink.c,v 1.57 91/07/21 20:35:00 converse Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: myAsciiSink.c,v $
 * Revision 2.0.1.1  1995/09/15 09:56:43  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:43  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.9  1995/02/10  10:33:58  pfeifer
 * Release 1.1.2
 *
 * Revision 1.8  1994/10/31  15:05:12  pfeifer
 * runns under X11R6
 *
 * Revision 1.7  1994/10/21  13:01:17  pfeifer
 * Moved the Log to the top
 *
 * Revision 1.6  1994/10/21  12:50:28  pfeifer
 * Now compiles under X11R6 - query font is invisible :-(
 *
 * Revision 1.5  1994/09/08  16:43:50  pfeifer
 * Added include cdialect.h to enshur, that string.h is included before ctype.h
 *
 * Revision 1.4  1994/08/05  07:25:00  pfeifer
 * Release beta 04
 *
 * Revision 1.3  1994/03/23  19:09:33  pfeifer
 * *** empty log message ***
 *
 * Revision 1.2  93/07/02  18:33:30  warnock
 * included ctype.h
 * 
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.2  92/06/22  10:41:17  jonathan
 * Borrowed from Xaw.  Modified to use an alternate font for keyword
 * highlighting.
 * 
 */

#include "../ir/cdialect.h"
#include <stdio.h>
#include <ctype.h>

#if (XtSpecificationRelease==5)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif /* not XtSpecificationRelease==5 */
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>
#include "myAsciiSinkP.h"
#include <X11/Xaw/AsciiSrcP.h>	/* For source function defs. */
#include <X11/Xaw/TextP.h>	/* I also reach into the text widget. */

#ifdef GETLASTPOS
#undef GETLASTPOS		/* We will use our own GETLASTPOS. */
#endif

#ifndef XawLF
#define XawLF	0x0a
#define XawCR	0x0d
#define XawTAB	0x09
#define XawBS	0x08
#define XawSP	0x20
#define XawDEL	0x7f
#define XawBSLASH	'\\'
#endif

#if (XtSpecificationRelease==5)
#define GETLASTPOS XawTextSourceScan(source, 0, XawstAll, XawsdRight, 1, TRUE)
#else /* XtSpecificationRelease==5 */
#define GETLASTPOS XawTextSourceScan(source, (XawTextPosition) 0, XawstAll, XawsdRight, 1, TRUE)
#endif /* XtSpecificationRelease==5 */

static void Initialize(), Destroy();
static Boolean SetValues();
#if (XtSpecificationRelease!=5)
static int MaxLines(), MaxHeight();
static void SetTabs();
#endif /* XtSpecificationRelease==5 */

static void DisplayText(), InsertCursor(), FindPosition();
static void FindDistance(), Resolve(), GetCursorBounds();

#define offset(field) XtOffsetOf(myAsciiSinkRec, ascii_sink.field)

static XtResource resources[] = {
#if (XtSpecificationRelease!=5)
    {XtNfont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	offset(font), XtRString, XtDefaultFont},
#endif /* XtSpecificationRelease==5 */
    {XtNecho, XtCOutput, XtRBoolean, sizeof(Boolean),
	offset(echo), XtRImmediate, (XtPointer) True},
    {XtNdisplayNonprinting, XtCOutput, XtRBoolean, sizeof(Boolean),
	offset(display_nonprinting), XtRImmediate, (XtPointer) True},
    {"altfont", XtCFont, XtRFontStruct, sizeof (XFontStruct *),
       offset(altfont), XtRString, XtDefaultFont},
};
#undef offset

#define SuperClass		(&textSinkClassRec)
myAsciiSinkClassRec myasciiSinkClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"MyAsciiSink",
    /* widget_size	  	*/	sizeof(myAsciiSinkRec),
    /* class_initialize   	*/	XawInitializeWidgetSet,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1		  	*/	NULL,
    /* obj2		  	*/	NULL,
    /* obj3		  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* obj4		  	*/	FALSE,
    /* obj5		  	*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7		  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* obj8		  	*/	NULL,
    /* obj9		  	*/	NULL,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11		 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* obj12		   	*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
/* text_sink_class fields */
  {
    /* DisplayText              */      DisplayText,
    /* InsertCursor             */      InsertCursor,
    /* ClearToBackground        */      XtInheritClearToBackground,
    /* FindPosition             */      FindPosition,
    /* FindDistance             */      FindDistance,
    /* Resolve                  */      Resolve,
#if (XtSpecificationRelease==5)
    /* MaxLines                 */      XtInheritMaxLines,
    /* MaxHeight                */      XtInheritMaxHeight,
    /* SetTabs                  */      XtInheritSetTabs,
#else /* XtSpecificationRelease==5 */
    /* MaxLines                 */      MaxLines,
    /* MaxHeight                */      MaxHeight,
    /* SetTabs                  */      SetTabs,
#endif /* XtSpecificationRelease==5 */
    /* GetCursorBounds          */      GetCursorBounds
  },
/* ascii_sink_class fields */
  {
    /* unused			*/	0
  }
};

WidgetClass myasciiSinkObjectClass = (WidgetClass)&myasciiSinkClassRec;

/* Utilities */

static int 
CharWidth (w, x, c, font)
Widget w;
int x;
unsigned char c;
XFontStruct *font;
{
    register int    i, width, nonPrinting;
    myAsciiSinkObject sink = (myAsciiSinkObject) w;
    Position *tab;

    if ( c == XawLF ) return(0);

    if (c == XawTAB) {
	/* Adjust for Left Margin. */
	x -= ((TextWidget) XtParent(w))->text.margin.left;

	if (x >= (int)XtParent(w)->core.width) return 0;
	for (i = 0, tab = sink->text_sink.tabs ; 
	     i < sink->text_sink.tab_count ; i++, tab++) {
	    if (x < *tab) {
		if (*tab < (int)XtParent(w)->core.width)
		    return *tab - x;
		else
		    return 0;
	    }
	}
	return 0;
    }

    if ( (nonPrinting = (c < (unsigned char) XawSP)) )
	if (sink->ascii_sink.display_nonprinting)
	    c += '@';
	else {
	    c = XawSP;
	    nonPrinting = False;
	}

    if (font->per_char &&
	    (c >= font->min_char_or_byte2 && c <= font->max_char_or_byte2))
	width = font->per_char[c - font->min_char_or_byte2].width;
    else
	width = font->min_bounds.width;

    if (nonPrinting)
	width += CharWidth(w, x, (unsigned char) '^', font);

    return width;
}

/*	Function Name: PaintText
 *	Description: Actually paints the text into the windoe.
 *	Arguments: w - the text widget.
 *                 gc - gc to paint text with.
 *                 x, y - location to paint the text.
 *                 buf, len - buffer and length of text to paint.
 *	Returns: the width of the text painted, or 0.
 *
 * NOTE:  If this string attempts to paint past the end of the window
 *        then this function will return zero.
 */

static Dimension
PaintText(w, gc, x, y, buf, len, font)
Widget w;
GC gc;
Position x, y;
unsigned char * buf;
int len;
XFontStruct *font;
{
    myAsciiSinkObject sink = (myAsciiSinkObject) w;
    TextWidget ctx = (TextWidget) XtParent(w);

    Position max_x;
    Dimension width = XTextWidth(font, (char *) buf, len); 
    max_x = (Position) ctx->core.width;

    if ( ((int) width) <= -x)	           /* Don't draw if we can't see it. */
      return(width);

    XDrawImageString(XtDisplay(ctx), XtWindow(ctx), gc, 
		     (int) x, (int) y, (char *) buf, len);
    if ( (((Position) width + x) > max_x) && (ctx->text.margin.right != 0) ) {
	x = ctx->core.width - ctx->text.margin.right;
	width = ctx->text.margin.right;
	XFillRectangle(XtDisplay((Widget) ctx), XtWindow( (Widget) ctx),
		       sink->ascii_sink.normgc, (int) x,
		       (int) y - font->ascent, 
		       (unsigned int) width,
		       (unsigned int) (font->ascent +
				       font->descent));
	return(0);
    }
    return(width);
}

#define STRINGSIZE 1000

int findKeyword(text, keylength)
char *text;
long *keylength;
{
  static char str[STRINGSIZE], *keys, *buf, b1[STRINGSIZE];
  int i, j, k, minpos, offset, pos;
  char c1, c2;

  minpos = 999999;
  if((keys = (char*)GetKeywordsUsed()) != NULL) {

    if(keylength != NULL && *keylength != 0 && *keylength < STRINGSIZE) {
      buf = b1;
      strncpy(buf, text, *keylength);
      buf[*keylength] = 0;
    }
    else buf = text;

    /* parse the keywords into individual words */
    for(j = 0, i = 0; i <= strlen(keys); i++) {
      str[j] = keys[i];
      if ((keys[i] == 0) || 
	  isspace(keys[i]) ||
	  ispunct(keys[i])) {
	str[j] = 0;
	j = 0;

	for(offset = 0, 
	    pos = findstring(buf+offset, str, FALSE);
	    pos >= 0;
	    offset=pos+1, 
	    pos = findstring(buf+offset, str, FALSE)) {
	  pos+=offset;
	  c1 = (pos==0)?' ':buf[pos-1];
	  c2 = buf[pos+strlen(str)];
	  if(c2==0) c2=' ';
	  if((isspace(c1) || ispunct(c1)) &&
	     (isspace(c2) || ispunct(c2))) {
	    if (pos < minpos) {
	      minpos = pos;
	      if(keylength != NULL)
		*keylength = strlen(str);
	      break;
	    }
	  }
	}
      }
      else
	j++;
    }
  }
  return minpos;
}

/* Sink Object Functions */

/*
 * This function does not know about drawing more than one line of text.
 */
 
#ifndef MAXIMUM
#define MAXIMUM(x, y) ((x)>(y)?(x):(y))
#endif
#ifndef MINIMUM
#define MINIMUM(x, y) ((x)<(y)?(x):(y))
#endif

static void 
DisplayText(w, x, y, pos1, pos2, highlight)
Widget w;
Position x, y;
Boolean highlight;
XawTextPosition pos1, pos2;
{
  myAsciiSinkObject sink = (myAsciiSinkObject) w;
  Widget source = XawTextGetSource(XtParent(w));
  unsigned char buf[BUFSIZ];

  int j, k, keypos, keylen, ws = pos1, we = pos2;
  XawTextBlock blk;
  GC gc = highlight ? sink->ascii_sink.invgc : sink->ascii_sink.normgc;
  GC invgc = highlight ? sink->ascii_sink.normgc : sink->ascii_sink.invgc;
  GC altgc = highlight ? sink->ascii_sink.invaltgc: sink->ascii_sink.altgc;
  GC invaltgc = highlight? sink->ascii_sink.altgc : sink->ascii_sink.invaltgc;

#if (XtSpecificationRelease==5)
  XFontStruct *font = sink->text_sink.font;
#else /* XtSpecificationRelease */
  XFontStruct *font = sink->ascii_sink.font;
#endif /* XtSpecificationRelease */
  XFontStruct *altfont = sink->ascii_sink.altfont;

  if (!sink->ascii_sink.echo) return;

#if (XtSpecificationRelease==5)
  y += sink->text_sink.font->ascent;
#else /* XtSpecificationRelease */
  y += sink->ascii_sink.font->ascent;
#endif /* XtSpecificationRelease */
  /* adjust positions to word boundaries */
  for(keypos = ws; ws > 0; ws--) {
    XawTextSourceRead(source, ws, &blk, 1);
    if (isspace(blk.ptr[0]) ||
	ispunct(blk.ptr[0])) {
      ws++;
      break;
    }
  }
  for (we=pos2; 
       XawTextSourceRead(source, we, &blk, 1) &&
       (blk.length != 0);
       we++) {
    if (isspace(blk.ptr[0]) ||
	ispunct(blk.ptr[0])) {
      break;
    }
  }
  we = MAXIMUM(we, pos2);
  if(ws != pos1 || we != pos2) {
    XawTextSourceRead(source, ws, &blk, we - ws);
    keylen = blk.length;
    keypos = findKeyword(blk.ptr, &keylen);
    if(keypos == 0) {
      pos1 = XawTextSourceRead(source, pos1, &blk, 
			       MINIMUM((keylen - (pos1 - ws)), (pos2 - pos1)));
      for(j = 0; j < blk.length; j++) buf[j] = blk.ptr[j];
#ifdef USE_ALTFONT	    
      x += PaintText(w, altgc, x, y, buf, j, altfont);
#else
      x += PaintText(w, invgc, x, y, buf, j, font);
#endif
    }
  }
  for ( j = 0 ; pos1 < pos2 ; ) {
    pos1 = XawTextSourceRead(source, pos1, &blk, pos2 - pos1);
    keylen = blk.length;
    keypos = findKeyword(blk.ptr, &keylen);
    for (k = 0; k < blk.length; k++) {
      if (k == keypos) {
	x += PaintText(w, gc, x, y, buf, j, font);
	for(j = 0; j < keylen; j++) buf[j] = blk.ptr[k++];
#ifdef USE_ALTFONT	    
	x += PaintText(w, altgc, x, y, buf, j, altfont);
#else
	x += PaintText(w, invgc, x, y, buf, j, font);
#endif
	j = 0;
	keylen = blk.length-k;
	keypos = findKeyword((blk.ptr + k), &keylen)+k;
      }
      if (j >= BUFSIZ) {	/* buffer full, dump the text. */
	x += PaintText(w, gc, x, y, buf, j, font);
	j = 0;
      }
      buf[j] = blk.ptr[k];
      if (buf[j] == XawLF)	/* line feeds ('\n') are not printed. */
	continue;

      else if (buf[j] == '\t') {
	Position temp = 0;
	Dimension width;

	if ((j != 0) && ((temp = PaintText(w, gc, x, y, buf, j, font)) == 0))
	  return;

	x += temp;
	width = CharWidth(w, x, (unsigned char) '\t', font);
	XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),
		       invgc, (int) x,
		       (int) y - font->ascent,
		       (unsigned int) width,
		       (unsigned int) (font->ascent +
				       font->descent));
	x += width;
	j = -1;
      }
      else if ( buf[j] < (unsigned char) ' ' ) {
	if (sink->ascii_sink.display_nonprinting) {
	  buf[j + 1] = buf[j] + '@';
	  buf[j] = '^';
	  j++;
	}
	else
	  buf[j] = ' ';
      }
      j++;
    }
  }
  if (j > 0)
    (void) PaintText(w, gc, x, y, buf, j, font);
}


#define insertCursor_width 6
#define insertCursor_height 3
static char insertCursor_bits[] = {0x0c, 0x1e, 0x33};

static Pixmap
CreateInsertCursor(s)
Screen *s;
{
    return (XCreateBitmapFromData (DisplayOfScreen(s), RootWindowOfScreen(s),
		  insertCursor_bits, insertCursor_width, insertCursor_height));
}

/*	Function Name: GetCursorBounds
 *	Description: Returns the size and location of the cursor.
 *	Arguments: w - the text object.
 * RETURNED        rect - an X rectangle to return the cursor bounds in.
 *	Returns: none.
 */

static void
GetCursorBounds(w, rect)
Widget w;
XRectangle * rect;
{
    myAsciiSinkObject sink = (myAsciiSinkObject) w;

    rect->width = (unsigned short) insertCursor_width;
    rect->height = (unsigned short) insertCursor_height;
    rect->x = sink->ascii_sink.cursor_x - (short) (rect->width / 2);
    rect->y = sink->ascii_sink.cursor_y - (short) rect->height;
}

/*
 * The following procedure manages the "insert" cursor.
 */

static void
InsertCursor (w, x, y, state)
Widget w;
Position x, y;
XawTextInsertState state;
{
    myAsciiSinkObject sink = (myAsciiSinkObject) w;
    Widget text_widget = XtParent(w);
    XRectangle rect;

    sink->ascii_sink.cursor_x = x;
    sink->ascii_sink.cursor_y = y;

    GetCursorBounds(w, &rect);
    if (state != sink->ascii_sink.laststate && XtIsRealized(text_widget)) 
        XCopyPlane(XtDisplay(text_widget),
		   sink->ascii_sink.insertCursorOn,
		   XtWindow(text_widget), sink->ascii_sink.xorgc,
		   0, 0, (unsigned int) rect.width, (unsigned int) rect.height,
		   (int) rect.x, (int) rect.y, 1);
    sink->ascii_sink.laststate = state;
}

/*
 * Given two positions, find the distance between them.
 */

static void
FindDistance (w, fromPos, fromx, toPos, resWidth, resPos, resHeight)
Widget w;
XawTextPosition fromPos;	/* First position. */
int fromx;			/* Horizontal location of first position. */
XawTextPosition toPos;		/* Second position. */
int *resWidth;			/* Distance between fromPos and resPos. */
XawTextPosition *resPos;	/* Actual second position used. */
int *resHeight;			/* Height required. */
{
    myAsciiSinkObject sink = (myAsciiSinkObject) w;
    Widget source = XawTextGetSource(XtParent(w));

    register XawTextPosition index, lastPos;
    register unsigned char c;
    XawTextBlock blk;

    /* we may not need this */
    lastPos = GETLASTPOS;
    XawTextSourceRead(source, fromPos, &blk, toPos - fromPos);
    *resWidth = 0;
    for (index = fromPos; index != toPos && index < lastPos; index++) {
	if (index - blk.firstPos >= blk.length)
	    XawTextSourceRead(source, index, &blk, toPos - fromPos);
	c = blk.ptr[index - blk.firstPos];
#if (XtSpecificationRelease==5)
	*resWidth += CharWidth(w, fromx + *resWidth, c, sink->text_sink.font);
#else /* XtSpecificationRelease */
	*resWidth += CharWidth(w, fromx + *resWidth, c, sink->ascii_sink.font);
#endif /* XtSpecificationRelease */
	if (c == XawLF) {
	    index++;
	    break;
	}
    }
    *resPos = index;
#if (XtSpecificationRelease==5)
    *resHeight = sink->text_sink.font->ascent +sink->text_sink.font->descent;
#else /* XtSpecificationRelease */
    *resHeight = sink->ascii_sink.font->ascent +sink->ascii_sink.font->descent;
#endif /* XtSpecificationRelease */
}


static void
FindPosition(w, fromPos, fromx, width, stopAtWordBreak, 
		  resPos, resWidth, resHeight)
Widget w;
XawTextPosition fromPos; 	/* Starting position. */
int fromx;			/* Horizontal location of starting position.*/
int width;			/* Desired width. */
int stopAtWordBreak;		/* Whether the resulting position should be at
				   a word break. */
XawTextPosition *resPos;	/* Resulting position. */
int *resWidth;			/* Actual width used. */
int *resHeight;			/* Height required. */
{
    myAsciiSinkObject sink = (myAsciiSinkObject) w;
    Widget source = XawTextGetSource(XtParent(w));

    XawTextPosition lastPos, index, whiteSpacePosition;
    int     lastWidth, whiteSpaceWidth;
    Boolean whiteSpaceSeen;
    unsigned char c;
    XawTextBlock blk;

    lastPos = GETLASTPOS;

    XawTextSourceRead(source, fromPos, &blk, BUFSIZ);
    *resWidth = 0;
    whiteSpaceSeen = FALSE;
    c = 0;
    for (index = fromPos; *resWidth <= width && index < lastPos; index++) {
	lastWidth = *resWidth;
	if (index - blk.firstPos >= blk.length)
	    XawTextSourceRead(source, index, &blk, BUFSIZ);
	c = blk.ptr[index - blk.firstPos];
#if (XtSpecificationRelease==5)
	*resWidth += CharWidth(w, fromx + *resWidth, c, sink->text_sink.font);
#else /* XtSpecificationRelease */
	*resWidth += CharWidth(w, fromx + *resWidth, c, sink->ascii_sink.font);
#endif /* XtSpecificationRelease */

	if ((c == XawSP || c == XawTAB) && *resWidth <= width) {
	    whiteSpaceSeen = TRUE;
	    whiteSpacePosition = index;
	    whiteSpaceWidth = *resWidth;
	}
	if (c == XawLF) {
	    index++;
	    break;
	}
    }
    if (*resWidth > width && index > fromPos) {
	*resWidth = lastWidth;
	index--;
	if (stopAtWordBreak && whiteSpaceSeen) {
	    index = whiteSpacePosition + 1;
	    *resWidth = whiteSpaceWidth;
	}
    }
    if (index == lastPos && c != XawLF) index = lastPos + 1;
    *resPos = index;
#if (XtSpecificationRelease==5)
    *resHeight = sink->text_sink.font->ascent +sink->text_sink.font->descent;
#else /* XtSpecificationRelease */
    *resHeight = sink->ascii_sink.font->ascent +sink->ascii_sink.font->descent;
#endif /* XtSpecificationRelease */
}

static void
Resolve (w, pos, fromx, width, leftPos, rightPos)
Widget w;
XawTextPosition pos;
int fromx, width;
XawTextPosition *leftPos, *rightPos;
{
    int resWidth, resHeight;
    Widget source = XawTextGetSource(XtParent(w));

    FindPosition(w, pos, fromx, width, FALSE, leftPos, &resWidth, &resHeight);
    if (*leftPos > GETLASTPOS)
      *leftPos = GETLASTPOS;
    *rightPos = *leftPos;
}

static void
GetGC(sink)
myAsciiSinkObject sink;
{
    XtGCMask valuemask = (GCFont | 
			  GCGraphicsExposures | GCForeground | GCBackground );
    XGCValues values;

#if (XtSpecificationRelease==5)
    values.font = sink->text_sink.font->fid;
#else /* XtSpecificationRelease */
    values.font = sink->ascii_sink.font->fid;
#endif /* XtSpecificationRelease */
    values.graphics_exposures = (Bool) FALSE;
    
    values.foreground = sink->text_sink.foreground;
    values.background = sink->text_sink.background;
    sink->ascii_sink.normgc = XtGetGC((Widget)sink, valuemask, &values);
    
    values.foreground = sink->text_sink.background;
    values.background = sink->text_sink.foreground;
    sink->ascii_sink.invgc = XtGetGC((Widget)sink, valuemask, &values);
    
    /*if((sink->ascii_sink.altfont = 
	XLoadQueryFont(XtDisplay(sink->object.parent),
		       "-*-courier-bold-r-*-*-14-*-*-*-*-*-*-*")) != NULL)*/
    values.font = sink->ascii_sink.altfont->fid;

    values.foreground = sink->text_sink.foreground;
    values.background = sink->text_sink.background;
    sink->ascii_sink.altgc = XtGetGC((Widget)sink, valuemask, &values);
    
    values.foreground = sink->text_sink.background;
    values.background = sink->text_sink.foreground;
    sink->ascii_sink.invaltgc = XtGetGC((Widget)sink, valuemask, &values);
    
    values.function = GXxor;
    values.background = (unsigned long) 0L;	/* (pix ^ 0) = pix */
    values.foreground = (sink->text_sink.background ^ 
			 sink->text_sink.foreground);
    valuemask = GCGraphicsExposures | GCFunction | GCForeground | GCBackground;
    
    sink->ascii_sink.xorgc = XtGetGC((Widget)sink, valuemask, &values);
}


/***** Public routines *****/

/*	Function Name: Initialize
 *	Description: Initializes the TextSink Object.
 *	Arguments: request, new - the requested and new values for the object
 *                                instance.
 *	Returns: none.
 *
 */

#define TAB_COUNT 32

/* ARGSUSED */
static void
Initialize(request, new)
Widget request, new;
{
  int tabs[TAB_COUNT], i, tab;
  myAsciiSinkObject sink = (myAsciiSinkObject) new;

  GetGC(sink);
    
  sink->ascii_sink.insertCursorOn= CreateInsertCursor(XtScreenOfObject(new));
  sink->ascii_sink.laststate = XawisOff;
  sink->ascii_sink.cursor_x = sink->ascii_sink.cursor_y = 0;

  for (i=0, tab=0 ; i < TAB_COUNT ; i++) 
    tabs[i] = (tab += 8);
  
  XawTextSinkSetTabs((Widget)sink, TAB_COUNT, tabs);
}

/*	Function Name: Destroy
 *	Description: This function cleans up when the object is 
 *                   destroyed.
 *	Arguments: w - the AsciiSink Object.
 *	Returns: none.
 */

static void
Destroy(w)
Widget w;
{
   myAsciiSinkObject sink = (myAsciiSinkObject) w;

   XtReleaseGC(w, sink->ascii_sink.normgc);
   XtReleaseGC(w, sink->ascii_sink.invgc);
   XtReleaseGC(w, sink->ascii_sink.xorgc);
   XtReleaseGC(w, sink->ascii_sink.altgc);
   XtReleaseGC(w, sink->ascii_sink.invaltgc);
   XFreePixmap(XtDisplayOfObject(w), sink->ascii_sink.insertCursorOn);
}

/*	Function Name: SetValues
 *	Description: Sets the values for the AsciiSink
 *	Arguments: current - current state of the object.
 *                 request - what was requested.
 *                 new - what the object will become.
 *	Returns: True if redisplay is needed.
 */

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
Widget current, request, new;
{
    myAsciiSinkObject w = (myAsciiSinkObject) new;
    myAsciiSinkObject old_w = (myAsciiSinkObject) current;

#if (XtSpecificationRelease==5)
    if (w->text_sink.font != old_w->text_sink.font
#else /* XtSpecificationRelease */
    if (w->ascii_sink.font != old_w->ascii_sink.font
#endif /* XtSpecificationRelease */
	|| w->ascii_sink.altfont != old_w->ascii_sink.altfont
	|| w->text_sink.background != old_w->text_sink.background
	|| w->text_sink.foreground != old_w->text_sink.foreground) {
	XtReleaseGC((Widget)w, w->ascii_sink.normgc);
	XtReleaseGC((Widget)w, w->ascii_sink.invgc);
	XtReleaseGC((Widget)w, w->ascii_sink.xorgc);
	XtReleaseGC((Widget)w, w->ascii_sink.altgc);
	XtReleaseGC((Widget)w, w->ascii_sink.invaltgc);
	GetGC(w);
	((TextWidget)XtParent(new))->text.redisplay_needed = True;
    } else {
	if ( (w->ascii_sink.echo != old_w->ascii_sink.echo) ||
	     (w->ascii_sink.display_nonprinting != 
                                     old_w->ascii_sink.display_nonprinting) )
	    ((TextWidget)XtParent(new))->text.redisplay_needed = True;
    }
    
    return False;
}

#if (XtSpecificationRelease!=5)
/*	Function Name: MaxLines
 *	Description: Finds the Maximum number of lines that will fit in
 *                   a given height.
 *	Arguments: w - the AsciiSink Object.
 *                 height - height to fit lines into.
 *	Returns: the number of lines that will fit.
 */

/* ARGSUSED */
static int
MaxLines(w, height)
Widget w;
Dimension height;
{
  myAsciiSinkObject sink = (myAsciiSinkObject) w;
  int font_height;

  font_height = sink->ascii_sink.font->ascent + sink->ascii_sink.font->descent;
  return( ((int) height) / font_height );
}

/*	Function Name: MaxHeight
 *	Description: Finds the Minium height that will contain a given number 
 *                   lines.
 *	Arguments: w - the myAsciiSink Object.
 *                 lines - the number of lines.
 *	Returns: the height.
 */

/* ARGSUSED */
static int
MaxHeight(w, lines)
Widget w;
int lines;
{
  myAsciiSinkObject sink = (myAsciiSinkObject) w;

  return(lines * (sink->ascii_sink.font->ascent + 
		  sink->ascii_sink.font->descent));
}

/*	Function Name: SetTabs
 *	Description: Sets the Tab stops.
 *	Arguments: w - the myAsciiSink Object.
 *                 tab_count - the number of tabs in the list.
 *                 tabs - the text positions of the tabs.
 *	Returns: none
 */

static void
SetTabs(w, tab_count, tabs)
Widget w;
int tab_count;
short *tabs;
{
  myAsciiSinkObject sink = (myAsciiSinkObject) w;
  int i;
  Atom XA_FIGURE_WIDTH;
  unsigned long figure_width = 0;
  XFontStruct *font = sink->ascii_sink.font;

/*
 * Find the figure width of the current font.
 */

  XA_FIGURE_WIDTH = XInternAtom(XtDisplayOfObject(w), "FIGURE_WIDTH", FALSE);
  if ( (XA_FIGURE_WIDTH != None) && 
       ( (!XGetFontProperty(font, XA_FIGURE_WIDTH, &figure_width)) ||
	 (figure_width == 0)) ) 
    if (font->per_char && font->min_char_or_byte2 <= '$' &&
	font->max_char_or_byte2 >= '$')
      figure_width = font->per_char['$' - font->min_char_or_byte2].width;
    else
      figure_width = font->max_bounds.width;

  if (tab_count > sink->text_sink.tab_count) {
    sink->text_sink.tabs = (Position *)
	XtRealloc((char *) sink->text_sink.tabs,
		  (Cardinal) (tab_count * sizeof(Position)));
    sink->text_sink.char_tabs = (short *)
	XtRealloc((char *) sink->text_sink.char_tabs,
		  (Cardinal) (tab_count * sizeof(short)));
  }

  for ( i = 0 ; i < tab_count ; i++ ) {
    sink->text_sink.tabs[i] = tabs[i] * figure_width;
    sink->text_sink.char_tabs[i] = tabs[i];
  }
    
  sink->text_sink.tab_count = tab_count;

#ifndef NO_TAB_FIX
  {  TextWidget ctx = (TextWidget)XtParent(w);
      ctx->text.redisplay_needed = True;
      _XawTextBuildLineTable(ctx, ctx->text.lt.top, TRUE);
  }
#endif
}
#endif /* XtSpecificationRelease==5 */
