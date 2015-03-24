/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/* This is a simple screen user interface for making WAIS queries
 * It is the strange offspring of waisq and curses..
 *
 * Revision 1.2 initial version by jcurran@nnsc.nsf.net 
 *   (dedicated to Suzi, Alex, et. al.)          
 *
 * Revision 1.5 changes 12/91 by datta@cs.uwp.edu
 *
 *   added page up/down 'J/K'
 *   screen state now returns to normal after piping commands
 *   added mail 'm' option 
 *   fixed number entry on source selection to allow for up to MAXINT sources
 *   there is now a pause after viewing a document so if the pager is 'more'
 *         the user will not miss the last screen.
 *
 * Revision 1.6 changes 01/92 by jcurran@nnsc.nsf.net
 *
 *   fixed common_wais_dir on help message
 *   change default common dir to /usr/lib/wais-sources
 *   fix formatting for >99 sources
 *   add ^v aliases for page down, <esc>v for page up
 *   add "/" (find) to find source
 *   add "/" (find) to find result
 *   simplify <return> processing for users
 *   changed the view source information to pipe through $PAGER or 'more'
 *         to allow the use of source descriptions larger than 1 screen.
 *         (done by datta@cs.uwp.edu)
 * 
 * Revision 1.7 changes 01/92 by jcurran@nnsc.nsf.net
 *
 *   fixed miscellaneous selection bugs
 *   added option processing and option screen
 *
 * Revision 1.12 changes 07/92 by warnock@hypatia.gsfc.nasa.gov
 *
 *   added patch from comp.infosystems.wais to add elm-like cursor paging
 *
 * Revision 1.13 changes 08/92 by warnock@hypatia.gsfc.nasa.gov
 *
 *   replaced piping documents and source descriptions with curses calls
 *
 * Notes:
 *   If there are ever more than 999 sources screen display will need
 *       changes.
 *
 *   The use of mvprintw() is dangerous unless there is window bounds
 *       checking. The curses with ULTRIX 4.2, BSD 4.3 and BSD 4.3 reno
 *       have unpredictable results when the window size is exceeded.
 *       Keep this in mind for further enhancements.
 *
 *   The ULTRIX curses.h may be missing some entries and may require
 *       updating. If so, add a DEFINE ULTRIX when you compile
 *
 */

#include "../ir/cdialect.h"
#include <curses.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <ui.h>
#include <sockets.h>
#ifdef I_TERMIOS
#include <termios.h>
#else /* not I_TERMIOS */
#ifdef I_TERMIO
#include <termio.h>
#else /* not I_TERMIO */
#include <sgtty.h>
#endif /* not I_TERMIO */
#endif /* not I_TERMIOS */
#ifdef I_NETINET_IN
#include <netinet/in.h>
#endif /* I_NETINET_IN */

#define STRINGSIZE	256

#include "wais.h"
#define MAIN
#include "globals.h"

#define	WAIS_SERVICE	"z39_50"

#define WAISSCREEN_NAME     "SWAIS"
#define WAISSCREEN_VERSION  "$Revision: 2.0.1.3 $"
#define WAISSCREEN_AUTHOR   "John Curran (jcurran@nnsc.nsf.net)"

#ifndef COMMON_SOURCE_DIR
#define COMMON_SOURCE_DIR "/usr/lib/wais-sources"
#endif

/* SWAIS PROGRAM STATES - each decade shares common screen (0-9, 10-19, etc) */
#ifndef UNKNOWN
#define	UNKNOWN	        0	/* curses probably already defines UNKNOWN */
#endif
#define	GETSOURCES	10
#define	GETKEYWORDS	11
#define	MAKEQUERY	12
#define	SHOWRESULTS	20
#define	GETOPTIONS	30
#define RELDOCS		40
#define	LEAVEPROGRAM	90

typedef int     programstate;

/* SWAIS OPTION TYPES */
#define	OPTION_BOOLEAN	0
#define OPTION_INTEGER	1
#define	OPTION_STRING	2

DocList         Mydocs = NULL;

typedef struct option {
  char            name[STRINGSIZE];
  char            desc[STRINGSIZE];
  int             type;
  void            (*update) ();
  union {
    int            *intptr;
    char           *charptr;
  } var;
} _Option      , *Option;

/* options */
#define MAXOPTIONS 6
int             option_widetitles = FALSE;
int             option_sortsources = TRUE;
int             option_pagerpause = TRUE;

_Option         options[MAXOPTIONS + 1];

programstate    state;
programstate    new_state;
jmp_buf         main_env;
static char     keywords[STRINGSIZE];
SList           Selected_Sources;
char            command[STRINGSIZE];
char           *sdir, *cdir;

#define MAXSELECTIONS 500
#define NOSELECTION -999999
char            select_line[MAXSELECTIONS][STRINGSIZE];
int             selection;		    /* 1 to max_selection */
int             max_selection;		    /* limit on selection */
int             current_page;		    /* 0 to needed pages; preset to -1 to force refresh */
int             page_length;		    /* number of selections per page */

/* routine specific selection number */
int             source_selection = 1;
int             result_selection = 1;
int             option_selection = 1;
int             relevant_selection = 1;

extern unsigned int sleep _AP ((unsigned int seconds));

void 
SortSources ()
{
  Boolean         Changed = TRUE;
  Source          s;
  SList           sl;

  while (Changed) {
    Changed = FALSE;
    for (sl = Sources; sl->nextSource != NULL; sl = sl->nextSource) {
      if (sl->nextSource->thisSource->name == NULL) {
	Changed = TRUE;
	sl->nextSource = sl->nextSource->nextSource;
      } else if (0 < strcasecmp (sl->thisSource->name, sl->nextSource->thisSource->name)) {
	Changed = TRUE;
	s = sl->thisSource;
	sl->thisSource = sl->nextSource->thisSource;
	sl->nextSource->thisSource = s;
      }
    }
  }
}

int 
freeRelDoc (dlist, number)
     DocList         dlist;
     int             number;
{
  DocList         d, dm, dl;
  DocList         dprev = NULL;
  int             i;

  for (i = 0, d = dlist; (d != NULL) && (i < number); dprev = d, i++, d = d->nextDoc);

  if (dprev == NULL && d != NULL) {	/* first doc */
    dl = d;
    dlist = d->nextDoc;
    d = dlist;
    freeDocumentID (dl->thisDoc);
    s_free (dl);
    Mydocs = dlist;
    return (listlength ((List) dlist));
  } else if (d != NULL) {
    freeDocumentID (d->thisDoc);
    dl = d;
    d = dl->nextDoc;
    s_free (dl);
    dprev->nextDoc = d;
    return (listlength ((List) dlist));
  }
  return (0);
}
void 
screenend ()
{
  signal (SIGINT, SIG_IGN);
  clear ();
  refresh ();
/*    reset_shell_mode(); */
  endwin ();
  echo ();
}

void 
startover ()
{
  signal (SIGINT, startover);
  longjmp (main_env, 1);
}

void 
screenstart ()
{
  initscr ();
  cbreak ();
  noecho ();
/*    reset_prog_mode(); */
  leaveok (stdscr, FALSE);
  signal (SIGINT, startover);
}

void
display_context (context)
     char           *context;
{
  mvprintw (LINES - 3, 0, context);
  clrtoeol ();
  refresh ();
}

void
display_prompt (prompt)
     char           *prompt;
{
  mvprintw (LINES - 1, 0, prompt);
  clrtoeol ();
  refresh ();
}

void
display_highlighted (str)
     char           *str;
{
  standout ();
  mvprintw (LINES - 1, 0, str);
  standend ();
  printw (" ");
  clrtoeol ();
  refresh ();
}

#ifdef ANSI_LIKE		/* use ansi varargs */
void 
PrintStatus (long logtype, long loglevel, char *format,...)
#else /* k&r */
void 
PrintStatus (va_alist)
     va_dcl

#endif /* ANSI_LIKE */
{
  va_list         ap;			    /* the variable arguments */
  char            message[1000];

#ifndef ANSI_LIKE
  long            logtype, loglevel;
  char           *format;

  va_start (ap);

  logtype = va_arg (ap, int);
  loglevel = va_arg (ap, int);
  format = va_arg (ap, char *);

#else
  va_start (ap, format);
#endif

  vsprintf (message, format, ap);
  if (stdscr != NULL) {
    display_highlighted (message);
    sleep (2);
  } else
    fprintf (stderr, "%s\n", message);
}

char           *
firstphrase (str)
     char           *str;
{
  char           *res;
  int             n;

  res = str;
  if ((n = strcspn (str, " ")) != 0)
    if ((n > 4) && (str[n - 4] == '.')) {

      n = n - 4;
      res = s_malloc (n + 1);
      strncpy (res, str, n);
    }
  return res;
}

