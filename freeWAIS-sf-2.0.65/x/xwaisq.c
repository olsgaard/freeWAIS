/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: xwaisq.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:41  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:41  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:29:04  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.22  92/06/15  13:51:38  jonathan
 * Changed call to fuzzbuttons.
 * 
 * Revision 1.21  92/06/03  17:09:00  jonathan
 * Changed Scandirs to leave listwidgets where they are.  Less confusing.
 * 
 * Revision 1.20  92/04/28  15:34:58  jonathan
 * Cleaned up includes.  Replaced references to directory routines with
 * scandir.
 * 
 * Revision 1.19  92/03/23  16:11:30  jonathan
 * Added stuff to set button sensitivity.
 * 
 * Revision 1.18  92/03/17  15:08:23  jonathan
 * Typed ExitCommand.
 * 
 * Revision 1.17  92/03/17  14:22:32  jonathan
 * Generally cleaned up.
 * 
 * Revision 1.16  92/03/07  19:39:23  jonathan
 * Fixed argument to ScanDirs.
 * 
 * Revision 1.15  92/03/06  15:01:51  jonathan
 * Moved freopen to right before main loop.
 * 
 * Revision 1.14  92/03/06  14:49:19  jonathan
 * New and Improved source loading!  Also rescans source directory if it
 * changes.
 * 
 * Revision 1.13  92/03/01  14:04:06  jonathan
 * Added command_name to main.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/xwaisq.c,v 2.0.1.1 1995/09/15 09:57:41 pfeifer Exp $";
#endif

#define XWAISQ_C
#define MAIN
#include "xwais.h"
#include <sys/stat.h>

/* Dummy function added so we can link to wais-ui.c.  swais uses the
 * real version of this routine, which is in swais.c, but it's called
 * in here -- warnock@hypatia.gsfc.nasa.gov, 7/21/93
 */
void
DumpTextToScreen(buffer, size)
char *buffer;
long size;
{
}   

#define offset(field) XtOffset(struct _app_resources*, field)
static XtResource resources[] = {
   {"questionDirectory", "QuestionDirectory", XtRString, sizeof(char *),
         offset(questionDirectory), XtRString, "~/wais-questions/"},
   {"userSourceDirectory", "UserSourceDirectory", XtRString, sizeof(char *),
         offset(userSourceDirectory), XtRString, "~/wais-sources/"},
   {"commonSourceDirectory", "CommonSourceDirectory", XtRString, sizeof(char *),
         offset(commonSourceDirectory), XtRString, ""},
   {"documentDirectory", "DocumentDirectory", XtRString, sizeof(char *),
         offset(documentDirectory), XtRString, "~/wais-documents/"},
   {"helpFile", "HelpFile", XtRString, sizeof(char *),
         offset(helpFile), XtRString, "./XwaisHELP"},
   {"removeSeekerCodes", "RemoveSeekerCodes", XtRString, sizeof(char *),
      offset(removeSeekerCodes), XtRString, "On"},
   {"rescanInterval", "RescanInterval", XtRString, sizeof(char *),
      offset(rescanInterval), XtRString, "1"},
   {"seedWords", "SeedWords", XtRString, sizeof(char *),
      offset(seedWords), XtRString, ""},
   {"initialSource", "InitialSource", XtRString, sizeof(char *),
      offset(initialSource), XtRString, ""},
   {"questionName", "QuestionName", XtRString, sizeof(char *),
      offset(questionName), XtRString, ""},
   {"filters", "Filters", XtRString, sizeof(char *),
      offset(filters), XtRString, ""},
   {"maximumResults", "MaximumResults", XtRInt, sizeof(int),
      offset(maximumResults), XtRImmediate, (caddr_t) 40},
   {"doSearch", "DoSearch", XtRBoolean, sizeof(Boolean),
      offset(doSearch), XtRBoolean, False},
   {"defaultsInstalled", "DefaultsInstalled",  XtRBoolean, sizeof(Boolean),
      offset(defaultsInstalled), XtRBoolean, False},
   {"showKeys", "ShowKeys",  XtRBoolean, sizeof(Boolean),
      offset(showKeys), XtRBoolean, False}
};
#undef offset

