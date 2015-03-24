/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*
 * $Log: qdisplay.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:05  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:04  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:26:24  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.4  92/04/28  15:29:47  jonathan
 * More prototypes.
 * 
 * Revision 1.3  92/03/23  16:09:38  jonathan
 * Added MakeSaveRequester.
 * 
 * Revision 1.2  92/03/17  14:13:03  jonathan
 * *** empty log message ***
 * 
*/

#ifndef _H_QDISPLAY
#define _H_QDISPLAY

void SetqIcon _AP((Widget parent));
void SettIcon _AP((Widget parent));
Widget MakeQuestionPane _AP((Widget parent,XQuestion question));
Widget MakeTextPopup _AP((Widget parent,Textbuff t,char *name));
QuestionWindow MakeQuestionForm _AP((XQuestion question));
Widget MakeSourcePopup _AP((Widget parent));
Widget MakePrefPopup _AP((Widget parent));
ScrollList MakeSourceFrame _AP((Widget parent));
ScrollList MakeTypeFrame _AP((Widget parent));
void MakeWaisCursors _AP((void));
#endif

