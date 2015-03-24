/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   Version 0.82
   Tue Apr 30 1991

   jonathan@Think.COM

*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef _H_SCOMMANDS
#define _H_SCOMMANDS

extern void EditSource();
extern void AddSource();
extern void WriteSourcetoFile();
extern void CloseSourceEdit();
extern void CancelSourceEdit();
extern void PopupSource();
extern void sVerticalScroll();
extern void sVerticalJump();
extern void setSourceMenu();

#endif