void
format_option (str, opt)
     char           *str;
     Option          opt;
{
  int            *p;

  switch (opt->type) {
  case OPTION_STRING:
    sprintf (str, "%s", (opt->var.charptr));
    break;
  case OPTION_INTEGER:
    p = opt->var.intptr;
    sprintf (str, "%d", *p);
    break;
  case OPTION_BOOLEAN:
    if (*(opt->var.intptr) == TRUE)
      sprintf (str, "on");
    else
      sprintf (str, "off");

    break;
  }
}

jmp_buf         local_env;

char            fpath[STRINGSIZE];

void
start_use (fd)
     FILE          **fd;
{
  *fd = fopen (fpath, "w+");
}

void
end_use (fd)
     FILE          **fd;
{
  fclose (*fd);
}

void
null ()
{
}

void
Refresh_sources ()
{
  FreeSources (Sources);
  Sources = NULL;
  ReadSourceDirectory (sdir, TRUE);
  ReadSourceDirectory (cdir, TRUE);
  if (NumSources == 0) {
    PrintStatus (STATUS_URGENT, STATUS_HIGH,
		 "Error: Unable to find any WAIS information sources.");
    exit (-1);
  }
  if (option_sortsources == TRUE)
    SortSources ();
}

void
UseWaisDocument (q, doc)
     Question        q;
     DocumentID      doc;
{
  char            message[STRINGSIZE];

  if ((doc->doc->type == NULL) || strcmp (doc->doc->type[0], "WSRC") != 0) {
    fprintf (stderr, "%s", "%s is not a WAIS source.  You can only USE WAIS sources.",
	     firstphrase (trim_junk (doc->doc->headline)));
    /* PrintStatus("%s is not a WAIS source.  You can only USE WAIS sources.",
       firstphrase(trim_junk(doc->doc->headline))); */
  } else {
    PrintStatus (STATUS_INFO, STATUS_MEDIUM,
	  "Retrieving: %s\n", firstphrase (trim_junk (doc->doc->headline)));
    sprintf (fpath, "%s%s.src", sdir, firstphrase (trim_junk (doc->doc->headline)));
    RetrieveWaisDocument (start_use, end_use, q, doc);

    PrintStatus (STATUS_INFO, STATUS_MEDIUM,
       "Adding Source: %s\n", firstphrase (trim_junk (doc->doc->headline)));
    Refresh_sources ();
  }
}

jmp_buf         local_env;

void 
screen_catchpipe (sig, code, scp, addr)
     int             sig, code;
     struct sigcontext *scp;
     char           *addr;
{
  signal (SIGPIPE, screen_catchpipe);
  longjmp (local_env, 1);
}

char            pipe_cmd[STRINGSIZE];

void
start_pipe (pipefd)
     FILE          **pipefd;
{
  screenend ();
  *pipefd = popen (pipe_cmd, "w");
}

void
end_pipe (pipefd)
     FILE          **pipefd;
{
  pclose (*pipefd);
}

void 
AddDocAsRelevant (doc)
     DocumentID      doc;
{
  int             i;
  char            bf[10];

  Mydocs = makeDocList (doc, Mydocs);
}

void
PipeWaisDocument (q, doc, cmd)
     Question        q;
     DocumentID      doc;
     char           *cmd;
{
  char            message[STRINGSIZE];

  PrintStatus (STATUS_INFO, STATUS_MEDIUM,
	  "Retrieving: %s\n", firstphrase (trim_junk (doc->doc->headline)));

  strcpy (pipe_cmd, cmd);
  signal (SIGPIPE, screen_catchpipe);
  if (setjmp (local_env) == 0)
    RetrieveWaisDocument (start_pipe, end_pipe, q, doc);
  signal (SIGPIPE, SIG_DFL);
}

void
DisplayWaisDocument (q, doc)
     Question        q;
     DocumentID      doc;
{
  char            message[STRINGSIZE];

  PrintStatus (STATUS_INFO, STATUS_MEDIUM,
	  "Retrieving: %s\n", firstphrase (trim_junk (doc->doc->headline)));

  RetrieveWaisDocument (NULL, NULL, q, doc);
}

void
FreeSourceList (sources)
     SourceList      sources;
{
  SourceList      s, n;

  for (s = sources; s != NULL; s = n) {
    if (s->thisSource != NULL) {
      if (s->thisSource->filename != NULL)
	s_free (s->thisSource->filename);
      s_free (s->thisSource);
    }
    n = s->nextSource;
    s_free (s);
  }
}

void 
AddQuestionSource (question, sourcename)
     Question        question;
     char           *sourcename;
{
  SourceList      tmp;
  SourceList      s;
  SourceID        sid;

  sid = (SourceID) s_malloc (sizeof (_SourceID));
  sid->filename = s_strdup (sourcename);
  s = (SourceList) s_malloc (sizeof (_SourceList));
  s->thisSource = sid;
  s->nextSource = NULL;
  tmp = question->Sources;
  question->Sources = s;
  if (tmp != NULL)
    s->nextSource = tmp;
}

void
usage (name)
     char           *name;
{
  fprintf (stderr, "Usage: %s\n", name);
  fprintf (stderr, "  [-s sourcname]          select sourcename for search\n");
  fprintf (stderr, "  [-S sourcedir]          defaults to ~/wais-sources\n");
  fprintf (stderr, "  [-C common_sourcedir]   defaults to %s\n", COMMON_SOURCE_DIR);
  fprintf (stderr, "  [-h]	              this help message\n");
  fprintf (stderr, "  [keywords]\n");
}

SourceID 
FindQuestionSource (sourcelist, sourcename)
     SourceList      sourcelist;
     char           *sourcename;
{
  SourceList      tmpsource;

  for (tmpsource = sourcelist; tmpsource != NULL;
       tmpsource = tmpsource->nextSource) {
    if (tmpsource->thisSource != NULL)
      if (!strcmp (sourcename, tmpsource->thisSource->filename))
	return (tmpsource->thisSource);
  }
  return (NULL);
}

Source 
FindSource (sourcelist, sourcename)
     SList           sourcelist;
     char           *sourcename;
{
  SList           tmpsource;

  for (tmpsource = sourcelist; tmpsource != NULL;
       tmpsource = tmpsource->nextSource) {
    if (tmpsource->thisSource != NULL)
      if (!strcmp (sourcename, tmpsource->thisSource->name))
	return (tmpsource->thisSource);
  }
  return (NULL);
}

void
DumpSources (sourcelist)
     SList           sourcelist;
{
  SList           tmpsource;

  if (sourcelist == NULL) {
    printf ("Sourcelist NULL");
    return;
  }
  printf (" Dump of Sourcelist \n");
  for (tmpsource = sourcelist; tmpsource != NULL; tmpsource = tmpsource->nextSource) {
    if (tmpsource->thisSource == NULL)
      printf ("	thisSource NULL\n");
    else
      printf ("	thisSource = %s\n", tmpsource->thisSource->name);
    if (tmpsource->nextSource == NULL)
      printf ("	nextSource NULL\n");
    else
      printf ("	following nextSource ->\n");
  }
}

SList
AddSource (sourcelist, currentsource)
     SList           sourcelist;
     Source          currentsource;
{
  Source          s;
  SList           tmpsource;
  SList           last;

  if (sourcelist == NULL)
    sourcelist = makeSList (NULL, NULL);

  last = sourcelist;

  for (tmpsource = sourcelist; tmpsource != NULL;
       last = tmpsource, tmpsource = tmpsource->nextSource)
    if (tmpsource->thisSource != NULL)
      if (!strcmp (currentsource->name, tmpsource->thisSource->name))
	return (sourcelist);

  s = (Source) s_malloc (sizeof (_Source));
  s->initp = currentsource->initp;
  s->name = s_strdup (currentsource->name);
  s->directory = s_strdup (currentsource->directory);

  if (last->thisSource == NULL)
    last->thisSource = s;
  else
    last->nextSource = makeSList (s, NULL);
  return (sourcelist);
}

SList
DeleteSource (sourcelist, currentsource)
     SList           sourcelist;
     Source          currentsource;
{
  SList           s;
  SList           tmpsource;
  SList           last;

  if (sourcelist == NULL)
    return (NULL);

  last = sourcelist;

  for (tmpsource = sourcelist; tmpsource != NULL;
       last = tmpsource, tmpsource = tmpsource->nextSource)
    if (tmpsource->thisSource != NULL)
      if (!strcmp (currentsource->name, tmpsource->thisSource->name)) {
	if (tmpsource->thisSource->name != NULL)
	  s_free (tmpsource->thisSource->name);
	if (tmpsource->thisSource->directory != NULL)
	  s_free (tmpsource->thisSource->directory);
	if (tmpsource->thisSource->maintainer != NULL)
	  s_free (tmpsource->thisSource->maintainer);

	/* save next link ptr, discard current link */
	s = tmpsource->nextSource;
	s_free (tmpsource);

	/* return next as start if start destroyed */
	if (last == sourcelist)
	  return (s);

	/* Splice next into position, return start ptr */
	last->nextSource = s;
	break;
      }
  return (sourcelist);
}

