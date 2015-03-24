/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/qdisplay.c,v 2.0.1.1 1995/09/15 09:57:02 pfeifer Exp $";
#endif

#include "xwais.h"
#include "cursor.h"
#include "myAsciiSink.h"
#include "myAsciiSrc.h"
#include "xwaisq.bit"

void SetqIcon(parent)
Widget parent;
{
  Arg args[1];
  Pixmap icon_pixmap = None;

  XtSetArg (args[ZERO], XtNiconPixmap, &icon_pixmap);
  XtGetValues(parent, args, ONE);
  if (icon_pixmap == None) {
    XtSetArg(args[ZERO], XtNiconPixmap, 
	     XCreateBitmapFromData(XtDisplay(parent),
				   XtScreen(parent)->root,
				   xwaisq_bits, xwaisq_width, xwaisq_height));
    XtSetValues (parent, args, ONE);
  }
}

Widget
MakeQuestionPane(parent, question)
Widget parent;
XQuestion question;
{
  Widget frame, stringlabelwid, button, view;
  static   AsciiSinkObject sink = NULL;
  static String
    name = "";
  Arg args[THREE];
  static String items[] = {NULL};
  Question q = question->q;

  question->window->shell = parent;
  frame =
    XtCreateManagedWidget("questionpopupform", formWidgetClass,
			  question->window->shell, NULL, ZERO);

  SetqIcon(question->window->shell);

  stringlabelwid =
    MakeLabel(frame, "keywordlabel", "Tell me about:", NULL, NULL);

#ifdef MOTIF
  question->window->keywordwid =
    MakeStringBox(frame, "keywords", "", stringlabelwid, NULL);
#else
  XtSetArg(args[ZERO], XtNeditType, XawtextEdit);
  XtSetArg(args[ONE], XtNstring, "");
  XtSetArg(args[TWO], XtNfromVert, stringlabelwid);

  question->window->keywordwid =
    XtCreateManagedWidget("keywords", asciiTextWidgetClass, frame, args, THREE);

  if (sink == NULL) {
    sink = (AsciiSinkObject)
      XtCreateWidget("sink", myasciiSinkObjectClass, 
		     question->window->keywordwid, NULL, ZERO);

    XtSetArg(args[ZERO], XtNtextSink, sink);
    XtSetValues(question->window->keywordwid, args, ONE);
  }
#endif

  searchButton = MakeCommandButton(frame, "wsearch", DoSearch,
			     stringlabelwid, question->window->keywordwid, NULL);
  stringlabelwid =
    MakeLabel(frame, "qsourcelabel", "In Sources:",
	      NULL, NULL);

  XtSetArg (args[ZERO], XtNallowVert, VERTVAL);

  view = XtCreateManagedWidget ("sview", viewportWidgetClass,
				frame, args, ONE);

  question->window->Sources =
    MakeScrollList(view, "questionsource", question->Source_Items,
		   EditQuestionSource, NULL, NULL);

#ifndef MOTIF
  XtAddCallback(question->window->Sources->ListWidget, XtNcallback,
		SensitizeDelSource, NULL);
#endif

  stringlabelwid =
    MakeLabel(frame, "qdoclabel", "Similar to:",
	      question->window->keywordwid, NULL);

  view = XtCreateManagedWidget ("dview", viewportWidgetClass,
				frame, args, ONE);

  question->window->RelevantDocuments = 
    MakeScrollList(view, "questiondoc", question->Relevant_Items,
		   ViewRelevant, NULL, NULL);

#ifndef MOTIF
  XtAddCallback(question->window->RelevantDocuments->ListWidget, XtNcallback,
		SensitizeDelDoc, NULL);
#endif

  addSourceButton = button =
    MakeCommandButton(frame, "qaddsource", PopupSourceMenu,
		      view, NULL, NULL);
  delSourceButton = button =
    MakeCommandButton(frame, "qdeletesource", DeleteQuestionSource,
		      view, button, NULL);
  XtSetSensitive(button, SENSVAL);

  addDocButton = button =
    MakeCommandButton(frame, "qadddoc", AddResponseToQuestion,
		      view, NULL, NULL);
  XtSetSensitive(button, SENSVAL);

  delDocButton = button =
    MakeCommandButton(frame, "qdeletedoc", DeleteQuestionDoc,
		      view, button, NULL);

  XtSetSensitive(button, SENSVAL);

  stringlabelwid =
    MakeLabel(frame, "qreslabel", "Resulting",   /* Added space after g */
	      NULL, NULL);

  stringlabelwid =
    MakeLabel(frame, "qreslabel1", "documents:",
	      stringlabelwid, NULL);

  viewbutton = button = 
    MakeCommandButton(frame, "view", ViewResponse,
		      stringlabelwid, NULL, NULL);

  saveAsButton = button =
    MakeCommandButton(frame, "qsave", ViewResponse,
		      button, NULL, NULL);

  abortButton = button =
    MakeCommandButton(frame, "Abort", Abort,
		      button, NULL, NULL);

  button =
    MakeCommandButton(frame, "Prefs", Prefs,
		      button, NULL, NULL);

  helpButton = button =
    MakeCommandButton(frame, "help", XwaisHelp,
		      button, NULL, NULL);
  doneButton = button =
    MakeCommandButton(frame, "qdone", CloseQuestionEdit,
		      button, NULL, NULL);

  view = XtCreateManagedWidget ("rview", viewportWidgetClass,
				frame, args, ONE);

  question->window->ResultDocuments = 
    MakeScrollList(view, "questionresults", question->Result_Items,
		   ViewResponse, NULL, NULL);

#ifndef MOTIF
  XtAddCallback(question->window->ResultDocuments->ListWidget,XtNcallback,
		SensitizeAddDoc,(XtPointer)0);
#endif

  stringlabelwid =
    MakeLabel(frame, "statuslabel", "Status:",
	      NULL, NULL);

  XtSetArg(args[ZERO], XtNeditType, XawtextEdit);

  question->window->StatusWindow =
    XtCreateManagedWidget("statusWindow", asciiTextWidgetClass,
			  frame, args, ONE);
  XtSetKeyboardFocus(frame, question->window->keywordwid);
  return(question->window->shell);
}

