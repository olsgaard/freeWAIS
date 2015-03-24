/*                               -*- Mode: C -*- 
 * 
 * cutil.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Sun Sep 10 12:29:45 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Tue Nov 14 19:02:50 1995
 * Language        : C
 * Update Count    : 14
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 */

/* Copyright (c) CNIDR (Work in progress) */

/* Wide AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
  
   3.26.90	Harry Morris, morris@think.com
   4.11.90  HWM - generalized conditional includes (see c-dialect.h)
   7/19/91  speed up substrcmp by ses@ccgr.technion.ac.il (Simon E Spero)
 *
 * $Log: cutil.c,v $
 * Revision 2.0.1.5  1995/11/14 18:03:20  pfeifer
 * patch51:
 *
 * Revision 2.0.1.4  1995/09/19  19:10:44  pfeifer
 * patch13: Changed ISO to MYCTYPE. Broke 8-bit code!
 *
 * Revision 2.0.1.3  1995/09/11  12:43:57  pfeifer
 * patch4: no change
 *
 * Revision 2.0.1.2  1995/09/10  11:19:30  pfeifer
 * patch2: Fixed check for existing vprintf.
 *
 * Revision 2.0.1.1  1995/09/08  19:10:06  pfeifer
 * patch1: Changed HAVE_STRDUP to HAS_STRDUP.
 *
 * Revision 2.0  1995/09/08  07:58:23  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.23  1995/04/17  13:15:11  huynh1
 * Release 1.1.2
 *
 * Revision 1.22  1995/02/10  10:32:41  pfeifer
 * Release 1.1.2
 *
 * Revision 1.21  1994/09/08  16:43:14  pfeifer
 * removed dead code (was ifdef'ed AUTOCONF)
 *
 * Revision 1.20  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.19  1994/08/08  07:31:52  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.17  1994/06/13  10:40:39  pfeifer
 * replaced strdup by function provided by Kent C Brodie
 * <kcb@post.its.mcw.edu>
 *
 * Revision 1.16  1994/06/10  12:49:48  pfeifer
 * typo
 *
 * Revision 1.15  1994/06/10  08:22:43  pfeifer
 * bad typo
 *
 * Revision 1.14  1994/06/10  08:15:24  pfeifer
 * added definition for strdup, wich is missing with solaris 2.3 ?
 *
 * Revision 1.13  1994/05/21  14:35:24  pfeifer
 * initalize memory, in fs_realloc if it is really an malloc (should be
 * obsolete :-( )
 *
 * Revision 1.12  1994/05/20  12:55:43  pfeifer
 * beta
 *
 * Revision 1.11  1994/03/23  15:30:58  pfeifer
 * fixed iso code
 *
 * Revision 1.10  1994/03/08  20:57:40  pfeifer
 * Patchlevel 04
 *
 * Revision 1.9  1993/09/22  16:07:52  pfeifer
 * Fixed word breaking for german ISO Umlaute and sz
 *
 * Revision 1.8  1993/07/16  12:10:19  huynh1
 * Function generate_newline_for_boolean deleted!
 *
 * Revision 1.7  1993/07/14  09:52:38  huynh1
 * keine "Anderung!
 *
 * Revision 1.6  1993/07/14  07:30:24  huynh1
 * "Anderung in der Funktion generate_newline_for_boolean: Variable umbenannt: brackets = parentheses!
 *
 * Revision 1.5  1993/07/13  19:14:43  huynh1
 * printf(".....") gel"oscht!
 *
 * Revision 1.4  1993/07/13  18:44:04  huynh1
 * Keine "Anderung!
 *
 * Revision 1.3  1993/07/13  18:40:31  huynh1
 * funktion generate_newline_for_boolean eingef"ugt!
 *
 * Revision 1.2  1993/02/16  14:59:21  freewais
 * include stdargs.h or varargs.h based on whether compiler
 * is ansi or non-ansi
 *
 * Revision 1.1  1993/02/16  14:53:14  freewais
 * Initial revision
 *
 * Revision 1.24  92/03/28  19:53:39  jonathan
 * Removed useless ifdef BSD in cprintf
 * 
 * Revision 1.23  92/03/07  19:44:39  jonathan
 * ANSIfied arguments.
 * 
 * Revision 1.22  92/03/04  16:14:23  jonathan
 * Added include <ctype.h> for isalnum.
 * 
 * Revision 1.21  92/02/21  11:03:15  jonathan
 * Fixed $Log.
 * 
 * Revision 1.20  92/02/21  11:02:02  jonathan
 * Added wais_log_level, RCSIdent
 * 
 * Revision 1.19  92/02/16  21:24:54  jonathan
 * Added code for vprintf, if needed (like for old BSD).
 * 
 * Revision 1.18  92/02/12  14:33:26  morris
 * made string_downcase not die when passed NULL
 * 
 * Revision 1.17  92/02/12  13:16:21  jonathan
 * Added $Log so RCS will put the log message in the header
 * 
 *
*/

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/cutil.c,v 2.0.1.5 1995/11/14 18:03:20 pfeifer Exp $";
#endif

