/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef _H_QCOMMANDS
#define _H_QCOMMANDS

typedef struct _sbuff {
  Source s;
  char* buffer;
  long buffer_offset;
  long toRead;
  long read_state;
  long page;
  long chars_per_page;
  long doc_size;
  Textbuff textstruct;
  XtInputId xid; /* Input Id from XtAddInput */
} _Sbuff, *Sbuff;

XawTextPosition findstring _AP((char* text,char* string,Boolean casesensitive));
void fuzzButtons _AP((boolean fuzz));
void Abort _AP((Widget w, XtPointer closure,XtPointer call_data));
void Prefs _AP((Widget w, XtPointer closure,XtPointer call_data));
void DoPSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void DontPSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void DoSearch _AP((Widget w, XtPointer closure,XtPointer call_data));
void CloseQuestionEdit _AP((Widget w, XtPointer closure,XtPointer call_data));
void AddResponseToQuestion _AP((Widget w, XtPointer closure,XtPointer call_data));
void AddDocToQuestion _AP((Widget w, XtPointer closure,XtPointer call_data));
void DeleteQuestionDoc _AP((Widget w, XtPointer closure,XtPointer call_data));
void PopupSourceMenu _AP((Widget w, XtPointer closure,XtPointer call_data));
void AddSourceToQuestion _AP((Widget w, XtPointer closure,XtPointer call_data));
void EditQuestionSource _AP((Widget w, XtPointer closure,XtPointer call_data));
void DeleteQuestionSource _AP((Widget w, XtPointer closure,XtPointer call_data));
void doType _AP((Widget w, XtPointer closure,XtPointer call_data));
void SensitizeDelSource _AP((Widget w,XtPointer closure,XtPointer call_data));
void SensitizeAddDoc _AP((Widget w,XtPointer closure,XtPointer call_data));
void SensitizeDelDoc _AP((Widget w,XtPointer closure,XtPointer call_data));
void ViewDoc _AP((DocumentID doc,char* type,long size,Boolean saveit));
void ViewResponse _AP((Widget w, XtPointer closure,XtPointer call_data));
void ViewRelevant _AP((Widget w, XtPointer closure,XtPointer call_data));
void EndText _AP((Widget w, XtPointer closure,XtPointer call_data));
void showKeyword _AP((Widget w, XtPointer closure,XtPointer call_data));
void SaveText _AP((Widget w, XtPointer closure,XtPointer call_data));
void DoSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void DoTSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void DontTSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void setSourceMenu _AP((void));
void addSection _AP((Widget w, XtPointer closure,XtPointer call_data));
void DoSSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void DontSSave _AP((Widget w, XtPointer closure,XtPointer call_data));
void showNext _AP((Widget w, XtPointer closure,XtPointer call_data));
void showPrevious _AP((Widget w, XtPointer closure,XtPointer call_data));
void setFile _AP((Widget w, XtPointer closure,XtPointer call_data));
void quitFile _AP((Widget w, XtPointer closure,XtPointer call_data));
char* GetKeywordsUsed _AP((void));
#endif

/*
 * $Log: qcommands.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:00  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:00  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:26:01  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.8  92/06/22  10:53:06  jonathan
 * Added prototype for GetKeywordsUsed.
 * 
 * Revision 1.7  92/06/15  13:53:52  jonathan
 * Added prototype for findkey (now public, for use by myAsciiSink.c)
 * 
 * Revision 1.6  92/04/28  15:29:00  jonathan
 * More prototypes.
 * 
 * Revision 1.5  92/03/23  16:09:10  jonathan
 * Added prototype for unfuzzButtons
 * 
 * Revision 1.4  92/03/17  14:24:41  jonathan
 * Prototyped everything.
 * 
 */