Widget
MakeTextPopup(parent, t, name)
Widget parent;
Textbuff t;
char *name;
{
  Widget frame, button;
  Arg args[TWO];
  static String items[] = {NULL};
  AsciiSinkObject sink;
  AsciiSourceObject src;

  XtSetArg(args[ZERO], XtNtitle, name);
  XtSetArg(args[ONE], XtNiconName, name);
  t->shell = XtCreatePopupShell("textpopup", applicationShellWidgetClass,
				parent, args, TWO);
  frame =
    XtCreateManagedWidget("questionpopupform", formWidgetClass,
			  t->shell, NULL, ZERO);

  XtSetArg(args[ZERO], XtNeditType, XawtextRead);
  t->textwindow =
    XtCreateManagedWidget("textWindow", asciiTextWidgetClass, frame, args, ONE);

  sink = (AsciiSinkObject)
    XtCreateWidget("sink", myasciiSinkObjectClass, 
		   t->textwindow, NULL, ZERO);

  src = (AsciiSrcObject)
    XtCreateWidget("sink", myasciiSrcObjectClass, 
		   t->textwindow, NULL, ZERO);

  XtSetArg(args[ZERO], XtNtextSink, sink);
  XtSetArg(args[ONE], XtNtextSource, src);
  XtSetValues(t->textwindow, args, TWO);

  button = MakeCommandButton(frame, "findKeywords", showKeyword,
			     t->textwindow, NULL, NULL);
  button = MakeCommandButton(frame, "addSection",addSection,
			     t->textwindow, button, NULL);
  button = MakeCommandButton(frame, "showNext", showNext,
			     t->textwindow, button, (XtPointer)t);
  button = MakeCommandButton(frame, "showPrevious", showPrevious,
			     t->textwindow, button, (XtPointer)t);
  button = MakeCommandButton(frame, "filesave", SaveText,
			     t->textwindow, button, NULL);
  button = MakeCommandButton(frame, "tdone", EndText,
			     t->textwindow, button, (XtPointer)t);

  MakeLabel(frame, "textstatuslabel", "Status:", button, NULL);

  XtSetArg(args[ZERO], XtNeditType, XawtextEdit);

  t->status =
    XtCreateManagedWidget("textStatus", asciiTextWidgetClass, frame, args, ONE);

  SettIcon(t->shell);

  XtPopup(t->shell, XtGrabNone);

  return t->textwindow;
}