#define _C_C_util_

#include "cutil.h"
#include "panic.h"

#include <ctype.h>
/*----------------------------------------------------------------------*/

/* #define MEMORY_ACCOUNTING */

#ifdef MEMORY_ACCOUNTING
#include "futil.h"
#undef s_checkPtr
#define s_checkPtr(ptr) fs_checkPtr(ptr)
static FILE* memRecord = NULL;
static void prepMemAcct _AP((void));
static void 
prepMemAcct()
{
  if (memRecord == NULL)
    memRecord = s_fopen("MemoryAccounting","w");
}
static void flushMemAcct _AP((void));
static void 
flushMemAcct()
{
  fflush(memRecord);
}
#define tickles 0
static char* 	badPtr	= (char*)-1;
static size_t	badSize	= -1;
#endif /* def MEMORY_ACCOUNTING */

/*----------------------------------------------------------------------*/

void
fs_checkPtr(ptr)
void* ptr;
/* If the ptr is NULL, give an error */
{ 
#ifdef MEMORY_ACCOUNTING
  static char** ptrs = NULL;
  long i;
  static Boolean  doneSetup = false;
  if (doneSetup == false)
    {
#ifdef THINK_C
      Debugger();
#endif /* def THINK_C */
      doneSetup = true;
    }
#endif /* def MEMORY_ACCOUNTING */

  if (ptr == NULL)
    panic("checkPtr found a NULL pointer");

#ifdef MEMORY_ACCOUNTING
/* look for specific ptr (useful when tracking un-freed memory) */
if (ptr == badPtr) warn("checking found bad ptr"); 
/* tickle a memory bug */
if (ptrs == NULL && tickles > 0)
  ptrs = (char**)malloc((size_t)tickles * sizeof(char*));
for (i = 0; i < tickles; i++)
{ ptrs[i] = malloc((size_t)5);
}
for (i = 0; i < tickles; i++)
{ free(ptrs[i]);
}
#endif /* def MEMORY_ACCOUNTING */

}

/*----------------------------------------------------------------------*/

void*
fs_malloc(size)
size_t size;
/* does safety checks and optional accounting */
{ 
  register void* ptr = NULL;

  if(!size) return(NULL);

#ifdef THINK_C
  ptr = (void*)NewPtr((long)size);
  s_checkPtr(ptr);
  memset(ptr,0,(size_t)size);	/* zero it */
#else  
  ptr = NULL;
  ptr = (void*)calloc((size_t)size,(size_t)1);
  s_checkPtr(ptr);
#endif
  
#ifdef MEMORY_ACCOUNTING
  /* look for specific size (useful when tracking un-freed memory) */
  if (size == badSize)
    warn("bad size in malloc");
  
  prepMemAcct();
  fprintf(memRecord,"malloced %lu bytes at %lu\n",size,ptr);
  flushMemAcct();
#endif

  return(ptr);
}

/*----------------------------------------------------------------------*/

