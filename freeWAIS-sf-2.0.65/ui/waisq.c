/* 
  WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface tools for the WAIS software.
   Do with it as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* $Log: waisq.c,v $
 * Revision 2.0.1.2  1995/12/13 15:38:56  pfeifer
 * patch56: From: malyshki@cs.wmich.edu (Vladislav Malyshkin)
 * patch56:   The differencs between these files is :
 * patch56:     1) added +1 to s_malloc(..) and s_realloc(...)
 * patch56:     2) now waisq understands  new values for -v
 * patch56:        option. Now it is possible to use
 * patch56:        -v 10-20    (print documents # 10,11,12,.....20)
 * patch56:        and
 * patch56:        -v all      (print all)
 * patch56:        old version
 * patch56:        -v document_number
 * patch56:        also works.
 *
/* Revision 2.0.1.1  1995/09/15  09:55:49  pfeifer
/* patch7: Fixed CNIDR copyright line.
/*
/* Revision 2.0  1995/09/08  08:05:46  pfeifer
/* Metaconfig baseline
/*
/* Revision 1.4  1995/04/27  12:06:19  pfeifer
/* PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
/*
/* Revision 1.3  1995/03/13  12:18:34  pfeifer
/* fixed: waisq was assuming that the normal interactive env. variables
/* were present (HOME, etc) and dereferencing them w/o checking if
/* the were NULL
/*
 * Revision 1.2  1994/08/05  07:20:20  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:24  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.25  92/06/03  17:30:33  jonathan
 * Removed declarations for waislogfile and wais_log_file_name (in ir/cutil.h now)
 * 
 * Revision 1.24  92/04/29  13:20:27  jonathan
 * Updated for release.
 * 
 * Revision 1.23  92/04/01  17:21:27  jonathan
 * changed loadsource to findsource (which now loads if necessary).
 * 
 * Revision 1.22  92/03/17  14:36:06  jonathan
 * Modified to use new ui functions in wais-ui.  Generally cleaned up.
 * 
 * Revision 1.21  92/03/06  14:52:02  jonathan
 * New and Improved source loading!
 * 
 * Revision 1.20  92/02/29  19:40:10  jonathan
 * Only read source for waisping (not all sources).  Much faster.
 * 
 * Revision 1.19  92/02/15  19:47:52  jonathan
 * Added $Log for RCS
 * 
*/

#include "wais.h"

#define MAIN
#include "globals.h"

#define WAISQ_DATE "Thu Aug 27 1992"

/* this will take as its argument the name of a question file,
   and will do all the right things with it.
*/

#ifdef ANSI_LIKE
void PrintStatus(long logtype, long loglevel, char* format, ...)
#else /* not ANSI_LIKE */
void PrintStatus(va_alist)
va_dcl
#endif /* not ANSI_LIKE */
{
  va_list ap;			/* the variable arguments */
  char message[1000];

#ifndef ANSI_LIKE
  long logtype, loglevel;
  char *format;

  va_start(ap);
  logtype = va_arg(ap, int);
  loglevel = va_arg(ap, int);
  format = va_arg(ap, char*);
#else /* ANSI_LIKE */
  va_start(ap, format);
#endif /* ANSI_LIKE */

  vsprintf(message, format, ap);
  fprintf(stderr, "%s", message);
#ifndef ANSI_LIKE
  va_end(ap);
#endif /* not ANSI_LIKE */
}

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

usage(name) 
char *name;
{
  if (strstr(name, "waisq") != NULL) {
    fprintf(stderr,"Usage: waisq\n");
    fprintf(stderr,"  [-f question_file] /* defaults to stdin, stdout */\n");
    fprintf(stderr,"  [-s sourcedir] /* or environment variable WAISSOURCEDIR, or ~/wais-sources/ */\n");
    fprintf(stderr,"  [-S sourcename]\n");
    fprintf(stderr,"  [-c common_sourcedir] /* or environment variable WAISCOMMONSOURCEDIR */\n");
    fprintf(stderr,"  [-m max_Result_Docs] /* defaults to 40 */\n");
    fprintf(stderr,"  [-v all ! document_number[-document_number2]]\n");
    fprintf(stderr,"  [-g]	/* to do a search */\n");
    fprintf(stderr,"  [-h]	/* this message */\n");
    fprintf(stderr,"  [-t]	/* test a source */\n");
    fprintf(stderr,"  [-V]	/* print version */\n");
    fprintf(stderr,"  [word [word [...]]]\n");
    fprintf(stderr,"or %s -	/* interactive mode */\n", name);
  }
  else {
    fprintf(stderr,"Usage: waisping\n");
    fprintf(stderr,"  [-s sourcedir] /* or environment variable WAISSOURCEDIR, or ~/wais-sources/ */\n");
    fprintf(stderr,"  [-c common_sourcedir] /* or environment variable WAISCOMMONSOURCEDIR */\n");
    fprintf(stderr,"  sourcename\n");
  }
}

