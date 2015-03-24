/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: xwais.h,v $
 * Revision 2.0.1.1  1995/09/15 09:57:37  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:33  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/08/08  07:33:17  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.3  1994/08/05  08:54:30  pfeifer
 * dumped numtosave since it's unused
 *
 * Revision 1.2  1994/08/05  07:28:43  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.15  92/04/30  12:23:34  jonathan
 * Removed redundant includes.
 * 
 * Revision 1.14  92/04/28  15:34:12  jonathan
 * Cleaned up includes.  More to go I'm sure.
 * 
 * Revision 1.13  92/03/23  16:10:53  jonathan
 * named a lot of buttons.
 * 
 * Revision 1.12  92/03/17  14:21:20  jonathan
 * Generally cleaned up.
 * 
 */

#ifndef _H_XWAIS
#define _H_XWAIS


#include "xstuff.h"

#ifdef _IBMR2
#undef SYSV
#undef void
#endif
 
/* usefull definitions */

#define STRINGSIZE	256

#include "cdialect.h"
#include "wais.h"
#include "text.h"
#include "scrolllist.h"
#include "util.h"
#include "scommands.h"

#ifdef XWAIS
#include "display.h"
#include "tcommands.h"
#else
#include "xquestion.h"
#include "qdisplay.h"
#include "qcommands.h"
#include "filereq.h"
#endif

#ifdef MAIN
#define ext
#else
#define ext extern
#endif

#define NUM_CURSORS 7

ext Cursor wais_cursors[NUM_CURSORS];

ext char *command_name;

ext char *sdir, *cdir;

/*
ext char* wais_log_file_name;
ext FILE* waislogfile;
*/
#define NO_ITEM_SELECTED -1

#define NUMQLINES 3
#define NUMSLINES 3
#define NUMRLINES 6

/*	externals in xwais.c 	*/

ext struct _app_resources {
    char *questionDirectory;
    char *userSourceDirectory;
    char *commonSourceDirectory;
    char *documentDirectory;
    char *helpFile;
    char *removeSeekerCodes;
    char *rescanInterval;
    char *seedWords;
    char *initialSource;
    char *questionName;
    char *filters;
    int maximumResults;
    Boolean doSearch;
    Boolean defaultsInstalled;
    Boolean showKeys;
} app_resources;

ext XtAppContext app_context;

ext XtIntervalId rescantimerid;

ext Widget top, form; /* top level things for display */

ext Widget
  labelwindow,
  messwidget;

ext ScrollList
  questionwindow,
  sourcewindow,
  typewindow,
  savelist;

ext Widget questionpopup,
  qnamewid, keywordwid, sshell, typeshell,
  savereq, dirnamewidget, filenamewidget, maxdocwid;

ext Widget sourcepopup, prefpopup, maxdocwid, sourcepathwid,
  filterwid, snamewid, serverwid, servicewid, dbwid,
  costwid, unitwid, maintainerwid, descwid;

ext Widget searchButton, addSourceButton, delSourceButton,
  addDocButton, delDocButton, helpButton, abortButton, 
  saveAsButton, doneButton;

ext Widget savebutton, viewbutton, cancelbutton, quitbutton;

ext Widget sourcemenu, sourcebutton;

ext String questionname, config, db;

ext Display *CurDpy;

ext int NumQuestions;

ext Source the_Source;

#ifndef XWAIS
ext XQuestion the_Question;
#endif

ext String *Question_items;

ext SList Sources;
extern char **Source_items;
ext int NumSources;

ext char** Type_items;

ext TextList allText;

ext Widget LastClicked;
ext Boolean double_click;

ext void Feep();

extern void DoQuit();

#endif