void*
fs_realloc(ptr,size)
void* ptr;
size_t size;
/* does safety checks and optional accounting 
   note - we don't know how big ptr's memory is, so we can't ensure
   that any new memory allocated is NULLed!
 */
{ 
  register void* nptr = NULL;
  
  if (ptr == NULL) {		/* this is really a malloc */
    nptr = s_malloc(size);      /* Initialize (up) */
    if (nptr != NULL) bzero(nptr,size);
    return(nptr);
  }
#ifdef THINK_C
  nptr = NewPtr(size);		/* need to make a copy */ 
  s_checkPtr(nptr);
  BlockMove(ptr,nptr,size);	/* move the old contents into it */
  DisposPtr(ptr);		    /* get rid of the old ones */
#else
  nptr = (void*)realloc(ptr,size);
  s_checkPtr(ptr);
#endif  
   
#ifdef MEMORY_ACCOUNTING
  /* look for specific size (useful when tracking un-freed memory) */
  if (size == badSize) 
    warn("bad size in realloc");
  
  prepMemAcct();
  fprintf(memRecord,"realloced %lu bytes at %lu from %lu\n",size,nptr,ptr);
  flushMemAcct();
#endif

  return(nptr);
}

/*----------------------------------------------------------------------*/

void
fs_free(ptr)
void* ptr;
/* does safety checks and optional accounting */
{
#ifdef MEMORY_ACCOUNTING
  prepMemAcct();
  /* note that the sizeof a pointer is always 4.  If only we could find out
     how much space that pointer was pointing to.  Oh well, this is a place
     holder for now
   */
  fprintf(memRecord,"freed %lu bytes at %lu\n",(size_t)sizeof(ptr),ptr);
  flushMemAcct();

  /* look for specific ptr (useful when tracking un-freed memory) */
  if (ptr == badPtr) warn("bad ptr in free");
#endif

  if (ptr != NULL)		/* some non-ansi compilers/os's cant handle freeing null */
    {				/* if we knew the size of this block of memory, we could clear it - oh well */
#ifdef THINK_C
      DisposPtr(ptr);
#else
      free(ptr);
#endif
    }
}

/*----------------------------------------------------------------------*/

char*
s_strdup(s)
char* s;

/* return a copy of s.  This is identical to the standard library routine
   strdup(), except that it is safe.  If s == NULL or malloc fails, 
   appropriate action is taken.
 */
{
  unsigned long len;
  char* copy = NULL;
  
  if (s == NULL)		/* saftey check to postpone stupid errors */
    return(NULL);
    
  len = strlen(s);		/* length of string - terminator */
  copy = (char*)s_malloc((size_t)(sizeof(char)*(len + 1)));
  strncpy(copy,s,len + 1);
  return(copy);
}

/*----------------------------------------------------------------------*/

char*
fs_strncat(dst,src,maxToAdd,maxTotal)
char* dst;
   char* src;
   size_t maxToAdd;
   size_t maxTotal;

/* like strncat, except the fourth argument limits the maximum total 
   length of the resulting string
 */
{
  size_t dstSize = strlen(dst);
  size_t srcSize = strlen(src);
  
  if (dstSize + srcSize < maxTotal) /* use regular old strncat */
    return(strncat(dst,src,maxToAdd));
  else
    { size_t truncateTo = maxTotal - dstSize - 1;
      char*  save;
      char*  result = NULL;

      save = strdup(src);
      save[truncateTo] = '\0';
      result = strncat(dst,save,maxToAdd);
      free(save);
      return(result);
    }
}

/*----------------------------------------------------------------------*/

char*
fs_strncpy(s1, s2, n)
     char* s1;
     char* s2;
     long n;
/* like strncpy except that it guarentees that the destination string ends with a NULL at position n.
 */
{
  s1[n-1] = '\0';
  return(strncpy(s1, s2, n - 1));
}

/*----------------------------------------------------------------------*/

boolean 
wordbreak_notalnum(ch)		/* dgg */
long   ch;
{   
    return( !isalnum(ch));
}

boolean 
wordbreak_notgraph(ch)		/* dgg */
long   ch;
{   
    return( !isgraph(ch));
}

boolean wordbreak_user(c)
long c;
{
  if ( (gDelimiters[0] != '\0') && (strchr(gDelimiters, (char) c) != NULL) )
    return(IS_DELIMITER);  
  else if (isgraph(c))
    return(NOT_DELIMITER);
  else
    return(IS_DELIMITER);
}

