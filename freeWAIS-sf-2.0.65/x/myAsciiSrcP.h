/* Borrowed by Jonny Goldman. */
/*
* $XConsortium: AsciiSrcP.h,v 1.8 91/02/20 17:58:15 converse Exp $
*/


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
 * AsciiSrcP.h - Private Header for Ascii Text Source.
 *
 * This is the private header file for the Ascii Text Source.
 * It is intended to be used with the Text widget, the simplest way to use
 * this text source is to use the AsciiText Object.
 *
 * Date:    June 29, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium 
 *          kit@expo.lcs.mit.edu
 */

/* 
 * TextSrcP.h - Private definitions for AsciiSrc object
 * 
 */

#ifndef _myAsciiSrcP_h
#define _myAsciiSrcP_h

#ifdef sgi
#include "myAsciiSrc.h"
#include <X11/Xaw/TextSrcP.h>
#else
#include <X11/Xaw/TextSrcP.h>
#include "myAsciiSrc.h"
#endif

/************************************************************
 *
 * Private declarations.
 *
 ************************************************************/

#ifdef L_tmpnam
#define TMPSIZ L_tmpnam
#else
#define TMPSIZ 32		/* bytes to allocate for tmpnam */
#endif

#define MAGIC_VALUE ((XawTextPosition) -1) /* Magic value. */

#define streq(a, b)        ( strcmp((a), (b)) == 0 )

typedef struct _Piece {		/* Piece of the text file of BUFSIZ allocated 
				   characters. */
  char * text;			/* The text in this buffer. */
  XawTextPosition used;		/* The number of characters of this buffer 
				   that have been used. */
  struct _Piece *prev, *next;	/* linked list pointers. */
} Piece;

/************************************************************
 *
 * New fields for the AsciiSrc object class record.
 *
 ************************************************************/

typedef struct _MyAsciiSrcClassPart { char foo; } MyAsciiSrcClassPart;

/* Full class record declaration */
typedef struct _MyAsciiSrcClassRec {
    ObjectClassPart     object_class;
    TextSrcClassPart	text_src_class;
    MyAsciiSrcClassPart	ascii_src_class;
} MyAsciiSrcClassRec;

extern MyAsciiSrcClassRec myasciiSrcClassRec;

/* New fields for the AsciiSrc object record */

typedef struct _MyAsciiSrcPart {

  /* Resources. */

  char       *string;		/* either the string, or the
				   file name, depending upon the type. */
  XawAsciiType type;		/* either string or disk. */
  XawTextPosition piece_size;	/* Size of text buffer for each piece. */
  Boolean data_compression;	/* compress to minimum memory automatically
				   on save? */
  XtCallbackList callback;	/* A callback list to call when the source is
				   changed. */
  Boolean use_string_in_place;	/* Use the string passed in place. */
  int     ascii_length;		/* length field for ascii string emulation. */

#ifdef ASCII_DISK
  String filename;		/* name of file for Compatability. */
#endif /* ASCII_DISK */

/* Private data. */

  Boolean	is_tempfile;	  /* Is this a temporary file? */
  Boolean       changes;	  /* Has this file been edited? */
  Boolean       allocated_string; /* Have I allocated the
				     string in ascii_src->string? */
  XawTextPosition length; 	/* length of file */
  Piece * first_piece;		/* first piece of the text. */
} MyAsciiSrcPart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MyAsciiSrcRec {
  ObjectPart    object;
  TextSrcPart	text_src;
  MyAsciiSrcPart	ascii_src;
} MyAsciiSrcRec;

#endif /* _myAsciiSrcP_h  --- Don't add anything after this line. */

/*
 * $Log: myAsciiSrcP.h,v $
 * Revision 2.0.1.1  1995/09/15 09:56:55  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:55  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/08/05  07:25:43  pfeifer
 * Release beta 04
 *
 * Revision 1.2  93/07/01  18:24:14  warnock
 * Fixed problem on SGIs under X11R4
 * 
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.1  92/06/22  10:43:39  jonathan
 * Initial revision
 * 
 */