char           *
fixdirname (dir)
     char           *dir;
{
  char           *res;

  if (dir[strlen (dir) - 1] == '/')
    res = dir;
  else {
    res = s_malloc (strlen (dir) + 2);
    sprintf (res, "%s/", dir);
  }
  return res;
}

int
selection_page (sel)
     int             sel;
{
  return (((sel - 1) / page_length));
}

int
selection_line (sel)
     int             sel;
{
  return (((sel - 1) % page_length) + 2);
}

selection_display (new_sel)
     int             new_sel;
{
  int             new_page;
  int             new_selection;
  int             sel_offset;
  int             k;

  standend ();
  mvprintw (selection_line (selection), 0, "%s", select_line[selection]);

  if (new_sel == NOSELECTION)
    new_selection = selection;
  else
    new_selection = new_sel;

  if (new_selection < 1)
    new_selection = max_selection;
  if (new_selection > max_selection)
    new_selection = 1;

  new_page = selection_page (new_selection);
  if (current_page != new_page) {
    sel_offset = new_page * page_length;
    for (k = 1; k <= page_length; k++)
      if ((sel_offset + k) <= max_selection)
	mvprintw (1 + k, 0, "%s", select_line[sel_offset + k]);
      else
	mvprintw (1 + k, 0, "\n");
  }
  if (new_sel != NOSELECTION) {
    standout ();
    mvprintw (selection_line (new_selection), 0, "%s", select_line[new_selection]);
    standend ();
  }
  move (selection_line (new_selection), 0);
  current_page = new_page;
  selection = new_selection;
  refresh ();
}

void
title_display (title, status, status_count)
     char           *title;
     char           *status;
     int             status_count;
{
  mvprintw (0, 0, WAISSCREEN_NAME);
  standout ();
  mvprintw (0, COLS / 2 - (strlen (title) / 2), "%s", title);
  standend ();
  mvprintw (0, COLS - (strlen (status) + 4), "%s %2ld", status, status_count);
}

void 
DumpTextToScreen (buffer, size)
     char           *buffer;
     long            size;
{
  int             linecnt, j, i;
  char            ch;
  int             OUTLINES;

  OUTLINES = LINES - 1;
  clear ();
  title_display ("Document Display", "Page:", 1);
  move (1, 0);
  linecnt = 1;

  for (i = 0; i < size; i++, j++) {
    addch (buffer[i]);

    if (buffer[i] < ' ') {
      clrtoeol ();
      refresh ();
      linecnt++;
    }
    if (linecnt == OUTLINES - 1) {
      PrintStatus (STATUS_URGENT, STATUS_HIGH,
		   "Press any key to continue, 'q' to quit.");
      ch = getch ();
      if ((ch == 'q') || (ch == 'Q')) {
	return;
      } else {
	clear ();
	move (0, 0);
	linecnt = 0;
      }
    }
  }
  PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press a key to continue");
  getch ();
}

void
show_source_entry_help ()
{
  int             j = 2;

  clear ();
  title_display ("Source Selection Help", "Page:", 1);
  mvprintw (j++, 0, "%s\t%s", "j, down arrow, ^N", "Move Down one source");
  mvprintw (j++, 0, "%s\t%s", "k, up arrow, ^P  ", "Move Up one source");
  mvprintw (j++, 0, "%s\t%s", "J, ^V, ^D        ", "Move Down one screen");
  mvprintw (j++, 0, "%s\t%s", "K, <esc> v, ^U   ", "Move Up one screen");
  mvprintw (j++, 0, "%s\t%s", "###              ", "Position to source number ##");
  mvprintw (j++, 0, "%s\t%s", "/sss             ", "Search for source sss");
  mvprintw (j++, 0, "%s\t%s", "<space>, <period>", "Select current source");
  mvprintw (j++, 0, "%s\t%s", "=                ", "Deselect all sources");
  mvprintw (j++, 0, "%s\t%s", "v, <comma>       ", "View current source info");
  mvprintw (j++, 0, "%s\t%s", "<ret>            ", "Perform search");
  mvprintw (j++, 0, "%s\t%s", "s                ", "Select new sources (refresh sources list)");
  mvprintw (j++, 0, "%s\t%s", "w                ", "Select new keywords");
  mvprintw (j++, 0, "%s\t%s", "X, -             ", "Remove current source permanently");
  mvprintw (j++, 0, "%s\t%s", "o                ", "Set and show swais options");
  mvprintw (j++, 0, "%s\t%s", "h, ?             ", "Show this help display");
  mvprintw (j++, 0, "%s\t%s", "H                ", "Display program history");
  mvprintw (j++, 0, "%s\t%s", "q                ", "Leave this program");
  PrintStatus (STATUS_URGENT, STATUS_HIGH,
	       "Press any key to continue");
  getch ();
}

void
view_source_info (sourcelist, current_s)
     SList           sourcelist;
     Source          current_s;
{
  FILE           *fp;
  int             j = 2;
  char            tmp_str[STRINGSIZE];

  clear ();
  title_display ("Source Info", "Page:", 1);
  if (NULL != current_s->name)
    mvprintw (j++, 0, "%s\t%s", "Name:       ", current_s->name);
  if (NULL != current_s->directory)
    mvprintw (j++, 0, "%s\t%s", "Directory:  ", current_s->directory);
  if (NULL != current_s->maintainer)
    mvprintw (j++, 0, "%s\t%s", "Maintainer: ", current_s->maintainer);
  if (NULL != FindSource (sourcelist, current_s->name))
    sprintf (tmp_str, "Yes");
  else
    sprintf (tmp_str, "No");
  mvprintw (j++, 0, "%s\t%s", "Selected:   ", tmp_str);
  format_source_cost (tmp_str, current_s);
  mvprintw (j++, 0, "%s\t%s", "Cost:       ", tmp_str);
  if (current_s->initp)
    sprintf (tmp_str, "(Accessed)");
  else
    tmp_str[0] = '\0';
  if (NULL != current_s->server)
    mvprintw (j++, 0, "%s\t%s %s", "Server:     ", current_s->server, tmp_str);
  if (NULL != current_s->service)
    mvprintw (j++, 0, "%s\t%s", "Service:    ", current_s->service);
  if (NULL != current_s->database)
    mvprintw (j++, 0, "%s\t%s", "Database:   ", current_s->database);
  if (NULL != current_s->description) {
    mvprintw (j++, 0, "%s", "Description:");
    mvprintw (j++, 0, "%s", current_s->description);
  }
  clearok (stdscr, FALSE);
  PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue.");
  cbreak ();
  noecho ();
  getch ();
}

void
view_result_info (current_doc)
     DocumentID      current_doc;
{
  int             j = 2;
  char            tmp_str[STRINGSIZE];

  clear ();
  title_display ("Result Information", "Page:", 1);

  /*  sure.. someday these might have data. jtc
     GetServer(current_doc->doc->id,tmp_str);
     mvprintw(j++,0,"%s\t%s","Server:     ",tmp_str);
     GetDatabase(current_doc->doc->id,tmp_str);
     mvprintw(j++,0,"%s\t%s","Database:   ",tmp_str);
     GetLocalID(current_doc->doc->id,tmp_str);
     mvprintw(j++,0,"%s\t%s","Local ID:   ",tmp_str);
   */
  if (NULL != current_doc->doc->date)
    mvprintw (j++, 0, "%s\t%s", "Date:       ", current_doc->doc->date);
  if (NULL != current_doc->doc->headline)
    mvprintw (j++, 0, "%s\t%s", "Source:     ", current_doc->doc->source);
  if (NULL != current_doc->doc->headline)
    mvprintw (j++, 0, "%s\t%s", "Headline:   ", current_doc->doc->headline);
  if (NULL != current_doc->doc->city)
    mvprintw (j++, 0, "%s\t%s", "City:       ", current_doc->doc->city);
  if (NULL != current_doc->doc->stock)
    mvprintw (j++, 0, "%s\t%s", "Stock:      ", current_doc->doc->stock);
  if (NULL != current_doc->doc->company)
    mvprintw (j++, 0, "%s\t%s", "Company:    ", current_doc->doc->company);
  if (NULL != current_doc->doc->industry)
    mvprintw (j++, 0, "%s\t%s", "Industry:   ", current_doc->doc->industry);
  if ((NULL != current_doc->doc->type) && current_doc->doc->type[0] != NULL)
    mvprintw (j++, 0, "%s\t%s", "Type:       ", current_doc->doc->type[0]);
  mvprintw (j++, 0, "%s\t%d", "Score       ", current_doc->rawScore);

  mvprintw (j++, 0, "%s\t%ld", "# of Lines: ", current_doc->doc->numLines);
  mvprintw (j++, 0, "%s\t%ld", "# of Chars: ", current_doc->doc->numChars);

  if (NULL != current_doc->doc->sourceID->filename)
    mvprintw (j++, 0, "%s\t%s", "SourceID:   ", current_doc->doc->sourceID->filename);
  switch (GetCopyrightDisposition (current_doc->doc->id)) {

  case (COPY_WITHOUT_RESTRICTION):
    sprintf (tmp_str, "Copy Without Restriction");
    break;
  case (ALL_RIGHTS_RESERVED):
    sprintf (tmp_str, "All Rights Reserved");
    break;
  case (DISTRIBUTION_RESTRICTIONS_APPLY):
    sprintf (tmp_str, "Distribution Restrictions Apply");
    break;
  default:
    sprintf (tmp_str, "Unknown");
    break;
  }
  mvprintw (j++, 0, "%s\t%s", "Disposition: ", tmp_str);

  PrintStatus (STATUS_URGENT, STATUS_HIGH,
	       "Press any key to continue");
  getch ();
}