typedef boolean (boolfunc) _AP((long c));

char*
strtokf(s1,isDelimiter)
char* s1;
boolfunc *isDelimiter; /* really *isDelimiter() */

/* This function is exactly like strtok, except that instead of passing a 
   delimiter string, you pass a function that decides if a character is 
   a delimiter or not, returning IS_DELIMITER or NOT_DELIMITER respecively.
   Note that passing a NULL delimiter function will cause the last delimiter
   to be used.
 */
{
  static char* searchStr = NULL;
  static boolfunc *delimiterFunc;
  long i;
  char* startTok = NULL;

  if (s1 != NULL)		/* passing s1 = NULL says use the last pos */
    searchStr = s1;
    
  if (isDelimiter != NULL)
    delimiterFunc = isDelimiter;
   
  if (searchStr == NULL || searchStr[0] == '\0')
    return(NULL);		/* nothing left to search */
    
  if (delimiterFunc == NULL)
    return(NULL);		/* no delimiter to search with */
    
  /* find the start of the next token */
  for (i = 0; searchStr[i] != '\0'; i++)
    { if ((*delimiterFunc)((long)searchStr[i]) == NOT_DELIMITER)
	break;
    }
  
  if (searchStr[i] == '\0') 
    return(NULL);		/* read to end of search string */
  else
    startTok = searchStr + i;	/* remember the starting point for this token*/
    
  /* find the end of the next token */
  for (; searchStr[i] != '\0'; i++)
    { if ((*delimiterFunc)((long)searchStr[i]) == IS_DELIMITER)
	break;
    }
   
  /* if the end is a delimiter (and not just the end of the search string)
     replace it with '\0', and put searchStr just beyond it, otherwise
     put searchStr at the terminator. */
  if (searchStr[i] != '\0')
    { searchStr[i] = '\0';
      searchStr = searchStr + i + 1;
    }
  else
    searchStr = searchStr + i;
   
  return(startTok);
}

/*----------------------------------------------------------------------*/

char*
strtokf_isalnum(s1)
char* s1;

/* 
  This is a partially evaluated version of strtok_f
 */
{
  static char* searchStr = NULL;
  long i;
  char* startTok = NULL;

  if (s1 != NULL)		/* passing s1 = NULL says use the last pos */
    searchStr = s1;
    
   
  if (searchStr == NULL || searchStr[0] == '\0')
    return(NULL);		/* nothing left to search */
    
    
  /* find the start of the next token */
  for (i = 0; searchStr[i] != '\0'; i++)
    { if (isalnum(searchStr[i]))
	break;
    }
  
  if (searchStr[i] == '\0') 
    return(NULL);		/* read to end of search string */
  else
    startTok = searchStr + i;	/* remember the starting point for this token*/
    
  /* find the end of the next token */
  for (; searchStr[i] != '\0'; i++)
    { if (!isalnum(searchStr[i]))
	break;
    }
   
  /* if the end is a delimiter (and not just the end of the search string)
     replace it with '\0', and put searchStr just beyond it, otherwise
     put searchStr at the terminator. */
  if (searchStr[i] != '\0')
    { searchStr[i] = '\0';
      searchStr = searchStr + i + 1;
    }
  else
    searchStr = searchStr + i;
   
  return(startTok);
}

/*----------------------------------------------------------------------*/

#ifdef ANSI_LIKE /* use ansi varargs */

long
cprintf(boolean print, char* format, ...)
/* just like printf, but only prints if the first argument = 1 */
{
  va_list ap;			/* the variable arguments */
  if (print == 1)
    { long res;
      va_start(ap,format);	/* init ap */
      res = vprintf(format,ap);	/* print the contents */
      va_end(ap);		/* free ap */
      return(res);
    }
  else
    return(0);
}

#else /* use k&r varargs */