QuestionWindow
MakeQuestionForm(question)
XQuestion question;
{
  QuestionWindow result;

  if((result = (QuestionWindow) s_malloc(sizeof(_QuestionWindow))) == NULL) {
    XwaisPrintf("Unable to allocate space for new window.\n");
    return(NULL);
  }

  question->window = result;
  question->q->CurrentSource = NO_ITEM_SELECTED;
  question->q->CurrentRelDoc = NO_ITEM_SELECTED;
  question->q->CurrentResDoc = NO_ITEM_SELECTED;

  result->shell = MakeQuestionPane(top, question);

  ReplaceText(result->keywordwid, question->q->keywords);

  return(result);
}

Widget
MakeSourcePopup(parent)
Widget parent;
{
  Arg args[THREE];

  Widget shell, frame, labelwid, stringlabelwid, button, tmp;
  WidgetClass wclass;
  static String
    namestring,
  serverstring,
  servicestring,
  dbstring,
  cost, units,
  maintainer,
  description;
  int CurrentSource;
  static   AsciiSinkObject sink = NULL;

  namestring = "default";
  serverstring = "localhost";
  servicestring = "8000";
  dbstring = "";
  cost = "";
  units = "";
  maintainer = "";
  description = "";

  shell = XtCreatePopupShell("sourcepopup", applicationShellWidgetClass,
			     parent, NULL, ZERO);
  frame = XtCreateManagedWidget("sourcepopupform", formWidgetClass,
				shell, NULL, ZERO);

  stringlabelwid = MakeLabel(frame, "sourcenamelabel", "       Name:", NULL, NULL);
  tmp = snamewid = MakeStringBox(frame, "sourcename", namestring, NULL, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "serverlabel",     "     Server:", tmp, NULL);
  tmp = serverwid = MakeStringBox(frame, "server", serverstring, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "servicelabel", "    Service:", tmp, NULL);
  tmp = servicewid = MakeStringBox(frame, "service", serverstring, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "dblabel",      "   Database:", tmp, NULL);
  tmp = dbwid = MakeStringBox(frame, "database", dbstring, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "costlabel",    "       Cost:", tmp, NULL);
  tmp = costwid = MakeStringBox(frame, "cost", cost, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "unitlabel",    "      Units:", tmp, NULL);
  tmp = unitwid = MakeStringBox(frame, "costunit", units, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "maintlabel",   " Maintainer:", tmp, NULL);
  tmp = maintainerwid = MakeStringBox(frame, "maintainer", maintainer, tmp, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "desclabel",    "Description:", tmp, NULL);

  XtSetArg(args[ZERO], XtNeditType, XawtextEdit);
  XtSetArg(args[ONE], XtNstring, description);
  XtSetArg(args[TWO], XtNfromVert, stringlabelwid);

  tmp = descwid = 
    XtCreateManagedWidget("description", asciiTextWidgetClass, frame, args, THREE);

  if (sink == NULL) {
    sink = (AsciiSinkObject)
      XtCreateWidget("sink", myasciiSinkObjectClass, 
		     descwid, NULL, ZERO);

    XtSetArg(args[ZERO], XtNtextSink, sink);
    XtSetValues(descwid, args, ONE);
  }

  button = MakeCommandButton(frame, "sdone", DoSSave, tmp, NULL, NULL);
  MakeCommandButton(frame, "cancel", CancelSourceEdit, tmp, button, NULL);

  return shell;
}

Widget
MakePrefPopup(parent)
Widget parent;
{
  Widget shell, frame, labelwid, stringlabelwid, button;
  WidgetClass wclass;
  static String maxDocs = "", sourcepath = "", filters = "";
  
  shell = XtCreatePopupShell("prefpopup", applicationShellWidgetClass,
			     parent, NULL, ZERO);
  frame = XtCreateManagedWidget("prefpopupform", formWidgetClass,
				shell, NULL, ZERO);

  labelwid = MakeLabel(frame, "preflabel", "Set Preferences", NULL, NULL);

  stringlabelwid = MakeLabel(frame, "prefmaxdoclabel",
			     "   Maximum Results:", labelwid, NULL);
  maxdocwid = MakeStringBox(frame, "maxdoc", maxDocs, labelwid, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "sourcepathlabel",	
			     "Source Search Path:", maxdocwid, NULL);
  sourcepathwid = MakeStringBox(frame, "sourcepath", sourcepath, maxdocwid, stringlabelwid);

  stringlabelwid = MakeLabel(frame, "filterlabel",	
			     "           Filters:", sourcepathwid, NULL);
  filterwid = MakeStringBox(frame, "filters", filters, sourcepathwid, stringlabelwid);

  button = MakeCommandButton(frame, "pdone", DoPSave, filterwid, NULL, NULL);
  MakeCommandButton(frame, "pcancel", DontPSave, filterwid, button, NULL);

  return shell;
}

