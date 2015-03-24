/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) 
 *
 *
 * $Log: filereq.h,v $
 * Revision 2.0.1.1  1995/09/15 09:56:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:37  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:24:20  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.2  92/04/30  12:23:12  jonathan
 * Added prototype for clearReqButtons.
 * 
 * Revision 1.1  92/03/25  18:52:01  jonathan
 * Initial revision
 * 
 */

#ifndef _H_FILEREQ
#define _H_FILEREQ

void SetFileName _AP((Widget w,XtPointer closure, XtPointer call_data));
void SetDir _AP((Widget w,XtPointer closure, XtPointer call_data));
void SetReqButtons _AP((boolean how));
ScrollList MakeSaveRequester _AP((Widget parent));

#endif