long
cprintf(va_alist)
va_dcl
/* just like printf, but only prints if the first argument = 1 */
{
  va_list ap;			/* the variable arguments */
  boolean print;
  long res;

  va_start(ap);			/* init ap */

  print = va_arg(ap,boolean);	/* get the condition */

  if (print == 1)
    { char* format = va_arg(ap,char*); /* get the format */
      res = vprintf(format,ap);	/* print the contents */
    }
  else
    res = 0;

  va_end(ap);			/* free ap */

  return(res);
}

  
#endif

/* 
   Define them here! Goes to libwais.a. So will be included in all
   programs using wais.
*/

char* wais_log_file_name = NULL;
FILE* waislogfile = NULL;

/* waislog - a new and improved logging facility.
   first two arguments are important, the rest is text.

   arg1: priority.  If priority > log_level (some global), output this
   message

   arg2: message code.  This is the kind of message (search, retrieval, etc). */

char *syslog_name = 0;
  
void
#ifdef ANSI_LIKE
waislog(long priority, long message, char *format, ...)
#else
waislog(va_alist)
va_dcl
#endif
  {
  va_list ap;
#ifndef ANSI_LIKE
  int priority, message;
  char *format;
  
  va_start(ap);
  priority = va_arg(ap, int);
  message = va_arg(ap, int);
  format = va_arg(ap, char *);
  vwaislog(priority, message, format, ap);
  va_end(ap);
#else
  va_start(ap, format);
  vwaislog(priority, message, format, ap);
  va_end(ap);
#endif
  }
  
  void
#ifdef ANSI_LIKE /* use ansi varargs */
vwaislog(long priority, long message, char* format, va_list ap)
#else
vwaislog(priority, message, format, ap)
long priority, message;
char *format;	
va_list ap;
#endif /* ANSI_LIKE */
  /* just like printf, but prints to the waislogfile, with PID and time. */
  {
  static boolean in_vwaislog; /* to avoid reentrancy problems if */
			      /* s_malloc fails and calls panic() */
  
#ifdef USE_SYSLOG
  static boolean opened = 0;
#endif
  
  in_vwaislog++;
  
#ifdef USE_SYSLOG
  if (syslog_name && !opened) {
    openlog(syslog_name, LOG_PID, LOG_WAIS);
    opened = 1;
  }
#endif
  
  if(priority <= wais_log_level) {
  
#ifdef USE_SYSLOG
    if (! opened) {
#endif
      if(waislogfile == NULL && wais_log_file_name != NULL)
	waislogfile = fopen(wais_log_file_name, "a");
  
      if(waislogfile) {
  
	fprintf(waislogfile, "%d: %d: %s: %d: ", wais_pid, log_line++,
		printable_time(), message);
  
	vfprintf(waislogfile, format,ap); /* print the contents */
	fprintf(waislogfile, "\n");
	fflush(waislogfile);
      }

      if(waislogfile != NULL && waislogfile != stderr) {
	fclose(waislogfile);
	waislogfile = NULL;
      }
#ifdef USE_SYSLOG
    }
    else {
      int log_priority;
      char log_buf[512];
      static int buffer_size;
      static char *buffer;
      char *use_buffer;
      int format_size = strlen(format);

      /*
       * All of this grungy code is to allow us to use s_malloc or
       * s_realloc to allocate enough space for the message buffers,
       * while at the same time not getting into an infinite loop if
       * s_malloc or s_realloc fails and calls panic.
       */
  
      if ((in_vwaislog > 1) || (format_size <= sizeof(log_buf) - 20)) {
	use_buffer = log_buf;
      }
      else {
	if (format_size + 20 > buffer_size) {
	  buffer_size = format_size + 20;
	  if (! buffer) {
	    buffer = s_malloc(buffer_size);
	  }
	  else {
	    buffer = s_realloc(buffer, buffer_size);
	  }
	}
	use_buffer = buffer;
      }

      if (message < 0) {
	   /* Error message or warning */
	   if (priority >= 9) {
		log_priority = LOG_NOTICE;
	   }
	   else if (priority >= 5) {
		log_priority = LOG_WARNING;
	   }
	   else {
		priority = LOG_ERR;
	   }
      }
      else {
	   switch (priority) {
	   case 1:
	   case 2:
	   case 3:
		log_priority = LOG_NOTICE;
		break;
	   case 4:
	   case 5:
	   case 6:
		log_priority = LOG_INFO;
		break;
	   default:
		log_priority = LOG_DEBUG;
		break;
	   }
      }
      
      sprintf(use_buffer, "%d: %s", message, format);
      vsyslog(log_priority, use_buffer, ap);
      }
#endif /* USE_SYSLOG */
    }
  
  in_vwaislog--;
}
    
  /*----------------------------------------------------------------------*/

