/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the X user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: xwais.c,v $
 * Revision 2.0.1.1  1995/09/15 09:57:35  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:07:29  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/08/08  07:33:13  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.2  1994/08/05  07:28:40  pfeifer
 * Release beta 04
 *
 * Revision 1.1  1993/02/16  15:10:18  freewais
 * Initial revision
 *
 * Revision 1.17  92/06/03  17:09:21  jonathan
 * Changed Scandirs to leave listwidgets where they are.  Less confusing.
 * 
 * Revision 1.16  92/04/28  15:33:35  jonathan
 * Changed references to directory reading functions to use scandir.
 * 
 * Revision 1.15  92/03/23  16:11:11  jonathan
 * ?
 * 
 * Revision 1.14  92/03/17  14:20:24  jonathan
 * Generally cleaned up.
 * 
 * Revision 1.13  92/03/07  19:39:42  jonathan
 * Fixed argument to ScanDirs.
 * 
 * Revision 1.12  92/03/06  14:48:19  jonathan
 * New and Improved source loading!
 * 
 * Revision 1.11  92/03/05  11:47:09  jonathan
 * Replace directory routines with scandir's.
 * 
 * Revision 1.10  92/03/01  14:03:40  jonathan
 * Added command_name to main.
 * 
 * 
*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/x/RCS/xwais.c,v 2.0.1.1 1995/09/15 09:57:35 pfeifer Exp $";
#endif

#define MAIN
#define XWAIS_C
#define XWAIS
#include "xwais.h"
#include "xwais.bit"
#include <sys/stat.h>

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
   {"initialSource", "initialSource", XtRString, sizeof(char *),
      offset(initialSource), XtRString, ""},
   {"questionName", "QuestionName", XtRString, sizeof(char *),
      offset(questionName), XtRString, ""},
   {"filters", "Filters", XtRString, sizeof(char *),
      offset(filters), XtRString, ""},
   {"doSearch", "DoSearch", XtRBoolean, sizeof(Boolean),
      offset(doSearch), XtRBoolean, False},
   {"defaultsInstalled", "DefaultsInstalled",  XtRBoolean, sizeof(Boolean),
      offset(defaultsInstalled), XtRBoolean, False}
};
#undef offset

void SetIcon(parent)
Widget parent;
{
  Arg args[1];
  Pixmap icon_pixmap = None;

  XtSetArg (args[0], XtNiconPixmap, &icon_pixmap);
  XtGetValues(parent, args, ONE);
  if (icon_pixmap == None) {
    XtSetArg(args[0], XtNiconPixmap, 
	     XCreateBitmapFromData(XtDisplay(parent),
				   XtScreen(parent)->root,
				   xwais_bits, xwais_width, xwais_height));
    XtSetValues (parent, args, ONE);
  }
}

static quit;

static int
my_alphasort(d1, d2)
    Direntry_t **d1;
    Direntry_t **d2;
{
    return strcasecmp(d1[0]->d_name, d2[0]->d_name);
}