void SetSource(question, sourcename)
Question question;
char *sourcename;
{
  SourceList s;
  SourceID sid;

  sid = (SourceID)s_malloc(sizeof(_SourceID));
  s = (SourceList)s_malloc(sizeof(_SourceList));
  s->thisSource = sid;
  s->nextSource = NULL;
  sid->filename = sourcename;
  question->Sources = s;
}

char *
fixdirname(dir)
char *dir;
{
  char *res;

  if(dir[strlen(dir)-1] == '/') res = dir;
  else {
    res = s_malloc(strlen(dir)+2);
    sprintf(res,"%s/", dir);
  }
  return res;
}

main(argc, argv)
     int argc;
     char *argv[];
{
  Question question;
  char msg[STRINGSIZE];
  char *qfilename, *sourcename;
  char keywords[STRINGSIZE];
  int i, *document_number, numdocs, maxDocs;
  Boolean dosearch, interactive_mode, test_mode, rescan;
  FILE *fp;
  char *cdir, *sdir, *home;
#ifndef SABER
  char *getenv();
#endif /* not SABER */
  char *pw;
  int bl,br,iw;

  if (command_name = (char*)rindex(argv[0], '/'))
    command_name++;
  else
    command_name = argv[0];

  sdir = cdir = qfilename = sourcename = NULL;
  keywords[0] = 0;
  dosearch = interactive_mode = test_mode = FALSE;

  NumSources = 0;
  maxDocs = 40;

  document_number = (int*)s_malloc(sizeof(int)*(maxDocs+1));
                     /* fixed, +1 is added. <malyshki@cs.wmich.edu> */
  numdocs = 0;

  if(strstr(argv[0], "waisping") != NULL) test_mode = TRUE;

  /* parse arguments */

  if (argc == 1) {
    usage(argv[0]);
    exit(0);
  }

  /* if the first arg is '-', do interactive mode */

  i = 1;

  if((argv[1][0] == '-') && (argv[1][1] == 0)) {
    interactive_mode = TRUE;
    i++;
  }
  for(; i < argc; i++) {
    if (*argv[i] == '-') {
      argv[i]++;
      switch (*argv[i]) {
      case 'f':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: file name missing.\n");
	  exit(1);
	  }
	qfilename = argv[i];
	break;
      case 'c':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: source directory missing.\n");
	  exit(1);
	  }
	cdir = argv[i];
	break;
      case 's':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: source directory missing.\n");
	  exit(1);
	  }
	sdir = argv[i];
	break;
      case 'S':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: source name missing.\n");
	  exit(1);
	  }
	sourcename = argv[i];
	break;
      case 'm':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: source name missing.\n");
	  exit(1);
	  }
	maxDocs = atoi(argv[i]);
	document_number = (int*)s_realloc(document_number,
					 sizeof(int)*(maxDocs+1));
        /* fixed, +1 is added. <malyshki@cs.wmich.edu> */
	break;
      case 'v':
	i++;
	if(i >= argc) {
	  fprintf(stderr, "Too few arguments: Document number missing.\n");
	  exit(1);
	  }
     /*-------------------------------------------------------*/
  /* added number1-number2  and "all" as a possible values of -v */
  /*     (by Vladislav Malyshkin <malyshki@cs.wmich.edu>) */
  if(strcmp(argv[i],"all")) 
      {/* parameter -v isn't equal to "all", extract values */
        pw=strchr(argv[i],'-');
        if(NULL == pw) 
         {
          bl=atoi(argv[i]);
          br=bl;
         }
        else 
         {
          br=atoi(pw+1);
          *pw='\0';
          bl=atoi(argv[i]);
          *pw='-'; /* restore argv[i] */
         }
      }
  else
      {
       bl=1;
       br=maxDocs;
      } 
  for(iw=bl;iw<=br;iw++) if((iw>0) && (iw<=maxDocs)) 
      {
       document_number[numdocs++] = iw;
       numdocs = MINIMUM(maxDocs, numdocs);
      }
	break;
  /* end -v flag values modification */
  /*-------------------------------------------------------*/
      case 'g':
	dosearch = TRUE;
	break;
      case 'h':
	usage(argv[0]);
	exit(0);
	break;
      case 't':
	test_mode = TRUE;
	break;
      case 'V':
	printf("%s version: %s, %s.\n", argv[0], VERSION_STRING, WAISQ_DATE);
	exit(0);
      default:
	fprintf(stderr, "Unknown option: %s.\n", argv[i]);
	exit(1);
      }
    }
    else {
      if(test_mode)
	strcpy(keywords, argv[i]);
      else if((strlen(keywords) + strlen(argv[i]) + 1) < STRINGSIZE) {
	strcat(keywords, argv[i]);
	strcat(keywords, " ");
      }
    }
  }
      
  if(sdir == NULL) {
    if((sdir = getenv("WAISSOURCEDIR")) == NULL) {
	  if ((home=getenv("HOME")) != NULL) {
        sprintf(msg, "%s/wais-sources/", getenv("HOME"));
        sdir = s_strdup(msg);
      }
    }
  }

  if(cdir == NULL) cdir = getenv("WAISCOMMONSOURCEDIR");

  if(sdir != NULL) sdir = fixdirname(sdir);

  if(cdir != NULL) {
    cdir = fixdirname(cdir);
  }

  the_Question = (Question)s_malloc(sizeof(_Question));

  the_Question->maxresdocs = maxDocs;

  the_Question->sourcepath = s_malloc((sdir?strlen(sdir):0)+(cdir?strlen(cdir):0)+2);
  sprintf(the_Question->sourcepath, "%s:%s", (sdir?sdir:""), (cdir?cdir:""));

  if(test_mode) {

    question = the_Question;

    SetSource(question, keywords);
    if(findsource(keywords, question->sourcepath) == NULL) {
      PrintStatus(STATUS_URGENT, STATUS_HIGH,
		  "Can't find source: %s\n", keywords);
    }
    else
      test_connection(question);
    exit(0);
  }

  else if(!interactive_mode) {
    if (qfilename != NULL) {
      if(strcmp(qfilename, "-")) {
	if((fp = fopen(qfilename, "r")) == NULL) {
	  fprintf(stderr, "Can't open question %s\n", qfilename);
	  exit(-1);
	}
	ReadQuestion(the_Question, fp);
	fclose(fp);
      }
    }
    else {
      fp = stdin;
      ReadQuestion(the_Question, fp);
    }

    question = the_Question;

    question->modified = TRUE;

    question->numsources =
      listlength((List)question->Sources);

    question->numdocs =
      listlength((List)question->RelevantDocuments);

    question->numresdocs =
      listlength((List)question->ResultDocuments);

    if (keywords[0] != 0) {
      strcpy(question->keywords,keywords);
    }

    if (sourcename != NULL) {
      SetSource(question, sourcename);
    }

    if(dosearch == TRUE) {
      SearchWais(question);
      if((qfilename == NULL) || (strcmp(qfilename, "-") == 0))
	WriteQuestionfp(stdout, question);
      else
	WriteQuestion(qfilename, question);
    }
    else if((qfilename != NULL) &&
	    strcmp(qfilename, "-") == 0)
      WriteQuestionfp(stdout, question);

    for(i = 0; i < numdocs; i++) {
      if((document_number[i] > 0) &&
	 (document_number[i] <= question->numresdocs)) { /* view a document */
	DocumentID doc;
	char *viewbuffer;
	long size = 0;

	if((doc = findDoc(question->ResultDocuments, document_number[i]-1))
	   == NULL) {
	  PrintStatus(STATUS_URGENT, STATUS_HIGH,
		      "Unable to find document.  This should not happen.\n");
	  return;
	}
	PrintStatus(STATUS_INFO, STATUS_LOW, 
		    "Viewing Document %d: %s\n", document_number[i],
                    (doc->doc->headline == NULL) ? "NULL" : doc->doc->headline);
	GetWaisDocument(question, doc, 
			doc->doc->type[0], stdout, &size);
      }
    }
  }
  else {
    /* interactive mode!
       read questions from standard in, write to stdout, until an empty question comes through.
       */
    question = the_Question;
    while(interactive_mode) {
      if(ReadQuestion(question, stdin)) {
	question->modified = TRUE;
	question->numsources =
	  listlength((List)question->Sources);
	question->numdocs =
	  listlength((List)question->RelevantDocuments);
	question->numresdocs =
	  listlength((List)question->ResultDocuments);

	/* cases */
	if((strlen(question->keywords) != 0) ||
	   (question->numdocs > 0)) {
	  SearchWais(question);
	  WriteQuestionfp(stdout, question);
	}
	else if(question->numresdocs > 0)
	  for(i = 0; i < question->numresdocs; i++) {
	    char* viewbuffer;
	    long size = 0;
	    DocumentID doc;

	    doc = findDoc(question->ResultDocuments, i);
	    if(doc != NULL) {
	      GetWaisDocument(question, doc, doc->doc->type[0],
			      stdout, &size);
	    }
	  }
	else {
	  SearchWais(question);
	  WriteQuestionfp(stdout, question);
	}
	fflush(stdout);
	PrintStatus(STATUS_INFO, STATUS_LOW,
		    "Waisq: Ready for next question.\n");
      }
      else interactive_mode = FALSE;
    }
  }
  exit(0);
}