void
warn(message)
char* message;

{
#ifdef THINK_C
  Debugger();
#else
  printf("%s\n<press return to continue>\n",message);
  getchar();
#endif
}

/*----------------------------------------------------------------------*/
boolean substrcmp(string1,string2)
unsigned char *string1, *string2;
{
  /* compares the strings up until one of then ends.
   * returns true if they are the same, false if not.
   */
  register unsigned char *a, *b;
  
  a = string1;
  b = string2;
  
  while (*a && *b) 
    if(*a++ != *b++) 
      return false;
  return true;
}

/*----------------------------------------------------------------------*/

char *printable_time()
{ 
  static char *string;
  time_t tptr;
  time(&tptr);
  string = ctime(&tptr);
  if(string){
    if(string[strlen(string)-1] == '\n')
      string[strlen(string)-1] = '\0';   
    return(string+4);
  }
  else
    return("Time Unknown");
}

/*----------------------------------------------------------------------*/

char char_downcase(long_ch)
unsigned long long_ch;
{
  unsigned char ch = long_ch & 0xFF; /* just want one byte */
  /* when ansi is the way of the world, this can be tolower */
#ifdef MYCTYPE
  return(tolower(ch));
#else
  return (((ch >= 'A') && (ch <= 'Z')) ? (ch + 'a' -'A') : ch);
#endif
}

char *string_downcase(word)
char *word;
{
  long i = 0;

  if (word == NULL)
    return(NULL);

  while(word[i] != '\0')
   { word[i] = char_downcase((unsigned long)word[i]);
     i++;
   }

  return(word);
}

/*----------------------------------------------------------------------*/


/* parsing arguments functions */

char *next_arg(argc,argv)
int *argc;
char ***argv;

     /* Returns NULL when it is out of arguments,
        This side effects both argc and argv.  argc always contains the number
	of arguments left.
	The first returned is the command name. 
	*/
{
  if((*argc)-- > 0)
    return(*((*argv)++));
  else
    return(NULL);
}

#ifdef FIELDS /* tung, 5/94 */
char *field_next_arg(argc, number_of_files, argv)
int *argc;
int *number_of_files;
char ***argv;

     /* Returns NULL when it is out of arguments,
        This side effects both argc and argv.  argc always contains the number
	of arguments left.
	The first returned is the command name. 
	*/
{
  if((*argc)-- > 0) {
    ++(*number_of_files);
    return(*((*argv) + (*number_of_files - 1)));
  }
  else
    return(NULL);
}

#endif
/*----------------------------------------------------------------------*/

char *peek_arg(argc,argv)
int *argc;
    char ***argv;

     /* Returns the next argument without popping it.
       Returns NULL when it is out of arguments.
       */
{
  if((*argc) > 0)
    return(**argv);
  else
    return(NULL);
}

/*----------------------------------------------------------------------*/


#ifdef THINK_C
#include <EventMgr.h>
#undef MAX_FILE_NAME_LEN 
#ifdef WAIStation
#include "CRetrievalApp.h"
#endif /* def WAIStation */
#endif /* def THINK_C */

void
beFriendly()
/* this routine is called during time intensive operations
   on single processing machines (macs and DOS).  It gives
   time to other processes
 */
{ 
#ifdef never
#ifdef THINK_C
   EventRecord  	macEvent;	/* an event */

   static RgnHandle	mouseRgn = NULL; /* region for mouse moved events */
   long		        sleepTime; 	/* max time between events */
   
#ifdef WAIStation
   gApplication->FrobWaitCursor();
#endif /* def WAIStation */
   
   if (mouseRgn == NULL)
     mouseRgn = NewRgn(); /* do we need to set its value? */
     
   sleepTime = 5; /* arbitrary - a tech note recommends < 50 */
   
   WaitNextEvent(everyEvent,&macEvent,sleepTime,mouseRgn); 
#endif /* def THINK_C */
#endif
}


