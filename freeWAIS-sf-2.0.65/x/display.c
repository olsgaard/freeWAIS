/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: display.c,v $
 * Revision 2.0.1.1  1995/09/15 09:56:30  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:06:31  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:30:57  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.5  92/04/28  15:25:21  jonathan
 * Changed use of Scrolllist.
 * 
 * Revision 1.4  92/03/17  14:16:13  jonathan
 * Generally cleaned things up.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/display.c,v 2.0.1.1 1995/09/15 09:56:30 pfeifer Exp $";
#endif

#define XWAIS
#include "xwais.h"

static ScrollList
MakeQuestionFrame(parent)
Widget parent;
{
  Widget frame, labelwid, button;
  Widget view;
  ScrollList result;
  Arg arglist[TWO];
  Cardinal num_args;

  frame = XtCreateManagedWidget("questions", formWidgetClass, parent, NULL, ZERO);

  labelwid = MakeLabel(frame, "questionlabel", "Questions:", NULL, NULL);

  num_args = 0;
#ifndef MOTIF
  XtSetArg (arglist[ZERO], XtNforceBars, True);
  XtSetArg (arglist[ONE], XtNallowVert, True);
  num_args = 2;
#endif
  view = XtCreateManagedWidget ("qview", viewportWidgetClass,
				frame, arglist, num_args);

  result =
    MakeScrollList(view, "questionWindow", Question_items, 
		   EditQuestion, NULL, NULL);

  button = MakeCommandButton(frame, "qadd", AddQuestion,
			     view, NULL, NULL);
  button = MakeCommandButton(frame, "qedit", EditQuestion,
			     view, NULL, NULL);
  button = MakeCommandButton(frame, "qdelete", DeleteQuestion,
			     view, NULL, NULL);
  return result;
}

static ScrollList
MakeSourceFrame(parent)
Widget parent;
{
  Widget frame, labelwid, button, view;
  ScrollList result;
  Arg arglist[TWO];
  Cardinal num_args;

  frame = XtCreateManagedWidget("tsources", formWidgetClass, parent, NULL, ZERO);

  labelwid = MakeLabel(frame, "sourcelabel", "Sources:", NULL, NULL);

  num_args = 0;
#ifndef MOTIF
  XtSetArg (arglist[ZERO], XtNforceBars, True);
  XtSetArg (arglist[ONE], XtNallowVert, True);
  num_args = 2;
#endif
  view = XtCreateManagedWidget ("tsview", viewportWidgetClass,
				frame, arglist, num_args);

  result =
    MakeScrollList(view, "sourceWindow", Source_items, 
		   EditSource, NULL, NULL);

  button = MakeCommandButton(frame, "sadd", AddSource,
			     view, NULL, NULL);
  button = MakeCommandButton(frame, "sedit", EditSource,
			     view, NULL, NULL);
  button = MakeCommandButton(frame, "sdelete", DeleteSource,
			     view, NULL, NULL);
  return result;
}

static Widget
MakeMessage(parent)
Widget parent;
{
  Widget stringlabelwid;
  Arg arglist[10];
  Cardinal num_args;

  stringlabelwid = MakeLabel(parent, "mainstatuslabel", "Status:",
			     NULL, NULL);
  num_args = 0;
  XtSetArg(arglist[num_args], XtNeditType, XawtextEdit); num_args++;
  XtSetArg(arglist[num_args], XtNsensitive, False); num_args++;
  XtSetArg(arglist[num_args], XtNfromVert, stringlabelwid); num_args++;
  return XtCreateManagedWidget("messageWindow", asciiTextWidgetClass,
			       parent, arglist, num_args);
}

static Widget
MakeSourcePopup(parent)
Widget parent;
{
  Arg args[THREE];
  Widget shell, frame, labelwid, stringlabelwid, button, tmp;
  static String
    namestring,
    serverstring,
    servicestring,
    dbstring,
    cost, units,
    maintainer,
    description;
  int CurrentSource;
  Source source;

  CurrentSource = get_selected_source();

  if (CurrentSource != NO_ITEM_SELECTED) {
    source = findSource(CurrentSource);
    namestring = source->name;
    serverstring = source->server;
    servicestring = source->service;
    dbstring = source->database;
    cost = source->cost;
    units = source->units;
    maintainer = source->maintainer;
    description = source->description;
  } else {
    namestring = "default";
    serverstring = "localhost";
    servicestring = "8000";
    dbstring = "";
    cost = "";
    units = "";
    maintainer = "";
    description = "";
  }

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

  button = MakeCommandButton(frame, "sdone", CloseSourceEdit, tmp, NULL, NULL);
  MakeCommandButton(frame, "cancel", CancelSourceEdit, tmp, button, NULL);

  return shell;
}

Widget
SetupWaisDisplay(parent)
Widget parent;
{
  Widget form, button;

  form = XtCreateManagedWidget ("form", formWidgetClass,
				parent, NULL, ZERO);

  questionwindow = MakeQuestionFrame(form);

  sourcewindow = MakeSourceFrame(form);

  button = MakeCommandButton(form, "help", XwaisHelp,
			     NULL, NULL, NULL);

  MakeCommandButton(form, "quit", DoQuit, NULL, button, NULL);

  messwidget = MakeMessage(form);

  sourcepopup = MakeSourcePopup(top);

  return form;
}