XtActionsRec xwaisActionsTable[] = {
  /* special for return only */
  {(String)"waissearch", (XtActionProc)DoSearch},
  {(String)"save", (XtActionProc)DoSave},
  {(String)"setdir", (XtActionProc)SetDir},
  {(String)"dopsave", (XtActionProc)DoPSave},
  NULL
  };

void ExitCommand()
{
  exit(-1);
}

PopExit(parent, message)
Widget parent;
char *message;
{
  Widget shell, frame, labelwid, stringlabelwid;
  WidgetClass wclass;
  static String namestring;
  Arg		args[5];
  Position	x, y;
  Dimension	width, height;
  Cardinal	n;

  shell = XtCreatePopupShell("exitpopup", applicationShellWidgetClass,
			     parent, NULL, ZERO);
  frame = XtCreateManagedWidget("exitpopupform", formWidgetClass,
				shell, NULL, ZERO);

  labelwid = MakeLabel(frame, "exitlabel", message, NULL, NULL);

  MakeCommandButton(frame, "Ok", ExitCommand, labelwid, NULL, NULL);

  n = 0;
  XtSetArg(args[n], XtNx, 100); n++;
  XtSetArg(args[n], XtNy, 100); n++;
  XtSetArg(args[n], XtNtitle, "XWAIS Question Error"); n++;
  XtSetValues(shell, args, n);
 
  XtPopup(shell, XtGrabExclusive);
}


static time_t usersourcetime, commonsourcetime, questiontime;
static int rescanint;

void ScanDirs(closure, id)
     Opaque closure;
     XtIntervalId *id;
{
  char **list_data;
  float top, shown;
  int CurrentQuestion, CurrentSource, i;
  struct stat buf;
  boolean rescan;
  char string[STRINGSIZE];

  rescan = FALSE;

  stat(app_resources.userSourceDirectory, &buf);

  if(buf.st_mtime != usersourcetime) {
    usersourcetime = buf.st_mtime;
    rescan = TRUE;
  }

  if(app_resources.commonSourceDirectory[0] != 0) {
    stat(app_resources.commonSourceDirectory, &buf);

    if(buf.st_mtime != commonsourcetime) {
      commonsourcetime = buf.st_mtime;
      rescan = TRUE;
    }
  }

  if (rescan) {
    rescan = FALSE;

    CurrentSource = get_selected_source();
    if (CurrentSource != NO_ITEM_SELECTED)
      strcpy(string, Source_items[CurrentSource]);

    NumSources = 0;

    GetSourceNames(app_resources.userSourceDirectory);
    if(app_resources.commonSourceDirectory[0] != 0)
      GetSourceNames(app_resources.commonSourceDirectory);

    RebuildListWidget(sourcewindow, Source_items, LIST_NONE);

    if(CurrentSource != NO_ITEM_SELECTED) {
      for(i = 0;
	  Source_items[i] != NULL && 
	  strcmp(Source_items[i], string) != 0;
	  i++);
#ifndef MOTIF
      if(i < NumSources) XawListHighlight(sourcewindow->ListWidget, i);
#endif
    }
  }

  rescantimerid = XtAddTimeOut(rescanint, ScanDirs, (Opaque) ScanDirs);
}

/* this will take as its argument the name of a question file,
   and will do all the right things with it.
*/