void
show_swais_history ()
{
  char            versiononly[STRINGSIZE];
  int             j = 2;

  clear ();
  title_display ("SWAIS History", "Page:", 1);
  mvprintw (j++, 0, "%s", " ");
  mvprintw (j++, 0, "%s",
	    "The WAIS (Wide Area Information Service) system is a collection of programs");
  mvprintw (j++, 0, "%s",
	    "which provide for convenient information distribution over wide area networks.");
  mvprintw (j++, 0, "%s",
	    "Tools for both \"publishing\" and accessing information sources are provided.");
  mvprintw (j++, 0, "%s",
	    "The Simple WAIS (SWAIS) interface is an basic access tool designed for those");
  mvprintw (j++, 0, "%s",
	    "focused on data retreival and not computer operation. It provides most of the");
  mvprintw (j++, 0, "%s",
	    "functionality of the more complicated interfaces but features a simple and");
  mvprintw (j++, 0, "%s",
	    "potentially more natural interface.  The functionality supported includes ");
  mvprintw (j++, 0, "%s",
      "source selection, keyword entry, and automatic document retrieval.");
  mvprintw (j++, 0, "%s",
	    "I hope that this tool may be of use.  Enjoy.");
  mvprintw (j++, 0, "%s", " ");
  mvprintw (j++, 0, "%s",
	    "The WAIS system is the result of a joint project between Thinking Machines,");
  mvprintw (j++, 0, "%s",
	    "Apple Computer, and Dow Jones.  For more information on WAIS, send mail to");
  mvprintw (j++, 0, "%s",
	    "\"wais-discussion@think.com\".  The current release of the WAIS software");
  mvprintw (j++, 0, "%s",
     "is available via anonymous ftp from think.com in subdirectory wais.");
  sscanf (WAISSCREEN_VERSION, "$ %*s %s", versiononly);
  mvprintw (LINES - 5, 0, "Simple Wais %s [built with %s]", versiononly, VERSION_STRING);
  mvprintw (LINES - 3, 0, "%s", WAISSCREEN_AUTHOR);

  PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue");
  getch ();
}

void
show_relevant_help ()
{
  int             j = 2;

  clear ();
  title_display ("Relevant Doc. Help", "Page:", 1);
  mvprintw (j++, 0, "%s\t\t%s", "j, ^N  ", "Move Down one item");
  mvprintw (j++, 0, "%s\t\t%s", "k, ^P  ", "Move Up one item");
  mvprintw (j++, 0, "%s\t\t%s", "J      ", "Move Down one screen");
  mvprintw (j++, 0, "%s\t\t%s", "K      ", "Move Up one screen");
  mvprintw (j++, 0, "%s\t\t%s", "D      ", "Delete All Relevant Docs");
  mvprintw (j++, 0, "%s\t\t%s", "X      ", "Delete Current Relevant Doc");
  mvprintw (j++, 0, "%s\t\t%s", "<space>", "Display current item");
  mvprintw (j++, 0, "%s\t\t%s", "<return>", "Display current item");
  mvprintw (j++, 0, "%s\t\t%s", "v      ", "View current item information");

  PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue");
  getch ();
}

void
show_search_results_help ()
{
  int             j = 2;

  clear ();
  title_display ("Search Results Help", "Page:", 1);
  mvprintw (j++, 0, "%s\t\t%s", "j, ^N  ", "Move Down one item");
  mvprintw (j++, 0, "%s\t\t%s", "k, ^P  ", "Move Up one item");
  mvprintw (j++, 0, "%s\t\t%s", "J      ", "Move Down one screen");
  mvprintw (j++, 0, "%s\t\t%s", "K      ", "Move Up one screen");
  mvprintw (j++, 0, "%s\t\t%s", "R      ", "Show relevant documents");
  mvprintw (j++, 0, "%s\t\t%s", "S      ", "Save current item to a file");
  mvprintw (j++, 0, "%s\t\t%s", "m      ", "Mail current item to an address");
  mvprintw (j++, 0, "%s\t\t%s", "##     ", "Position to item number ##");
  mvprintw (j++, 0, "%s\t\t%s", "/sss   ", "Position to item beginning sss");
  mvprintw (j++, 0, "%s\t\t%s", "<space>", "Display current item");
  mvprintw (j++, 0, "%s\t\t%s", "<return>", "Display current item");
#ifndef SECURECLIENT
  mvprintw (j++, 0, "%s\t\t%s", "|      ", "Pipe current item into a unix command");
#endif
  mvprintw (j++, 0, "%s\t\t%s", "v      ", "View current item information");
  mvprintw (j++, 0, "%s\t\t%s", "r      ", "Make current item a relevant document");
  mvprintw (j++, 0, "%s\t\t%s", "s      ", "Specify new sources to search");
  mvprintw (j++, 0, "%s\t\t%s", "u      ", "Use it; add it to the list of sources");
  mvprintw (j++, 0, "%s\t\t%s", "w      ", "Make another search with new keywords");
  mvprintw (j++, 0, "%s\t\t%s", "o      ", "Set and show swais options");
  mvprintw (j++, 0, "%s\t\t%s", "h      ", "Show this help display");
  mvprintw (j++, 0, "%s\t\t%s", "H      ", "Display program history");
  mvprintw (j++, 0, "%s\t\t%s", "q      ", "Leave this program");
  PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue");
  getch ();
}

int
get_input_string (str)
     char            str[STRINGSIZE];
{
  int             startcol, startline;
  int             pos;
  char            ch;

  getyx (stdscr, startline, startcol);
  addstr (str);
  clrtoeol ();
  refresh ();

  pos = strlen (str);
  while (TRUE) {

    ch = getch ();
    if (ch == killchar ()) {
      move (startline, startcol);
      clrtoeol ();
      refresh ();
      str[0] = 0;
      pos = 0;
    } else if ((ch == erasechar ()) || (ch == 127)) {
      if (pos > 0) {
	str[--pos] = '\0';
	mvaddch (startline, startcol + pos, ' ');
	move (startline, startcol + pos);
	refresh ();
      } else {
	str[0] = '\0';
	break;
      }
    } else if (ch == '\n')
      break;
    else if (isprint (ch)) {
      str[pos++] = ch;
      str[pos] = '\0';
      addch (ch);
      refresh ();
    }
  }

  display_prompt ("");
  return (str[0] != '\0');
}

void
display_source_selected (sourcelist, currentsource)
     SList           sourcelist;
     Source          currentsource;
{
  if (NULL != FindSource (sourcelist, currentsource->name)) {
    mvprintw (selection_line (selection), 5, "*");
  } else {
    mvprintw (selection_line (selection), 5, " ");
  }
}

void
source_selected (sourcelist, currentsource, sel)
     SList           sourcelist;
     Source          currentsource;
{
  if (NULL != FindSource (sourcelist, currentsource->name)) {
    select_line[sel][5] = '*';
  } else {
    select_line[sel][5] = ' ';
  }
}

programstate
nextstate (question)
     Question        question;
{
  if ((question->numsources == 0) || !(question->modified)) {
    return (GETSOURCES);
  }
  return (GETKEYWORDS);
}

programstate
makequery_state (question)
     Question        question;
{
  SList           asource;

  /* rebuild question source list from Selected_Sources */
  FreeSourceList (question->Sources);
  question->Sources = NULL;
  /* build new question source list */
  for (asource = Selected_Sources;
       asource != NULL; asource = asource->nextSource)
    if (asource->thisSource != NULL)
      AddQuestionSource (question, asource->thisSource->name);
  question->numsources = listlength ((List) question->Sources);

  question->RelevantDocuments = Mydocs;
  question->numdocs = listlength ((List) question->RelevantDocuments);
  question->ResultDocuments = NULL;
  question->numresdocs = listlength ((List) question->ResultDocuments);
  question->maxresdocs = *options[6].var.intptr;

  SearchWais (question);
  question->modified = FALSE;

  if (question->numresdocs > 0) {
    result_selection = 1;
    return (SHOWRESULTS);
  } else
    return (nextstate (question));
}