static int
isqfile(dp)
Direntry_t *dp;
{
  char lastchar = dp->d_name[strlen(dp->d_name)-1];
  return(lastchar != '~' && lastchar != '#' &&
	  strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."));
}

void
ReadQuestionDirectory(directory)
char *directory;
{
  Direntry_t **list;
  char filename[MAX_FILENAME_LEN], lastchar;
  FILE *fp;
  char config[STRINGSIZE];
  int i, j;
  float shown;

  if ((j = scandir(directory, &list, isqfile, my_alphasort)) < 0)
    {
      char booboo[STRINGSIZE];
      sprintf(booboo, "Error on open of questions directory: %s.\n", directory);
      XwaisPrintf(booboo);
      return;
    }

  if(Question_items != NULL) {
    for(i =0; Question_items[i] != NULL; i++) s_free(Question_items[i]);
    s_free(Question_items);
  }

  Question_items = (char**) s_malloc((j+1) * sizeof(char*));

  for (i = 0; i < j; i++) {
    Question_items[i] = s_strdup(list[i]->d_name);
    s_free(list[i]);
  }

  NumQuestions = j;
  s_free(list);
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

  stat(app_resources.questionDirectory, &buf);

  if(buf.st_mtime != questiontime) {
    questiontime = buf.st_mtime;
    rescan = TRUE;
  }

  if (rescan) {
    NumQuestions = 0;

    CurrentQuestion = get_selected_question();

    if (CurrentQuestion != NO_ITEM_SELECTED)
      strcpy(string, Question_items[CurrentQuestion]);

    ReadQuestionDirectory(app_resources.questionDirectory);
    Question_items[NumQuestions] = NULL;

    RebuildListWidget(questionwindow, Question_items, LIST_NONE);

    if(CurrentQuestion != NO_ITEM_SELECTED) {
      for(i = 0;
	  Question_items[i] != NULL &&
	  strcmp(Question_items[i], string) != 0;
	  i++);
      if(i < NumQuestions) XawListHighlight(questionwindow->ListWidget, i);
    }
  }

  rescantimerid = XtAddTimeOut(rescanint, ScanDirs, (Opaque) ScanDirs);
}

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
  XtSetArg(args[n], XtNtitle, "XWAIS Error"); n++;
  XtSetValues(shell, args, n);

  XtPopup(shell, XtGrabExclusive);
}

void
main(argc, argv)
     int argc;
     char *argv[];
{
  struct stat buf;
  long first_filename_number = 1; /* for indexing into the arglist */
  long count;




  if (command_name = (char*)rindex(argv[0], '/'))
    command_name++;
  else
    command_name = argv[0];

  

  NumSources = 0;

  double_click = FALSE;

  top = XtInitialize( "xwais", "Xwais", NULL, 0, &argc, argv);
  
  XtGetApplicationResources(top, &app_resources, resources,
			    XtNumber(resources), NULL, 0);

  {
    Arg		args[5];
    Cardinal	n;

    n = 0;
    XtSetArg(args[n], XtNtitle, "XWAIS"); n++;
    XtSetArg(args[n], XtNiconName, "XWAIS"); n++;
    XtSetValues(top, args, n);
  }

  SetIcon(top);

  if (app_resources.defaultsInstalled == False) {
    PopExit(top, "X resources not properly installed");
    XtMainLoop();
  }

  CurDpy = XtDisplay(top);

  form = SetupWaisDisplay(top);

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

  /* let's see if user directories exist, if not, try to create them */
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
  }
  
  stat(app_resources.userSourceDirectory, &buf);
  usersourcetime = buf.st_mtime;
  GetSourceNames(app_resources.userSourceDirectory);

  if(app_resources.commonSourceDirectory[0] != 0) {
    stat(app_resources.commonSourceDirectory, &buf);
    commonsourcetime = buf.st_mtime;
    GetSourceNames(app_resources.commonSourceDirectory);
  }

  stat(app_resources.questionDirectory, &buf);
  questiontime = buf.st_mtime;
  ReadQuestionDirectory(app_resources.questionDirectory);

  Question_items[NumQuestions] = NULL;

  RebuildListWidget(questionwindow, Question_items, LIST_NONE);

  RebuildListWidget(sourcewindow, Source_items, LIST_NONE);

  /* and away we go! */

  rescanint = 1000 * atoi(app_resources.rescanInterval);

  rescantimerid = XtAddTimeOut(rescanint, ScanDirs, (Opaque) ScanDirs);

  XtRealizeWidget(top);

  XtMainLoop();
}

/* ARGSUSED */
void
DoQuit(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  char msg[STRINGSIZE], quest[STRINGSIZE];

  if(double_click && LastClicked == w) {
    exit(0); 
  }

  msg[0] = 0;

  XwaisPrintf("If you really want to quit, press Quit again.\n");
  Feep();
  double_click = TRUE;
  LastClicked = w;
}