#ifndef HAS_VPRINTF
/* Portable vsprintf  by Robert A. Larson <blarson@skat.usc.edu> */

/* Copyright 1989 Robert A. Larson.
 * Distribution in any form is allowed as long as the author
 * retains credit, changes are noted by their author and the
 * copyright message remains intact.  This program comes as-is
 * with no warentee of fitness for any purpouse.
 *
 * Thanks to Doug Gwen, Chris Torek, and others who helped clarify
 * the ansi printf specs.
 *
 * Please send any bug fixes and improvments to blarson@skat.usc.edu .
 * The use of goto is NOT a bug.
 */

/* Feb	7, 1989		blarson		First usenet release */

/* This code implements the vsprintf function, without relying on
 * the existance of _doprint or other system specific code.
 *
 * Define NOVOID if void * is not a supported type.
 *
 * Two compile options are available for efficency:
 *	INTSPRINTF	should be defined if sprintf is int and returns
 *			the number of chacters formated.
 *	LONGINT		should be defined if sizeof(long) == sizeof(int)
 *
 *	They only make the code smaller and faster, they need not be
 *	defined.
 *
 * UNSIGNEDSPECIAL should be defined if unsigned is treated differently
 * than int in argument passing.  If this is definded, and LONGINT is not,
 * the compiler must support the type unsingned long.
 *
 * Most quirks and bugs of the available sprintf fuction are duplicated,
 * however * in the width and precision fields will work correctly
 * even if sprintf does not support this, as will the n format.
 *
 * Bad format strings, or those with very long width and precision
 * fields (including expanded * fields) will cause undesired results.
 */

#ifdef OSK		/* os9/68k can take advantage of both */
#define LONGINT
#define INTSPRINTF
#endif

/* This must be a typedef not a #define! */
#ifdef NOVOID
typedef char *pointer;
#else
typedef void *pointer;
#endif

#ifdef	INTSPRINTF
#define Sprintf(string,format,arg)	(sprintf((string),(format),(arg)))
#else
#define Sprintf(string,format,arg)	(\
	sprintf((string),(format),(arg)),\
	strlen(string)\
)
#endif

typedef int *intp;

int vsprintf(dest, format, args)
char *dest;
register char *format;
va_list args;
{
    register char *dp = dest;
    register char c;
    register char *tp;
    char tempfmt[64];
#ifndef LONGINT
    int longflag;
#endif

    tempfmt[0] = '%';
    while( (c = *format++) != 0) {
	if(c=='%') {
	    tp = &tempfmt[1];
#ifndef LONGINT
	    longflag = 0;
#endif
continue_format:
	    switch(c = *format++) {
		case 's':
		    *tp++ = c;
		    *tp = '\0';
		    dp += Sprintf(dp, tempfmt, va_arg(args, char *));
		    break;
		case 'u':
		case 'x':
		case 'o':
		case 'X':
#ifdef UNSIGNEDSPECIAL
		    *tp++ = c;
		    *tp = '\0';
#ifndef LONGINT
		    if(longflag)
			dp += Sprintf(dp, tempfmt, va_arg(args, unsigned long));
		    else
#endif
			dp += Sprintf(dp, tempfmt, va_arg(args, unsigned));
		    break;
#endif
		case 'd':
		case 'c':
		case 'i':
		    *tp++ = c;
		    *tp = '\0';
#ifndef LONGINT
		    if(longflag)
			dp += Sprintf(dp, tempfmt, va_arg(args, long));
		    else
#endif
			dp += Sprintf(dp, tempfmt, va_arg(args, int));
		    break;
		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		    *tp++ = c;
		    *tp = '\0';
		    dp += Sprintf(dp, tempfmt, va_arg(args, double));
		    break;
		case 'p':
		    *tp++ = c;
		    *tp = '\0';
		    dp += Sprintf(dp, tempfmt, va_arg(args, pointer));
		    break;
		case '-':
		case '+':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case ' ':
		case '#':
		case 'h':
		    *tp++ = c;
		    goto continue_format;
		case 'l':
#ifndef LONGINT
		    longflag = 1;
		    *tp++ = c;
#endif
		    goto continue_format;
		case '*':
		    tp += Sprintf(tp, "%d", va_arg(args, int));
		    goto continue_format;
		case 'n':
		    *va_arg(args, intp) = dp - dest;
		    break;
		case '%':
		default:
		    *dp++ = c;
		    break;
	    }
	} else *dp++ = c;
    }
    *dp = '\0';
    return dp - dest;
}


