/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   Version 0.8
   Fri Feb  1 1991

   jonathan@Think.COM

*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_SCROLLLIST
#define _H_SCROLLLIST

typedef struct scrollList {
  Widget ListWidget;
  char* name;
  XtCallbackProc callback;
} _ScrollList, *ScrollList;

#endif