void
main(argc, argv)
     int argc;
     char *argv[];
{
  struct stat buf;
  long count;
  Widget frame, labelwid, stringlabelwid, button;
  WidgetClass wclass;
  static String
    name = "";
  Arg arglist[10];
  Cardinal num_args;
  static String items[] = {NULL};
  int n;
  float shown;
  Question question;
  char msg[STRINGSIZE];
  char *cdir, *sdir;


  Type_items = (char**)s_malloc(20*sizeof(char*));
  Type_items[0] = NULL;

  if (command_name = (char*)rindex(argv[0], '/'))
    command_name++;
  else
    command_name = argv[0];

  
  the_Source = (Source) s_malloc(sizeof(_Source));

  NumSources = 0;

  allText = NULL;

  double_click = FALSE;

  top = XtInitialize("xwais", "Xwais", NULL, 0, &argc, argv);
  
  XtAddActions(xwaisActionsTable, XtNumber(xwaisActionsTable));

  XtGetApplicationResources(top, &app_resources, resources,
			    XtNumber(resources), NULL, 0);

  app_context = XtWidgetToApplicationContext(top);

  if (app_resources.defaultsInstalled == False) {
    PopExit(top, "X resources not properly installed");
    XtMainLoop();
  }

  if(app_resources.documentDirectory[0] == '~') {
    char *home, *dir, *getenv();
    
    if((home = getenv("HOME")) != NULL) {
      if((dir = s_malloc(strlen(home) +
		       strlen(app_resources.documentDirectory) +
		       2)) == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(dir, home);
      strcat(dir, &app_resources.documentDirectory[1]);
      app_resources.documentDirectory=dir;
    }
  }

  if(app_resources.userSourceDirectory[0] == '~') {
    char *home, *dir, *getenv();
    
    if((home = getenv("HOME")) != NULL) {
      if((dir = s_malloc(strlen(home) +
		       strlen(app_resources.userSourceDirectory) +
		       2)) == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(dir, home);
      strcat(dir, &app_resources.userSourceDirectory[1]);
      app_resources.userSourceDirectory=dir;
    }
  }
  sdir = app_resources.userSourceDirectory;
  cdir = app_resources.commonSourceDirectory;

  stat(app_resources.userSourceDirectory, &buf);
  usersourcetime = buf.st_mtime;
  GetSourceNames(app_resources.userSourceDirectory);

  if(app_resources.commonSourceDirectory[0] != 0) {
    stat(app_resources.commonSourceDirectory, &buf);
    commonsourcetime = buf.st_mtime;
    GetSourceNames(app_resources.commonSourceDirectory);
  }

  if(app_resources.questionDirectory[0] == '~') {
    char *home, *dir, *getenv();
    
    if((home = getenv("HOME")) != NULL) {
      if((dir = s_malloc(strlen(home) +
		       strlen(app_resources.questionDirectory) +
		       2)) == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(dir, home);
      strcat(dir, &app_resources.questionDirectory[1]);
      app_resources.questionDirectory=dir;
    }
  }
  /* let's see if it exists, if not, try to create it */
  {
    DIR *dirp;
    char *makedir;

    if((dirp = opendir(app_resources.questionDirectory)) == NULL) {
      if((makedir = s_malloc(strlen(app_resources.questionDirectory) + 12))
	 == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(makedir, "/bin/mkdir ");
      strcat(makedir, app_resources.questionDirectory);
      if(makedir[strlen(makedir)-1] == '/') makedir[strlen(makedir)-1] = 0;
      if(system(makedir) != 0)
	fprintf(stderr, "Error creating directory: %s\n",
		app_resources.questionDirectory);
    }
    else
      closedir(dirp);

    if((dirp = opendir(app_resources.userSourceDirectory)) == NULL) {
      if((makedir = s_malloc(strlen(app_resources.userSourceDirectory) + 12))
	 == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(makedir, "/bin/mkdir ");
      strcat(makedir, app_resources.userSourceDirectory);
      if(makedir[strlen(makedir)-1] == '/') makedir[strlen(makedir)-1] = 0;
      if(system(makedir) != 0)
	fprintf(stderr, "Error creating directory: %s\n",
		app_resources.userSourceDirectory);
    }
    else
      closedir(dirp);

    if((dirp = opendir(app_resources.documentDirectory)) == NULL) {
      if((makedir = s_malloc(strlen(app_resources.documentDirectory) + 12))
	 == NULL) {
	fprintf(stderr, "Ran out of space trying to create directory name.\n");
	exit(-1);
      }
      strcpy(makedir, "/bin/mkdir ");
      strcat(makedir, app_resources.documentDirectory);
      if(makedir[strlen(makedir)-1] == '/') makedir[strlen(makedir)-1] = 0;
      if(system(makedir) != 0)
	fprintf(stderr, "Error creating directory: %s\n",
		app_resources.documentDirectory);
    }
    else
      closedir(dirp);
  }
  
  {
    char qfilename[256];
    FILE *fp;

    if(argc < 1 || argc > 2) {
      fprintf(stderr, "usage: %s [Question_file_name]", argv[0]);
      exit(0);
    }
    else {

      the_Question = (XQuestion)s_malloc(sizeof(_XQuestion));
      the_Question->q = (Question)s_malloc(sizeof(_Question));
      the_Question->q->maxresdocs = app_resources.maximumResults;
      the_Question->q->sourcepath = s_malloc((sdir?strlen(sdir):0)+(cdir?strlen(cdir):0)+2);
      sprintf(the_Question->q->sourcepath, "%s:%s", (sdir?sdir:""), (cdir?cdir:""));
      if (argc == 1){ /* we have no quesiton specified.  Let's just make a new one */
	if(app_resources.questionName[0] != 0)
	  strcpy(the_Question->q->name, app_resources.questionName);
	else
	  strcpy(the_Question->q->name, "New Question");
      }
      else {
	sprintf(qfilename, "%s%s", app_resources.questionDirectory, argv[1]);
	if((fp = fopen(qfilename, "r")) == NULL) {
	  fprintf(stderr, "Can't open question %s\n", qfilename);
	  exit(-1);
	}
	strcpy(the_Question->q->name, argv[1]);
	ReadQuestion(the_Question->q, fp);
	fclose(fp);
      }
    }
  }

  if (app_resources.seedWords[0] != 0
      && the_Question->q->keywords[0] == 0)
    strcpy(the_Question->q->keywords, app_resources.seedWords);

  if (app_resources.initialSource[0] != 0
      && the_Question->q->Sources == NULL) {
    the_Question->q->Sources = makeSourceList((SourceID)s_malloc(sizeof(_SourceID)),
				       NULL);
    the_Question->q->Sources->thisSource->filename = s_strdup(app_resources.initialSource);
  }    

  CurDpy = XtDisplay(top);

  MakeQuestionForm(the_Question);

  sourcepopup = (Widget) MakeSourcePopup(top);

  messwidget = the_Question->window->StatusWindow;

  GetSourceNames(app_resources.userSourceDirectory);
  if(app_resources.commonSourceDirectory[0] != 0)
    GetSourceNames(app_resources.commonSourceDirectory);


  /* and away we go! */

  question = the_Question->q;

  sprintf(msg, "X WAIS Question: %s",  question->name);

  num_args = 0;
  XtSetArg(arglist[num_args], XtNtitle, msg); num_args++;
  XtSetArg(arglist[num_args], XtNiconName, question->name); num_args++;
  XtSetValues(top, arglist, num_args);

  question->modified = TRUE;

  sourcewindow = (ScrollList)MakeSourceFrame(top);
  typewindow = (ScrollList)MakeTypeFrame(top);

  RebuildListWidget(sourcewindow, Source_items, LIST_NONE);

  XtRealizeWidget(top);

  /* do all  the necessary stuff */

  the_Question->Source_Items =
    buildSourceItemList(question->Sources);

  RebuildListWidget(the_Question->window->Sources, the_Question->Source_Items, LIST_NONE);

  question->numsources = charlistlength(the_Question->Source_Items);

  the_Question->Relevant_Items =
    buildDocumentItemList(question->RelevantDocuments, FALSE);

  RebuildListWidget(the_Question->window->RelevantDocuments,
		    the_Question->Relevant_Items, LIST_NONE);

  question->numdocs = charlistlength(the_Question->Relevant_Items);

  the_Question->Result_Items =
    buildDocumentItemList(question->ResultDocuments, TRUE);

  RebuildListWidget(the_Question->window->ResultDocuments,
		    the_Question->Result_Items, LIST_NONE);

  question->numresdocs = charlistlength(the_Question->Result_Items);

  if(app_resources.doSearch != False) DoSearch(NULL, NULL, NULL);

  rescanint = 1000 * atoi(app_resources.rescanInterval);

  rescantimerid = XtAddTimeOut(rescanint, ScanDirs, (Opaque) ScanDirs);

  /*freopen("/dev/null", "a", stderr);*/

  MakeWaisCursors();
  fuzzButtons(false);

  XtSetArg(arglist[0], XtNsensitive, False);
  XtSetValues(viewbutton, arglist, ONE);
  XtSetValues(saveAsButton, arglist, ONE);

  XtMainLoop();
}

