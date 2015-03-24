/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: tcommands.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:14  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:12  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:27:06  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.6  92/04/28  15:31:18  jonathan
 * Cleaned up use of scrollList.
 * 
 * Revision 1.5  92/03/17  14:17:18  jonathan
 * Generally cleaned up.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/tcommands.c,v 2.0.1.1 1995/09/15 09:57:14 pfeifer Exp $";
#endif

#define _C_TCOMMANDS
#define XWAIS
#include "xwais.h"

static Boolean editting_new_question;
static int last_question = NO_ITEM_SELECTED;
extern int last_source;

int get_selected_question()
{
  return(get_selected_item(questionwindow->ListWidget));
}

/* ARGSUSED */
void
EditQuestion(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char msg[STRINGSIZE];
  int CurrentQuestion;

  double_click = FALSE;
  LastClicked = w;

  if((CurrentQuestion = get_selected_question())
     != NO_ITEM_SELECTED) {
#ifdef MOTIF
    last_question = CurrentQuestion;
#endif
    if (CurrentQuestion == last_question) {
      sprintf(msg, "Opening question: %s\n", Question_items[CurrentQuestion]);
      XwaisPrintf(msg);
      sprintf(msg, "xwaisq \"%s\" &", Question_items[CurrentQuestion]);
      system(msg);
    }
    else last_question = CurrentQuestion;
  }
  else {
    last_question = -1;
    if (NumQuestions > 0)
      XwaisPrintf("No question selected.\nPlease select one and try again.\n");
    else {
      XwaisPrintf("Opening new question\n");
      system("xwaisq &");
    }
  }
}

/* ARGSUSED */
void
AddQuestion(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  Question new;

  double_click = FALSE;
  LastClicked = w;

  XwaisPrintf("Opening new question\n");
  system("xwaisq &");
}

void
DeleteQuestion(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char msg[STRINGSIZE];
  int CurrentQuestion;

  double_click = FALSE;
  LastClicked = w;

  if((CurrentQuestion = get_selected_question())
     != NO_ITEM_SELECTED) {
    sprintf(msg, "%s%s",
	    app_resources.questionDirectory,
	    Question_items[CurrentQuestion]);
    if(unlink(msg) != 0)
      XwaisPrintf("Error delete Question.\n");

    last_question = NO_ITEM_SELECTED;
    ScanDirs(NULL, NULL);
  }
  else
    XwaisPrintf("No question selected.\nPlease select one and try again.\n");
}

void
DeleteSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char msg[STRINGSIZE];

  double_click = FALSE;
  LastClicked = w;

  if(get_selected_source() != NO_ITEM_SELECTED) {
    sprintf(msg, "%s%s",
	    app_resources.userSourceDirectory,
	    Source_items[get_selected_source()]);

    if (unlink(msg) != 0)
      XwaisPrintf("Error deleting source.\n");
    else {
      last_source = NO_ITEM_SELECTED;
      XawListUnhighlight(sourcewindow->ListWidget);
    }
  }
  else
    XwaisPrintf("No source selected.\nPlease select one and try again.\n");
}
