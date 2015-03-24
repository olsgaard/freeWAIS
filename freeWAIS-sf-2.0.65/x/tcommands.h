/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: tcommands.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:17  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:14  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:27:08  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.3  92/03/17  14:17:33  jonathan
 * Prototyped everything!
 * 
 */

#ifndef _H_TCOMMANDS
#define _H_TCOMMANDS

int get_selected_question _AP((void));
void EditQuestion _AP((Widget w,XtPointer closure, XtPointer call_data));
void AddQuestion _AP((Widget w,XtPointer closure, XtPointer call_data));
void DeleteQuestion _AP((Widget w,XtPointer closure, XtPointer call_data));
void DeleteSource _AP((Widget w,XtPointer closure, XtPointer call_data));
#endif