void
option_screen (question)
     Question        question;
{
  DocList         doclist;
  DocumentID      current_doc;
  int             k;
  int             option_count;
  char            fstr[STRINGSIZE];
  char            tstr[STRINGSIZE];
  char           *sourcename;

  option_count = MAXOPTIONS;

  title_display ("Option Settings", "Options:", option_count);

  mvprintw (1, 2, "#    Option        Value ");
  sprintf (fstr, "%%03d:   %%-12.12s  %%-%d.%ds", COLS - 22, COLS - 22);

  for (k = 1; k <= option_count; k++) {
    format_option (tstr, &options[k]);
    sprintf (select_line[k], fstr, k, options[k].name, tstr);
  }

  selection = option_selection;
  current_page = -1;
  max_selection = option_count;
  page_length = MINIMUM (LINES - 6, option_count);
  selection_display (NOSELECTION);

}

programstate
option_state (question)
     Question        question;
{

  char            user_key[STRINGSIZE];
  char            digit_str[STRINGSIZE];
  int             k, miss, option_count;

  option_count = MAXOPTIONS + 1;
  selection_display (selection);
  display_context (options[selection].desc);

  digit_str[0] = '\0';
  while (TRUE) {
    display_prompt ("<space> to change, arrows to move, s for sources, r for results, ? for help");
    user_key[0] = getch ();
    switch (user_key[0]) {

    case 'h':
      ;
    case '?':
      show_search_results_help ();
      state = UNKNOWN;
      return (GETOPTIONS);

      /* forgive me: hardwired arrow keys for vt100 since we're
         not using SysV curses and bsd curses lacks key support  */
    case '\033':
      user_key[1] = getch ();
      user_key[2] = getch ();
      user_key[3] = '\0';
      if ((strcmp (user_key, "\033[A") == 0) ||
	  (strcmp (user_key, "\033OA") == 0)) {
	selection_display (selection - 1);
	display_context (options[selection].desc);
	option_selection = selection;
	break;
      }
      if ((strcmp (user_key, "\033[B") == 0) ||
	  (strcmp (user_key, "\033OB") == 0)) {
	selection_display (selection + 1);
	display_context (options[selection].desc);
	option_selection = selection;
	break;
      }
      /* emacs <meta>v or left arrow = page previous */
      if (user_key[1] == 'v' ||
	  (strcmp (user_key, "\033[D") == 0) ||
	  (strcmp (user_key, "\033OD") == 0)) {
	selection_display (selection - page_length);
	display_context (options[selection].desc);
	option_selection = selection;
	break;
      }
      /* right arrow = page next */
      if ((strcmp (user_key, "\033[C") == 0) ||
	  (strcmp (user_key, "\033OC") == 0)) {
	selection_display (selection + page_length);
	display_context (options[selection].desc);
	option_selection = selection;
	break;
      }
      break;

    case 10:
      ;
    case ' ':
      k = selection;
      switch (options[k].type) {
      case OPTION_INTEGER:
	digit_str[0] = '\0';
	mvprintw (LINES - 3, 0, "New Value: ");
	get_input_string (digit_str);
	if (atoi (digit_str) != 0) {
	  *(options[k].var.intptr) = atoi (digit_str);
	  options[k].update ();
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
	break;
      case OPTION_STRING:
	digit_str[0] = '\0';
	mvprintw (LINES - 3, 0, "New Value: ");
	if (get_input_string (digit_str)) {
	  strcpy (options[k].var.charptr, digit_str);
	  options[k].update ();
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
	break;
      case OPTION_BOOLEAN:
	if (*(options[k].var.intptr) == TRUE)
	  *(options[k].var.intptr) = FALSE;
	else
	  *(options[k].var.intptr) = TRUE;
	options[k].update ();
	break;
      }
      state = UNKNOWN;
      return (GETOPTIONS);

    case 14:
      ;
    case 'j':
      selection_display (selection + 1);
      display_context (options[selection].desc);
      option_selection = selection;
      break;

    case 16:
      ;
    case 'k':
      selection_display (selection - 1);
      display_context (options[selection].desc);
      option_selection = selection;
      break;

    case 'q':
      return (LEAVEPROGRAM);
    case 's':
      return (GETSOURCES);
    case 'r':
      if (question->numresdocs > 0)
	return (SHOWRESULTS);
      break;
    case 'w':
      return (GETKEYWORDS);

    case 18:
      ;
    case '\f':
      wrefresh (curscr);
      break;

    case '/':
      user_key[0] = '\0';
      mvprintw (LINES - 3, 0, "Option Name: ");
      get_input_string (user_key);
      standend ();
      mvprintw (LINES - 3, 0, " ");
      clrtoeol ();
      refresh ();
      if (strlen (user_key)) {
	miss = TRUE;
	for (k = 1; k <= option_count; k++) {
	  if (!strncasecmp (user_key, options[k].name,
			    strlen (user_key))) {
	    selection_display (k);
	    option_selection = k;
	    miss = FALSE;
	    break;
	  }
	}
	if (miss = TRUE)
	  PrintStatus (STATUS_URGENT, STATUS_HIGH,
		       "Unable to find item");
      }
      display_context (options[option_selection].desc);
      break;

    case 4:
      ;
    case 22:
      ;				/* emacs ctrl-v */
    case 'J':
      selection_display (selection + page_length);
      display_context (options[selection].desc);
      option_selection = selection;
      break;

    case 21:
      ;
    case 'K':
      selection_display (selection - page_length);
      display_context (options[selection].desc);
      option_selection = selection;
      break;

    default:
      if (isdigit (user_key[0])) {
	digit_str[0] = user_key[0];
	digit_str[1] = '\0';
	mvprintw (LINES - 3, 0, "Item Number: ");
	get_input_string (digit_str);
	if (atoi (digit_str) != 0) {
	  selection_display (atoi (digit_str));
	  option_selection = selection;
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
	display_context (options[selection].desc);
      }
    }
  }
}

void
source_screen (question)
     Question        question;
{
  int             k;
  int             source_count;
  Source          current_s;
  SList           asource;
  char            fstr[STRINGSIZE];

  source_count = listlength ((List) Sources);

  title_display ("Source Selection", "Sources:", source_count);
  mvprintw (1, 2, "#            Server                          Source                      Cost");

  sprintf (fstr, "%%03d:   [%%20.20s]  %%-%d.%ds %%16.16s", COLS - 49, COLS - 49);
  k = 1;
  for (asource = Sources; asource != NULL; asource = asource->nextSource) {

    char            tmpstr[STRINGSIZE];
    char            cost_str[STRINGSIZE];

    current_s = asource->thisSource;
    tmpstr[0] = '\0';
    if (strstr (current_s->name, ".src") != NULL)
      strncat (tmpstr, current_s->name, strlen (current_s->name) - 4);
    else
      strcat (tmpstr, current_s->name);

    format_source_cost (cost_str, current_s);
    sprintf (select_line[k], fstr, k, current_s->server, tmpstr, cost_str);
    source_selected (Selected_Sources, current_s, k);
    k++;
  }
  selection = source_selection;
  current_page = -1;
  max_selection = source_count;
  page_length = MINIMUM (LINES - 6, source_count);
  selection_display (NOSELECTION);

  mvprintw (LINES - 3, 0, "Keywords:");
  mvprintw (LINES - 3, 10, "%s", question->keywords);
  refresh ();
}

programstate
source_state (question)
     Question        question;
{
  int             miss, k;

  /* int source_count; */
  /* Source current_s; */
  SList           asource;
  char            user_key[STRINGSIZE];
  char            digit_str[STRINGSIZE];
  char            message[STRINGSIZE];

  /* source_count = listlength((List)Sources); */
  selection_display (selection);

  digit_str[0] = '\0';

  while (TRUE) {

    display_prompt ("<space> selects, w for keywords, arrows move, <return> searches, q quits, or ?");
    user_key[0] = getch ();
    switch (user_key[0]) {

    case 'H':
      show_swais_history ();
      state = UNKNOWN;
      return (GETSOURCES);

    case 'h':
      ;
    case '?':
      show_source_entry_help ();
      state = UNKNOWN;
      return (GETSOURCES);

      /* forgive me: hardwired arrow keys for vt100 since we're
         not using SysV curses and bsd curses lacks key support  */
    case '\033':
      user_key[1] = getch ();
      user_key[2] = getch ();
      user_key[3] = '\0';
      if ((strcmp (user_key, "\033[A") == 0) ||
	  (strcmp (user_key, "\033OA") == 0)) {
	selection_display (selection - 1);
	source_selection = selection;
	break;
      }
      if ((strcmp (user_key, "\033[B") == 0) ||
	  (strcmp (user_key, "\033OB") == 0)) {
	selection_display (selection + 1);
	source_selection = selection;
	break;
      }
      /* emacs <meta>v or left arrow = page previous */
      if (user_key[1] == 'v' ||
	  (strcmp (user_key, "\033[D") == 0) ||
	  (strcmp (user_key, "\033OD") == 0)) {
	selection_display (selection - page_length);
	source_selection = selection;
	break;
      }
      /* right arrow = page next */
      if ((strcmp (user_key, "\033[C") == 0) ||
	  (strcmp (user_key, "\033OC") == 0)) {
	selection_display (selection + page_length);
	source_selection = selection;
	break;
      }
      break;

    case 'o':
      return (GETOPTIONS);

    case ',':
      ;
    case 'v':
      k = 0;
      for (asource = Sources;
	   asource != NULL; asource = asource->nextSource) {
	k++;
	if (k == selection)
	  break;
      }
      view_source_info (Selected_Sources, asource->thisSource);
      state = UNKNOWN;
      return (GETSOURCES);

    case 'X':
      ;
    case '-':
      k = 0;
      for (asource = Sources;
	   asource != NULL; asource = asource->nextSource) {
	k++;
	if (k == selection)
	  break;
      }
      sprintf (fpath, "%s%s.src", sdir, firstphrase (trim_junk (asource->thisSource->name)));
      PrintStatus (STATUS_URGENT, STATUS_HIGH,
		   "Removing Source: %s\n", firstphrase (trim_junk (asource->thisSource->name)));

      if (unlink (fpath) == 0) {
	if (FindSource (Selected_Sources, asource->thisSource->name)) {

	  Selected_Sources = DeleteSource (Selected_Sources, asource->thisSource);
	  question->numsources--;
	  question->modified = TRUE;
	}
	FreeSources (Sources);
	Sources = NULL;
	ReadSourceDirectory (sdir, TRUE);
	ReadSourceDirectory (cdir, TRUE);
	state = UNKNOWN;
	return (GETSOURCES);
      }
      PrintStatus (STATUS_URGENT, STATUS_HIGH,
		   "Unable to remove common sources");
      break;

    case '/':
      user_key[0] = '\0';
      mvprintw (LINES - 3, 0, "Source Name: ");
      get_input_string (user_key);
      standend ();
      mvprintw (LINES - 3, 0, " ");
      clrtoeol ();
      refresh ();
      if (strlen (user_key)) {
	k = 0;
	miss = TRUE;
	for (asource = Sources;
	     asource != NULL; asource = asource->nextSource) {
	  k++;
	  if (asource->thisSource != NULL)
	    if (!strncasecmp (user_key,
			    asource->thisSource->name, strlen (user_key))) {
	      selection_display (k);
	      source_selection = k;
	      miss = FALSE;
	      break;
	    }
	}
	if (miss)
	  PrintStatus (STATUS_URGENT, STATUS_HIGH,
		       "Unable to find source");
      }
      break;

    case '=':
      k = 0;
      for (asource = Sources;
	   asource != NULL; asource = asource->nextSource) {
	k++;
	if (FindSource (Selected_Sources,
			asource->thisSource->name)) {

	  Selected_Sources = DeleteSource (Selected_Sources, asource->thisSource);
	  question->numsources--;
	  question->modified = TRUE;
	  display_source_selected (Selected_Sources, asource->thisSource);
	  source_selected (Selected_Sources, asource->thisSource, k);
	  selection_display (k);
	}
      }
      break;

    case '.':
      ;
    case ' ':
      k = 0;
      for (asource = Sources;
	   asource != NULL; asource = asource->nextSource) {
	k++;
	if (k == selection)
	  break;
      }
      if (FindSource (Selected_Sources, asource->thisSource->name)) {

	Selected_Sources = DeleteSource (Selected_Sources, asource->thisSource);
	question->numsources--;
	question->modified = TRUE;
      } else {
	Selected_Sources = AddSource (Selected_Sources, asource->thisSource);
	question->numsources++;
	question->modified = TRUE;
      }
      display_source_selected (Selected_Sources, asource->thisSource);
      source_selected (Selected_Sources, asource->thisSource, selection);
      selection_display (selection);
      break;

    case 14:
      ;
    case 'j':
      selection_display (selection + 1);
      source_selection = selection;
      break;

    case 16:
      ;
    case 'k':
      selection_display (selection - 1);
      source_selection = selection;
      break;
    case 'q':
      return (LEAVEPROGRAM);
    case 'r':
      if (question->numresdocs > 0)
	return (SHOWRESULTS);
      break;
    case 's':
      return (GETSOURCES);
    case 'R':
      return (RELDOCS);
    case 10:
      k = 0;
      for (asource = Sources;
	   asource != NULL; asource = asource->nextSource) {
	k++;
	if (k == selection)
	  break;
      }
      if (NULL == FindSource (Selected_Sources, asource->thisSource)) {

	Selected_Sources = AddSource (Selected_Sources, asource->thisSource);
	question->numsources++;
	question->modified = TRUE;
      }
      display_source_selected (Selected_Sources, asource->thisSource);
      source_selected (Selected_Sources, asource->thisSource, selection);
      selection_display (selection);
      return (nextstate (question));

    case 'w':
      return (GETKEYWORDS);

    case 18:
      ;
    case '\f':
      wrefresh (curscr);
      break;

    case 4:
      ;
    case 22:
      ;				/* ctrl-v */
    case 'J':
      selection_display (selection + page_length);
      source_selection = selection;
      break;

    case 21:
      ;
    case 'K':
      selection_display (selection - page_length);
      source_selection = selection;
      break;

    default:
      if (isdigit (user_key[0])) {
	digit_str[0] = user_key[0];
	digit_str[1] = '\0';
	mvprintw (LINES - 3, 0, "Source Number: ");
	get_input_string (digit_str);
	if (atoi (digit_str) != 0) {
	  selection_display (atoi (digit_str));
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
	mvprintw (LINES - 3, 0, "Keywords:");
	mvprintw (LINES - 3, 10, "%s", question->keywords);
      }
    }
  }
}

void
mail_command (question)
     Question        question;
{
  char            tmpstr[STRINGSIZE];

  display_prompt ("Enter your e-mail address; ^C to cancel");
  standout ();
  tmpstr[0] = '\0';
  strcat (tmpstr, "mail ");
  mvprintw (LINES - 3, 0, "Address:");
  standend ();
  move (LINES - 3, 9);
  if (get_input_string (command)) {
    standend ();
    mvprintw (LINES - 3, 0, "Address:");
    refresh ();
    strcat (tmpstr, command);
    mvprintw (LINES - 2, 0, tmpstr);
    PipeWaisDocument (question, findDoc (question->ResultDocuments, selection - 1), tmpstr);
    cbreak ();
    screenstart ();
  }
  standend ();
  mvprintw (LINES - 3, 0, "\n");
  display_prompt ("");
  refresh ();
}

void
pipe_command (question)
     Question        question;
{
  display_prompt ("Enter the command to be executed on this item; ^C to cancel");
  standout ();
  mvprintw (LINES - 3, 0, "Command:");
  standend ();
  move (LINES - 3, 9);
  if (get_input_string (command)) {
    standend ();
    mvprintw (LINES - 3, 0, "Command:");
    refresh ();
    PipeWaisDocument (question, findDoc (question->ResultDocuments, selection - 1), command);
    cbreak ();
    noecho ();
    PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue");
    getch ();
    cbreak ();
    screenstart ();
  }
  standend ();
  mvprintw (LINES - 3, 0, "\n");
  display_prompt ("");
  refresh ();
}
void
save_command (question)
     Question        question;
{
  char            cbuffer[100];

  display_prompt ("Enter the filename into which to save this item; ^C to cancel");
  standout ();
  mvprintw (LINES - 3, 0, "File:");
  standend ();
  move (LINES - 3, 6);
  if (get_input_string (command)) {
    standend ();
    mvprintw (LINES - 3, 0, "File:");
    refresh ();
    sprintf (cbuffer, "cat >> %s", command);
    PipeWaisDocument (question, findDoc (question->ResultDocuments, selection - 1), cbuffer);
    cbreak ();
    noecho ();
    PrintStatus (STATUS_URGENT, STATUS_HIGH, "Press any key to continue");
    getch ();
    cbreak ();
    screenstart ();
  }
  standend ();
  mvprintw (LINES - 3, 0, "\n");
  display_prompt ("");
  refresh ();
}

keyword_state (question)
     Question        question;
{
  display_prompt ("Enter keywords with spaces between them; <return> to search; ^C to cancel");
  standout ();
  mvprintw (LINES - 3, 0, "Keywords:");
  standend ();
  move (LINES - 3, 10);
  strcpy (keywords, question->keywords);
  if (get_input_string (keywords)) {
    standend ();
    mvprintw (LINES - 3, 0, "Keywords:");
    refresh ();
    strcpy (question->keywords, keywords);
    question->modified = TRUE;
    return (MAKEQUERY);
  }
  standend ();
  mvprintw (LINES - 3, 0, "Keywords:");
  mvprintw (LINES - 3, 10, "%s", question->keywords);
  refresh ();
  return (GETSOURCES);
}
void
docs_screen (question)
     Question        question;
{
  DocList         doclist;
  DocumentID      current_doc;
  int             k;
  int             doc_count;
  char            fstr[STRINGSIZE];
  char           *sourcename;

  doc_count = listlength ((List) Mydocs);
  title_display ("Rel. Documents", "Items:", doc_count);
  if (option_widetitles == FALSE) {
    mvprintw (1, 2, "#    Score     Source                       Title Lines");
    sprintf (fstr, "%%03d:   [%%4d] (%%15.15s)  %%-%d.%ds %%5ld\n", COLS - 40, COLS - 40);
  } else {
    mvprintw (1, 2, "#    Score                               Title Lines");
    sprintf (fstr, "%%03d:   [%%4d]  %%-%d.%ds %%5ld\n", COLS - 22, COLS - 22);
  }

  if (Mydocs != NULL) {
    k = 1;
    for (doclist = Mydocs; doclist != NULL;
	 doclist = doclist->nextDoc) {

      current_doc = doclist->thisDoc;
      if (current_doc != NULL) {
	sourcename = firstphrase (trim_junk (current_doc->doc->source));
	if (NULL != strrchr (sourcename, '/')) {
	  sourcename = (char *) strrchr (sourcename, '/');
	  sourcename++;
	}
	if (option_widetitles == TRUE)
	  sprintf (select_line[k], fstr, k, current_doc->rawScore,
		   firstphrase (trim_junk (current_doc->doc->headline)),
		   current_doc->doc->numLines);
	else
	  sprintf (select_line[k], fstr, k, current_doc->rawScore, sourcename,
		   firstphrase (trim_junk (current_doc->doc->headline)),
		   current_doc->doc->numLines);
	k++;
      }
    }
  }
  selection = relevant_selection;
  current_page = -1;
  max_selection = doc_count;
  page_length = MINIMUM (LINES - 6, doc_count);
  selection_display (NOSELECTION);
}

void
result_screen (question)
     Question        question;
{
  DocList         doclist;
  DocumentID      current_doc;
  int             k;
  int             doc_count;
  char            fstr[STRINGSIZE];
  char           *sourcename;

  doc_count = question->numresdocs;

  title_display ("Search Results", "Items:", doc_count);

  if (option_widetitles == FALSE) {
    mvprintw (1, 2, "#    Score     Source                       Title                       Lines");
    sprintf (fstr, "%%03d:   [%%4d] (%%15.15s)  %%-%d.%ds %%5ld\n", COLS - 40, COLS - 40);
  } else {
    mvprintw (1, 2, "#    Score                               Title                          Lines");
    sprintf (fstr, "%%03d:   [%%4d]  %%-%d.%ds %%5ld\n", COLS - 22, COLS - 22);
  }

  if (question->ResultDocuments != NULL) {
    k = 1;
    for (doclist = question->ResultDocuments; doclist != NULL;
	 doclist = doclist->nextDoc) {

      current_doc = doclist->thisDoc;
      if (current_doc != NULL) {
	sourcename = firstphrase (trim_junk (current_doc->doc->source));
	if (NULL != strrchr (sourcename, '/')) {
	  sourcename = (char *) strrchr (sourcename, '/');
	  sourcename++;
	}
	if (option_widetitles == TRUE)
	  sprintf (select_line[k], fstr, k, current_doc->rawScore,
		   firstphrase (trim_junk (current_doc->doc->headline)),
		   current_doc->doc->numLines);
	else
	  sprintf (select_line[k], fstr, k, current_doc->rawScore, sourcename,
		   firstphrase (trim_junk (current_doc->doc->headline)),
		   current_doc->doc->numLines);
	k++;
      }
    }
  }
  selection = result_selection;
  current_page = -1;
  max_selection = doc_count;
  page_length = MINIMUM (LINES - 6, doc_count);
  selection_display (NOSELECTION);
}
programstate
docs_state (question)
     Question        question;
{

  DocList         doclist;
  DocumentID      current_doc;
  char            user_key[STRINGSIZE];
  char            digit_str[STRINGSIZE];
  int             k, miss, tmpval;

  selection_display (selection);

  digit_str[0] = '\0';
  while (TRUE) {
    display_prompt ("<space> selects, arrows move, w keywords, r results, s sources, ? for help");
    user_key[0] = getch ();
    switch (user_key[0]) {

    case 'q':
      return (LEAVEPROGRAM);
    case '\033':
      user_key[1] = getch ();
      user_key[2] = getch ();
      user_key[3] = '\0';
      if ((strcmp (user_key, "\033[A") == 0) ||
	  (strcmp (user_key, "\033OA") == 0)) {
	selection_display (selection - 1);
	relevant_selection = selection;
	break;
      }
      if ((strcmp (user_key, "\033[B") == 0) ||
	  (strcmp (user_key, "\033OB") == 0)) {
	selection_display (selection + 1);
	relevant_selection = selection;
	break;
      }
      /* emacs <meta>v or left arrow = page previous */
      if (user_key[1] == 'v' ||
	  (strcmp (user_key, "\033[D") == 0) ||
	  (strcmp (user_key, "\033OD") == 0)) {
	selection_display (selection - page_length);
	relevant_selection = selection;
	break;
      }
      /* right arrow = page next */
      if ((strcmp (user_key, "\033[C") == 0) ||
	  (strcmp (user_key, "\033OC") == 0)) {
	selection_display (selection + page_length);
	relevant_selection = selection;
	break;
      }
    case 'X':
      tmpval = freeRelDoc (Mydocs, relevant_selection - 1);
      state = UNKNOWN;
      if (tmpval)
	return (RELDOCS);
      else
	return (GETKEYWORDS);

    case 'D':			/* delete all relevant docs */
      freeDocList (Mydocs);
      Mydocs = NULL;
      selection = -1;
      relevant_selection = 1;
      return (GETKEYWORDS);

    case 'o':
      return (GETOPTIONS);

    case 's':
      return (GETSOURCES);
    case 'w':
      return (GETKEYWORDS);
    case 'r':
      return (SHOWRESULTS);
    case 'H':
      show_swais_history ();
      state = UNKNOWN;
      return (RELDOCS);

    case 'h':
      ;
    case '?':
      show_relevant_help ();
      state = UNKNOWN;
      return (RELDOCS);

    case 14:
      ;
    case 'j':
      selection_display (selection + 1);
      relevant_selection = selection;
      break;

    case 16:
      ;
    case 'k':
      selection_display (selection - 1);
      relevant_selection = selection;
      break;

    default:
      if (isdigit (user_key[0])) {
	digit_str[0] = user_key[0];
	digit_str[1] = '\0';
	mvprintw (LINES - 3, 0, "Item Number: ");
	get_input_string (digit_str);
	if (atoi (digit_str) != 0) {
	  selection_display (atoi (digit_str));
	  relevant_selection = selection;
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
      }
    }
  }
}
programstate
result_state (question)
     Question        question;
{

  DocList         doclist;
  DocumentID      current_doc;
  char            user_key[STRINGSIZE];
  char            digit_str[STRINGSIZE];
  int             k, miss;

  selection_display (selection);

  digit_str[0] = '\0';
  while (TRUE) {
    display_prompt ("<space> selects, arrows move, w for keywords, s for sources, ? for help");
    user_key[0] = getch ();
    switch (user_key[0]) {

    case 'H':
      show_swais_history ();
      state = UNKNOWN;
      return (SHOWRESULTS);

    case 'h':
      ;
    case '?':
      show_search_results_help ();
      state = UNKNOWN;
      return (SHOWRESULTS);

      /* forgive me: hardwired arrow keys for vt100 since we're
         not using SysV curses and bsd curses lacks key support  */
    case '\033':
      user_key[1] = getch ();
      user_key[2] = getch ();
      user_key[3] = '\0';
      if ((strcmp (user_key, "\033[A") == 0) ||
	  (strcmp (user_key, "\033OA") == 0)) {
	selection_display (selection - 1);
	result_selection = selection;
	break;
      }
      if ((strcmp (user_key, "\033[B") == 0) ||
	  (strcmp (user_key, "\033OB") == 0)) {
	selection_display (selection + 1);
	result_selection = selection;
	break;
      }
      /* emacs <meta>v or left arrow = page previous */
      if (user_key[1] == 'v' ||
	  (strcmp (user_key, "\033[D") == 0) ||
	  (strcmp (user_key, "\033OD") == 0)) {
	selection_display (selection - page_length);
	result_selection = selection;
	break;
      }
      /* right arrow = page next */
      if ((strcmp (user_key, "\033[C") == 0) ||
	  (strcmp (user_key, "\033OC") == 0)) {
	selection_display (selection + page_length);
	result_selection = selection;
	break;
      }
    case 'o':
      return (GETOPTIONS);

    case ',':
      ;
    case 'v':
      view_result_info (findDoc (question->ResultDocuments, selection - 1));
      state = UNKNOWN;
      return (SHOWRESULTS);
    case 'V':
      break;

    case 'u':
      UseWaisDocument (question, findDoc (question->ResultDocuments, selection - 1));
      break;

    case 10:
      ;
    case ' ':
      DisplayWaisDocument (question,
			findDoc (question->ResultDocuments, selection - 1));
/*
   PipeWaisDocument(question, 
   findDoc(question->ResultDocuments,selection-1), 
   "${PAGER-more}");
   if (option_pagerpause==TRUE) {
   cbreak();
   noecho();
   PrintStatus(STATUS_URGENT, STATUS_HIGH, 
   "Press any key to continue"); 
   getch();
   }
 */
      screenstart ();
      state = UNKNOWN;
      return (SHOWRESULTS);
    case 'S':
      save_command (question);
      state = UNKNOWN;
      return (SHOWRESULTS);
      break;
    case 'R':
      return (RELDOCS);
    case 'r':
      AddDocAsRelevant (findDoc (question->ResultDocuments, selection - 1));
      state = UNKNOWN;
      break;
#ifndef SECURECLIENT
    case '|':
      ;
    case 'c':
      pipe_command (question);
      state = UNKNOWN;
      return (SHOWRESULTS);
#endif
    case 14:
      ;
    case 'j':
      selection_display (selection + 1);
      result_selection = selection;
      break;

    case 16:
      ;
    case 'k':
      selection_display (selection - 1);
      result_selection = selection;
      break;

    case 'q':
      return (LEAVEPROGRAM);
    case 's':
      return (GETSOURCES);
    case 'w':
      return (GETKEYWORDS);

    case 18:
      ;
    case '\f':
      wrefresh (curscr);
      break;

    case '/':
      user_key[0] = '\0';
      mvprintw (LINES - 3, 0, "Item Name: ");
      get_input_string (user_key);
      standend ();
      mvprintw (LINES - 3, 0, " ");
      clrtoeol ();
      refresh ();
      if (strlen (user_key)) {
	k = 0;
	miss = TRUE;
	for (doclist = question->ResultDocuments;
	     doclist != NULL; doclist = doclist->nextDoc) {
	  current_doc = doclist->thisDoc;
	  k++;
	  if (current_doc != NULL)
	    if (!strncasecmp (user_key, current_doc->doc->headline,
			      strlen (user_key))) {
	      selection_display (k);
	      result_selection = k;
	      miss = FALSE;
	      break;
	    }
	}
	if (miss)
	  PrintStatus (STATUS_URGENT, STATUS_HIGH,
		       "Unable to find item");
      }
      break;

    case 'm':
      mail_command (question);
      state = UNKNOWN;
      return (SHOWRESULTS);

    case 4:
      ;
    case 22:
      ;				/* emacs ctrl-v */
    case 'J':
      selection_display (selection + page_length);
      result_selection = selection;
      break;

    case 21:
      ;
    case 'K':
      selection_display (selection - page_length);
      result_selection = selection;
      break;

    default:
      if (isdigit (user_key[0])) {
	digit_str[0] = user_key[0];
	digit_str[1] = '\0';
	mvprintw (LINES - 3, 0, "Item Number: ");
	get_input_string (digit_str);
	if (atoi (digit_str) != 0) {
	  selection_display (atoi (digit_str));
	  result_selection = selection;
	}
	standend ();
	mvprintw (LINES - 3, 0, " ");
	clrtoeol ();
      }
    }
  }
}

void
main (argc, argv)
     int             argc;
     char          **argv;
{
  Question        question;
  SList           asource;
  char            msg[STRINGSIZE];
  char           *getenv ();
  char            sourcename[STRINGSIZE];
  int             i, doc_count, maxDocs;

  /*Mydocs=makeDocList(NULL,NULL); */
  if (command_name = (char *) rindex (argv[0], '/'))
    command_name++;
  else
    command_name = argv[0];

  command[0] = '\0';

  sdir = cdir = NULL;
  keywords[0] = 0;
  sourcename[0] = 0;

  maxDocs = 40;

  i = 1;
  for (; i < argc; i++) {
    if (*argv[i] == '-') {
      argv[i]++;
      switch (*argv[i]) {
      case 'C':
	i++;
	if (i >= argc) {
	  fprintf (stderr, "Too few arguments: common source directory missing.\n");
	  exit (1);
	}
	cdir = argv[i];
	break;
      case 'S':
	i++;
	if (i >= argc) {
	  fprintf (stderr, "Too few arguments: user source directory missing.\n");
	  exit (1);
	}
	sdir = argv[i];
	break;
      case 's':
	i++;
	if (i >= argc) {
	  fprintf (stderr, "Too few arguments: source name missing.\n");
	  exit (1);
	}
	sprintf (sourcename, "%s.src", argv[i]);
	break;
      case 'h':
	usage (argv[0]);
	exit (0);
	break;
      default:
	fprintf (stderr, "Unknown option: %s.\n", argv[i]);
	exit (1);
      }
    } else {
      if ((strlen (keywords) + strlen (argv[i]) + 1) < STRINGSIZE) {
	if (strlen (keywords))
	  strcat (keywords, " ");
	strcat (keywords, argv[i]);
      }
    }
  }

  if (sdir == NULL) {
    if ((sdir = getenv ("WAISSOURCEDIR")) == NULL) {
      sprintf (msg, "%s/wais-sources/", getenv ("HOME"));
      sdir = s_strdup (msg);
    }
  }
  sdir = fixdirname (sdir);
  ReadSourceDirectory (sdir, TRUE);

  if (cdir == NULL) {
    if ((cdir = getenv ("WAISCOMMONSOURCEDIR")) == NULL) {
      strcpy (msg, COMMON_SOURCE_DIR);
      cdir = s_strdup (msg);
    }
  }
  cdir = fixdirname (cdir);
  ReadSourceDirectory (cdir, TRUE);

  Refresh_sources ();

  strcpy (options[1].name, "widetitles");
  strcpy (options[1].desc, "Show wide Item titles on Results Screen by omitting source name");
  options[1].type = OPTION_BOOLEAN;
  options[1].var.intptr = &option_widetitles;
  options[1].update = null;

  strcpy (options[2].name, "sortsources");
  strcpy (options[2].desc, "Display sources sorted alphabetically rather than directory order");
  options[2].type = OPTION_BOOLEAN;
  options[2].var.intptr = &option_sortsources;
  options[2].update = Refresh_sources;

  strcpy (options[3].name, "sourcedir");
  strcpy (options[3].desc, "Personal WAIS source directory containing *.src description files");
  options[3].type = OPTION_STRING;
  options[3].var.charptr = sdir;
  options[3].update = Refresh_sources;

  strcpy (options[4].name, "commondir");
  strcpy (options[4].desc, "Common WAIS source directory containing *.src description files");
  options[4].type = OPTION_STRING;
  options[4].var.charptr = cdir;
  options[4].update = Refresh_sources;

  strcpy (options[5].name, "pagerpause");
  strcpy (options[5].desc, "Pause after displaying a document via the external pager");
  options[5].type = OPTION_BOOLEAN;
  options[5].var.intptr = &option_pagerpause;
  options[5].update = null;

  strcpy (options[6].name, "maxitems");
  strcpy (options[6].desc, "Maximum items (documents) returned in a single query");
  options[6].type = OPTION_INTEGER;
  options[6].var.intptr = &maxDocs;
  options[6].update = null;

  question = (Question) s_malloc (sizeof (_Question));
  question->numsources = 0;
  question->modified = FALSE;

  screenstart ();

  state = UNKNOWN;

  if (sourcename[0] != 0)
    for (asource = Sources; asource != NULL; asource = asource->nextSource)
      if (!strcmp (sourcename, asource->thisSource->name)) {
	Selected_Sources = AddSource (Selected_Sources, asource->thisSource);
	question->numsources++;
	question->modified = TRUE;
      }
  if (keywords[0] != '\0') {
    strcpy (question->keywords, keywords);
    question->modified = TRUE;
  }
  new_state = nextstate (question);

  if (setjmp (main_env) != 0) {
    state = UNKNOWN;
    new_state = GETSOURCES;
  }
  while (TRUE) {

    if (new_state != state) {
      if ((new_state / 10) != (state / 10)) {
	clear ();
	switch (new_state) {
	case GETSOURCES:
	  ;
	case GETKEYWORDS:
	  ;
	case MAKEQUERY:
	  source_screen (question);
	  break;
	case SHOWRESULTS:
	  result_screen (question);
	  break;
	case GETOPTIONS:
	  option_screen (question);
	  break;
	case RELDOCS:
	  doc_count = listlength ((List) Mydocs);
	  if (doc_count > 0)
	    docs_screen (question);
	  else
	    source_screen (question);
	  break;
	case LEAVEPROGRAM:
	  break;
	}
      } else {
	selection_display (NOSELECTION);
	display_prompt ("");
      }
      state = new_state;
    }
    switch (state) {

    case MAKEQUERY:
      new_state = makequery_state (question);
      break;

    case GETKEYWORDS:
      ;
      new_state = keyword_state (question);
      break;

    case GETSOURCES:
      new_state = source_state (question);
      break;

    case SHOWRESULTS:
      new_state = result_state (question);
      break;

    case GETOPTIONS:
      new_state = option_state (question);
      break;

    case LEAVEPROGRAM:
      screenend ();
      exit (0);
    case RELDOCS:
      doc_count = listlength ((List) Mydocs);
      if (doc_count > 0)
	new_state = docs_state (question);
      else
	new_state = source_state (question);
      break;
    }
  }
}
