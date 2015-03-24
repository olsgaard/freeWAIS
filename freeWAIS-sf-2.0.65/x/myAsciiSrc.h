/* Borrowed by Jonny Goldman. */
/*
 * $XConsortium: AsciiSrc.h,v 1.13 91/07/22 19:02:47 converse Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */



/*
 * AsciiSrc.h - Public Header file for Ascii Text Source.
 *
 * This is the public header file for the Ascii Text Source.
 * It is intended to be used with the Text widget, the simplest way to use
 * this text source is to use the AsciiText Object.
 *
 * Date:    June 29, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium 
 *          kit@expo.lcs.mit.edu
 */


#ifndef _MyAsciiSrc_h
#define _MyAsciiSrc_h

#ifdef sgi
#include <X11/Xaw/TextI.h>
#endif
#include <X11/Xaw/TextSrc.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 callback	     Callback		Callback	(none)
 dataCompression     DataCompression	Boolean		True
 length		     Length		int		(internal)
 pieceSize	     PieceSize		int		BUFSIZ
 string		     String		String		NULL
 type		     Type		XawAsciiType	XawAsciiString
 useStringInPlace    UseStringInPlace	Boolean		False

*/
 
/* Class record constants */

extern WidgetClass myasciiSrcObjectClass;

typedef struct _MyAsciiSrcClassRec *MyAsciiSrcObjectClass;
typedef struct _MyAsciiSrcRec      *MyAsciiSrcObject;

/*
 * Just to make people's lives a bit easier.
 */

#define MyAsciiSourceObjectClass MyAsciiSrcObjectClass
#define MyAsciiSourceObject      MyAsciiSrcObject

/*
 * Resource Definitions.
 */

#define XtCDataCompression "DataCompression"
#define XtCPieceSize "PieceSize"
#define XtCType "Type"
#define XtCUseStringInPlace "UseStringInPlace"

#define XtNdataCompression "dataCompression"
#define XtNpieceSize "pieceSize"
#define XtNtype "type"
#define XtNuseStringInPlace "useStringInPlace"

#define XtRAsciiType "AsciiType"

#define XtEstring "string"
#define XtEfile "file"

typedef enum /* {XawAsciiFile, XawAsciiString} */ XawAsciiType;

/************************************************************
 *
 * Public routines 
 *
 ************************************************************/

/*	Function Name: XawAsciiSourceFreeString
 *	Description: Frees the string returned by a get values call
 *                   on the string when the source is of type string.
 *	Arguments: w - the AsciiSrc object.
 *	Returns: none.
 */

extern void MyAsciiSourceFreeString(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

/*	Function Name: MyAsciiSave
 *	Description: Saves all the pieces into a file or string as required.
 *	Arguments: w - the asciiSrc Object.
 *	Returns: TRUE if the save was successful.
 */

extern Boolean MyAsciiSave(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

/*	Function Name: MyAsciiSaveAsFile
 *	Description: Save the current buffer as a file.
 *	Arguments: w - the asciiSrc object.
 *                 name - name of the file to save this file into.
 *	Returns: True if the save was successful.
 */

extern Boolean MyAsciiSaveAsFile(
#if NeedFunctionPrototypes
    Widget		/* w */,
#ifdef sgi
    String
#else
    _Xconst char*	/* name */
#endif
#endif 
);

/*	Function Name: MyAsciiSourceChanged
 *	Description: Returns true if the source has changed since last saved.
 *	Arguments: w - the asciiSource object.
 *	Returns: a Boolean (see description).
 */

extern Boolean MyAsciiSourceChanged(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

#endif /* _MyAsciiSrc_h  - Don't add anything after this line. */

/*
 * $Log: myAsciiSrc.h,v $
 * Revision 2.0.1.1  1995/09/15 09:56:53  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:53  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/08/05  07:25:34  pfeifer
 * Release beta 04
 *
 * Revision 1.2  93/07/01  18:22:35  warnock
 * Fixed problem on SGI's under X11R4
 * 
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.1  92/06/22  10:46:14  jonathan
 * Initial revision
 * 
 */
