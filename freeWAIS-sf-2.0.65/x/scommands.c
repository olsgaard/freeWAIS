/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*
 * $Log: scommands.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:07  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:06  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:26:41  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.9  92/05/19  09:26:58  jonathan
 * Fixed bug in CancelSourceEdit where new source caused core dump.
 * 
 * Revision 1.8  92/04/28  15:30:17  jonathan
 * Cleaned up use of scrollists.
 * 
 * Revision 1.7  92/03/17  14:17:00  jonathan
 * Generally cleaned up.
 * 
*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/scommands.c,v 2.0.1.1 1995/09/15 09:57:07 pfeifer Exp $";
#endif

#define _C_SCOMMANDS
#define XWAIS
#include "xwais.h"

int last_source = NO_ITEM_SELECTED;

static Source edit_source = NULL;

int get_selected_source()
{
  return(get_selected_item(sourcewindow->ListWidget));
}

/* widget commands */

/* ARGSUSED */
void
EditSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  SList s;
  char msg[1000];
  int CurrentSource;

  double_click = FALSE;
  LastClicked = w;

  if ((CurrentSource = get_selected_source())
      != NO_ITEM_SELECTED) {
#ifdef MOTIF
    last_source = CurrentSource;
#endif
    if (last_source == CurrentSource) {
      if(edit_source == NULL) 
	edit_source = (Source)s_malloc(sizeof(_Source));

      if(!ReadSourceFile(edit_source, 
			 Source_items[CurrentSource],
			 app_resources.userSourceDirectory))
	ReadSourceFile(edit_source, 
		       Source_items[CurrentSource],
		       app_resources.commonSourceDirectory);

      sprintf(msg, "Opening source: %s\n", edit_source->name);
      XwaisPrintf(msg);
      PopupSource(edit_source);
    }
    else last_source = CurrentSource;
  } 
  else {
    last_source = -1;
    XwaisPrintf("No source selected.\nPlease selected one and try again.\n");
  }
}

/* ARGSUSED */
void
AddSource(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{

  double_click = FALSE;
  LastClicked = w;

  if(edit_source == NULL) 
    edit_source = (Source)s_malloc(sizeof(_Source));

  edit_source->name = s_strdup("new.src");
  edit_source->directory = s_strdup(app_resources.userSourceDirectory);
  strncpy(edit_source->server, "localhost", STRINGSIZE);
  strncpy(edit_source->service, "8000", STRINGSIZE);
  strncpy(edit_source->database, "database_name", STRINGSIZE);
  edit_source->maintainer = s_strdup("");
  edit_source->description = s_strdup("");
  XwaisPrintf("Opening new source\n");
  PopupSource(edit_source);
}

void PopupSource(s)
Source s;
{
  Arg arglist[10];
  Cardinal num_args;

  if (s != NULL) {
    num_args = 0;
    XtSetArg(arglist[num_args], XtNtitle, s->name); num_args++;
    XtSetValues(sourcepopup, arglist, num_args);
    ReplaceText(snamewid, s->name);
    ReplaceText(serverwid, s->server);
    ReplaceText(servicewid, s->service);
    ReplaceText(dbwid, s->database);
    ReplaceText(costwid, s->cost);
    ReplaceText(unitwid, s->units);
    ReplaceText(maintainerwid, s->maintainer);
    if(s->description == NULL) s->description = s_strdup("");
    ReplaceAsciiText(descwid, s->description);
    XtPopup(sourcepopup, XtGrabNone);
  }
}

/* ARGSUSED */
void CloseSourceEdit(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char name[STRINGSIZE];

  double_click = FALSE;
  LastClicked = w;

  if (edit_source != NULL) {
    strcpy(name, GetString(snamewid));

    if(!((strlen(name) > 4) && 
	 strstr(name, ".src") &&
	 (!strcmp(".src", strstr(name, ".src")))))
      strcat(name, ".src");

    if(edit_source->name != NULL) s_free(edit_source->name);
    edit_source->name = s_strdup(name);

    if (edit_source->maintainer != NULL) s_free(edit_source->maintainer);
    edit_source->maintainer = s_strdup(GetString(maintainerwid));

    if (edit_source->description != NULL) s_free(edit_source->description);
    edit_source->description = s_strdup(GetAsciiString(descwid));

    strncpy(edit_source->server, GetString(serverwid), STRINGSIZE);
    strncpy(edit_source->service, GetString(servicewid), STRINGSIZE);
    strncpy(edit_source->database, GetString(dbwid), STRINGSIZE);
    strncpy(edit_source->cost, GetString(costwid), STRINGSIZE);
    strncpy(edit_source->units, GetString(unitwid), STRINGSIZE);

    WriteSource(app_resources.userSourceDirectory, edit_source, TRUE);

    /* ScanDirs(NULL, NULL);*/
  }
  else XwaisPrintf("No currently selected source.  This shouldn't happen.\n");
  XtPopdown(sourcepopup);
}

/* ARGSUSED */
void CancelSourceEdit(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  SList s;

  double_click = FALSE;
  LastClicked = w;

  XtPopdown(sourcepopup);
}
