/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: text.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:21  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:17  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:27:26  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.2  92/03/17  14:17:54  jonathan
 * Prototyped functions.
 * 
 */

#ifndef _H_TEXT
#define _H_TEXT

typedef struct textbuff {
  Widget	shell;
  Widget	textwindow;
  Widget	status;
  DocumentID 	docid;
  char *	text;
  char *	type;
  long 		size;
} _Textbuff, *Textbuff;

typedef struct textlist {
  Textbuff thisText;
  struct textlist *nextText;
} _TextList, *TextList;

/* functions */

TextList NewText _AP((void));
void KillText _AP((Textbuff t));
Textbuff findText _AP((Widget w));
Textbuff findTextDoc _AP((DocumentID doc, char* type));

#endif
