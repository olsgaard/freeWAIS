/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Change log:
 * $Log: panic.c,v $
 * Revision 2.0.1.1  1995/09/15 09:46:41  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:00:55  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.6  1994/08/08  07:32:47  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.4  1994/07/22  12:25:20  pfeifer
 * Renamed logfile to waislogfile and log_file_name to wais_log_file_name
 * to avoid clashes when linking with gopher and other systems
 *
 * Revision 1.3  1994/05/20  12:57:00  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:38  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.10  92/03/07  19:42:30  jonathan
 * ANSIfied arguments.
 * 
 * Revision 1.9  92/02/20  13:58:10  jonathan
 * Removed redundant include of varargs.h.  Removed fprintf for BSD, since
 * we've not got a vprintf.
 * 
 * Revision 1.8  92/02/12  13:38:05  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

/* panic is an error system interface.  On the Mac, it will pop
 * up a little window to explain the problem.
 * On a unix box, it will print out the error and call perror()
 */
 
#include "panic.h"
#include "futil.h"

#include <ctype.h> 

#ifndef EXIT_FAILURE  /* should be in stdlib */
#define EXIT_FAILURE (-1)
#endif /* ndef EXIT_FAILURE */

/*----------------------------------------------------------------------*/

static void exitAction _AP((long error));

static void
exitAction(error)
long error;
{
  long i;
#ifdef THINK_C
  Debugger();
#else
  for (i = 0; i < 100000; i++)
    ;
#endif
#if 0
    abort();
#else
  exit(-1);
#endif
}

/*----------------------------------------------------------------------*/

#define PANIC_HEADER "Fatal Error:  "
#define BELL "\007"

#ifdef THINK_C /* pop up a dialog box */
#include "CDynamicError.h"
#endif

void
#ifdef ANSI_LIKE
  panic(char* format, ...)
#else
panic(va_alist)
va_dcl
#endif
  {
    va_list ap;			/* the variable arguments */
#ifndef ANSI_LIKE
  char *format;

  va_start(ap);
  format = va_arg(ap, char *);
#else
  va_start(ap, format);
#endif

#ifdef THINK_C			/* pop up a dialog box */

  char buffer[1000];		/* hope this is enough space! */
  long i;
  strncpy(buffer,PANIC_HEADER,1000);
  SysBeep(5);
  vsprintf(buffer + strlen(PANIC_HEADER),format,ap);
  for (i = 0L; buffer[i] != '\0'; i++)
    { if (buffer[i] == '\n' || buffer[i] == '\r')
	buffer[i] = ' ';
      }
  gError->PostAlertMessage(buffer);

#else
  
  vwaislog(1, -1, format, ap);
  va_end(ap);

#endif
 
  exitAction(0);
}
  

/*----------------------------------------------------------------------*/