ScrollList
MakeSourceFrame(parent)
Widget parent;
{
  Widget frame, labelwid, button, view;
  ScrollList result;
  Arg args[TWO];

  XtSetArg(args[ZERO], XtNtitle, "Sources");
  XtSetArg(args[ONE], XtNiconName, "Sources");
  sshell = XtCreatePopupShell("sourcepane", transientShellWidgetClass, 
			      parent, args, TWO);
  frame = XtCreateManagedWidget("sources", formWidgetClass, sshell, NULL, ZERO);

  XtSetArg (args[ZERO], XtNallowVert, VERTVAL);

  view = XtCreateManagedWidget ("ssview", viewportWidgetClass,
				frame, args, ONE);

  result =
    MakeScrollList(view, "sourceWindow", Source_items, 
		   AddSourceToQuestion, NULL, NULL);

  button = MakeCommandButton(frame, "qsadd", AddSourceToQuestion,
			     view, NULL, NULL);
  return result;
}

ScrollList
MakeTypeFrame(parent)
Widget parent;
{
  Widget frame, labelwid, button, view;
  ScrollList result;
  Arg args[TWO];

  XtSetArg(args[ZERO], XtNtitle, "Types");
  XtSetArg(args[ONE], XtNiconName, "Types");
  typeshell = XtCreatePopupShell("typepane", transientShellWidgetClass, 
			      parent, args, TWO);
  frame = XtCreateManagedWidget("types", formWidgetClass, typeshell, NULL, ZERO);

  XtSetArg (args[ZERO], XtNallowVert, True);

  view = XtCreateManagedWidget ("typeview", viewportWidgetClass,
				frame, args, ONE);

  result =
    MakeScrollList(view, "typeWindow", Type_items, doType, NULL, NULL);

  button = MakeCommandButton(frame, "None", doType,
			     view, NULL, NULL);
  return result;
}

void
MakeWaisCursors()
{
  int i;
  XColor fore, back;
  Pixmap cursor_pixmap, mask;

  back.red = 65535;
  back.green = 65535;
  back.blue = 65535;

  fore.red = 0;
  fore.green = 0;
  fore.blue = 0;

  mask = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor_mask, cursor_width, cursor_height);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor0_bits, cursor_width, cursor_height);
  wais_cursors[0] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);

  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor1_bits, cursor_width, cursor_height);
  wais_cursors[1] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor2_bits, cursor_width, cursor_height);
  wais_cursors[2] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor3_bits, cursor_width, cursor_height);
  wais_cursors[3] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor4_bits, cursor_width, cursor_height);
  wais_cursors[4] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor5_bits, cursor_width, cursor_height);
  wais_cursors[5] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
  cursor_pixmap = XCreateBitmapFromData(XtDisplay(top),
					XtScreen(top)->root,
					cursor6_bits, cursor_width, cursor_height);
  wais_cursors[6] = XCreatePixmapCursor(CurDpy, cursor_pixmap, mask, &fore, &back, 0, 0);
}

/*
 *
 * $Log: qdisplay.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:02  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:02  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1995/01/10  17:06:47  pfeifer
 * Better layout and colors?
 *
 * Revision 1.3  1994/12/23  16:43:07  pfeifer
 * Change label to make layout correct
 *
 * Revision 1.2  1994/08/05  07:26:17  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.12  92/06/22  10:50:53  jonathan
 * Put use of myAsciiSink and myAsciiSrc here.
 * 
 * Revision 1.11  92/06/15  13:50:53  jonathan
 * Removed ShowKey button from text popups.
 * 
 * Revision 1.10  92/04/28  15:29:31  jonathan
 * Cleaned up ANSI specs on most function calls.
 * 
 * Revision 1.9  92/03/23  16:11:59  jonathan
 * added save requester.
 * 
 * Revision 1.8  92/03/17  14:16:40  jonathan
 * Added cursor support.
 * 
*/
