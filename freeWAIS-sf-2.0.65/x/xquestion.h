/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/* $Log: xquestion.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:30  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:23  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:31:02  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.4  92/03/17  14:19:56  jonathan
 * Separated from question.c.
 * 
 */

#ifndef _H_XQUESTION
#define _H_XQUESTION

#include "question.h"

typedef struct QuestionWindow {
  Widget shell;
  Widget keywordwid;
  ScrollList Sources;
  ScrollList RelevantDocuments;
  ScrollList ResultDocuments;
  Widget StatusWindow;
  Widget Textwindow;
  char *textbuffer;
} _QuestionWindow, *QuestionWindow;

typedef struct xquestion{
  Question q;
  char **Source_Items;
  char **Relevant_Items;
  char **Result_Items;
  QuestionWindow window;
} _XQuestion, *XQuestion;

/* function definitions */

XQuestion findQuestionWindow();
QuestionWindow MakeQuestionWindow();
void DeleteQuestionWindow();
char **buildQuestionItemList();
DocList build_response_list();
void SearchWais();
boolean init_for_source();
DocumentID getNextorPrevDoc();

#endif