int vfprintf(dest, format, args)
FILE *dest;
register char *format;
va_list args;
{
    register char c;
    register char *tp;
    register int count = 0;
    char tempfmt[64];
#ifndef LONGINT
    int longflag;
#endif

    tempfmt[0] = '%';
    while(c = *format++) {
	if(c=='%') {
	    tp = &tempfmt[1];
#ifndef LONGINT
	    longflag = 0;
#endif
continue_format:
	    switch(c = *format++) {
		case 's':
		    *tp++ = c;
		    *tp = '\0';
		    count += fprintf(dest, tempfmt, va_arg(args, char *));
		    break;
		case 'u':
		case 'x':
		case 'o':
		case 'X':
#ifdef UNSIGNEDSPECIAL
		    *tp++ = c;
		    *tp = '\0';
#ifndef LONGINT
		    if(longflag)
			count += fprintf(dest, tempfmt, va_arg(args, unsigned long));
		    else
#endif
			count += fprintf(dest, tempfmt, va_arg(args, unsigned));
		    break;
#endif
		case 'd':
		case 'c':
		case 'i':
		    *tp++ = c;
		    *tp = '\0';
#ifndef LONGINT
		    if(longflag)
			count += fprintf(dest, tempfmt, va_arg(args, long));
		    else
#endif
			count += fprintf(dest, tempfmt, va_arg(args, int));
		    break;
		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		    *tp++ = c;
		    *tp = '\0';
		    count += fprintf(dest, tempfmt, va_arg(args, double));
		    break;
		case 'p':
		    *tp++ = c;
		    *tp = '\0';
		    count += fprintf(dest, tempfmt, va_arg(args, pointer));
		    break;
		case '-':
		case '+':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case ' ':
		case '#':
		case 'h':
		    *tp++ = c;
		    goto continue_format;
		case 'l':
#ifndef LONGINT
		    longflag = 1;
		    *tp++ = c;
#endif
		    goto continue_format;
		case '*':
		    tp += Sprintf(tp, "%d", va_arg(args, int));
		    goto continue_format;
		case 'n':
		    *va_arg(args, intp) = count;
		    break;
		case '%':
		default:
		    putc(c, dest);
		    count++;
		    break;
	    }
	} else {
	    putc(c, dest);
	    count++;
	}
    }
    return count;
}

vprintf(format, args)
char *format;
va_list args;
{
    return vfprintf(stdout, format, args);
}

#endif

#ifndef HAS_STRDUP
char* strdup(s)
const char* s;

/* return a copy of s.  This is identical to the standard library routine
   strdup(), except that it is safe.  If s == NULL or malloc fails, 
   appropriate action is taken.
 */
{
  unsigned long len;
  char* copy = NULL;
  
  if (s == NULL)		/* saftey check to postpone stupid errors */
    return(NULL);
    
  len = strlen(s);		/* length of string - terminator */
  copy = (char*)s_malloc((size_t)(sizeof(char)*(len + 1)));
  strncpy(copy,s,len + 1);
  return(copy);
}
#endif
/*----------------------------------------------------------------------*/

#ifdef NEED_MYBCOPY
void
mybcopy(b1, b2, length)
     char *b1, *b2;
     int length;
{
  int i;
  if (b1 > b2) {
    for (i=0;i<length;i++) {
      b2[i] = b1[i];
    }
  } else {
    for (i=length-1;i>=0;i--) {
      b2[i] = b1[i];
    }
  }
}
#endif /* NEED_MYBCOPY */

