/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
  
   3.26.90	Harry Morris, morris@think.com
   4.11.90  HWM - generalized conditional includes (see c-dialect.h)
 *
 * $Log: cutil.h,v $
 * Revision 2.0.1.3  1995/09/15 09:39:48  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.2  1995/09/11  12:44:30  pfeifer
 * patch4: Added conditional inclusion of sys/param.h.
 *
 * Revision 2.0.1.1  1995/09/08  19:10:36  pfeifer
 * patch1: Fixed prototype for substrcmp
 *
 * Revision 2.0  1995/09/08  07:58:27  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.10  1994/08/08  07:32:30  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.9  1994/07/15  17:11:03  pfeifer
 * negative numerics
 *
 * Revision 1.8  1994/07/13  07:53:26  pfeifer
 * beta 02
 *
 * Revision 1.7  1994/06/10  08:14:42  pfeifer
 * added definition for strdup, which is missing with solaris 2.3 ?
 *
 * Revision 1.6  1994/05/20  12:55:46  pfeifer
 * beta
 *
 * Revision 1.5  1994/03/08  20:57:54  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.19  92/03/07  19:44:24  jonathan
 * Added some IBM defines. mycroft@hal.gnu.ai.mit.edu.
 * 
 * Revision 1.18  92/02/21  11:01:07  jonathan
 * Added wais_log_level
 * 
 * Revision 1.17  92/02/16  21:24:25  jonathan
 * Removed macro for waislog under BSD, since vprintf is now part of cutil.c
 * 
 *
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef _H_C_util_
#define _H_C_util_

#include "cdialect.h"
#include "../defaults.h"

#include <stdio.h>   /* this used to be wrapped in an ifndef NULL, 
			but messed up on some gcc's */
 
#ifdef I_STDARG
#include <stdarg.h>
#endif
#ifdef I_VARARGS
#include <varargs.h>
#endif

#ifdef I_SYS_PARAM
#include <sys/param.h>
#endif

#define MAX_FILENAME_LEN 255
#define MAX_DELIMITERS 256

/*----------------------------------------------------------------------*/
/* types and constants */

#ifndef boolean
#define boolean	unsigned long
#endif /* ndef boolean */ 

#ifndef THINK_C
#ifndef Boolean
#define Boolean	boolean
#endif /* ndef Boolean */ 
#endif /* ndef THINK_C */

#ifndef true
#define true 	(boolean)1L
#endif /* ndef true */

#ifndef false
#define false 	(boolean)0L   /* used to be (!true), but broke 
				 some compilers */
#endif /* ndef false */

#ifndef TRUE
#define TRUE	true
#endif /* ndef TRUE */

#ifndef FALSE
#define FALSE	false
#endif /* ndef FALSE */

#ifndef NULL
#define NULL	0L
#endif /* ndef NULL */

/*----------------------------------------------------------------------*/
/* Fast string macros - warning don't pass NULL to these! */

#define STREQ(s1,s2) ((*(s1)==*(s2)) && !strcmp(s1,s2))
#define STRNCMP(s1,s2,n) \
    ((*(s1)==*(s2)) ? strncmp(s1,s2,n) : (*(s1) - *(s2)))

/*----------------------------------------------------------------------*/
/* convenience */

#define NL() printf("\n")
#define isnumchar(A) (isdigit(A)||(A)=='-'||(A)=='.')
/*----------------------------------------------------------------------*/
/* functions */

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

/* enhanced memory handling functions - don't call them directly, use the
   macros below */
void	fs_checkPtr _AP((void* ptr));
void*	fs_malloc _AP((size_t size));
void*	fs_realloc _AP((void* ptr,size_t size));
void	fs_free _AP((void* ptr));
char* 	fs_strncat _AP((char* dst,char* src,size_t maxToAdd,size_t maxTotal));
char* 	fs_strncpy _AP((char* s1,char* s2, long n));

/* macros for memory functions.  call these in your program.  */
#define s_checkPtr(ptr) 	fs_checkPtr(ptr)
#define s_malloc(size)	      	fs_malloc(size)
#define s_realloc(ptr,size)	fs_realloc((ptr),(size))
#define s_free(ptr)		{ fs_free((char*)ptr); ptr = NULL; }
#define s_strncat(dst,src,maxToAdd,maxTotal)	fs_strncat((dst),(src),(maxToAdd),(maxTotal))
#define s_strncpy(s1,s2,n) fs_strncpy((s1), (s2), (n))

char* 	s_strdup _AP((char* s));

boolean wordbreak_notalnum _AP(( long ch));  /* dgg */
boolean wordbreak_notgraph _AP(( long ch));	 /* dgg */
boolean wordbreak_user _AP(( long ch));	 /* dgg, uses gDelimiters */

char*	strtokf _AP((char* s1,boolean (*isDelimiter)(long c))); 
char* strtokf_isalnum _AP((char* s1));

#define IS_DELIMITER	true
#define	NOT_DELIMITER	false

#ifdef ANSI_LIKE	/* use ansi */
long		cprintf _AP((boolean print,char* format,...));
#else /* use K & R */
long		cprintf _AP(());
#endif

/* 
   will go to libwais.a and used by all *wais* programs
*/
extern char *wais_log_file_name;
extern FILE *waislogfile;

#ifdef ANSI_LIKE	/* use ansi */
void		waislog _AP((long priority, long message, char* format,...));
void		vwaislog _AP((long priority, long message, char *format, va_list));
#else /* use K & R */
void		waislog _AP(());
void		vwaislog _AP(());
#endif /* ANSI_LIKE */

/* waislog priorities and messages */
/* this is backwards because of how wais_log_level works. */
#define WLOG_HIGH	1
#define WLOG_MEDIUM 	5
#define WLOG_LOW	9

#define WLOG_CONNECT	1
#define WLOG_CLOSE	2
#define WLOG_SEARCH	3
#define WLOG_RESULTS	4
#define WLOG_RETRIEVE	5
#define WLOG_INDEX	6
#define WLOG_INFO	100
#define WLOG_ERROR	-1
#define WLOG_WARNING	-2

void 	warn _AP((char* message));

boolean substrcmp _AP((unsigned char *string1, unsigned char *string2));
#define MAXIMUM(x,y) (((x) > (y)) ? (x) : (y))
#define MINIMUM(x,y) (((x) < (y)) ? (x) : (y))
#define ABSOLUTE(x) (((x) < 0) ? (-(x)) : (x))

char *printable_time _AP((void));

char char_downcase _AP((unsigned long ch));
char *string_downcase _AP((char* word));


char *next_arg _AP((int *argc, char ***argv));
#ifdef FIELDS /* tung, 5/94 */
char *field_next_arg _AP((int *argc, int *number_of_files, char ***argv));
#endif
char *peek_arg _AP((int *argc, char ***argv));

void		beFriendly _AP((void));

#ifdef _C_C_util_
long wais_pid = 0;
long log_line = 0;
long wais_log_level = 10;
char gDelimiters[MAX_DELIMITERS];  /* dgg */
#else
extern long wais_pid;
extern long log_line;
extern wais_log_level;
extern char gDelimiters[];  /* dgg */
#endif /* _C_C_util_ */

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#ifdef USE_SYSLOG
#define LOG_WAIS LOG_LOCAL5
#endif

/*----------------------------------------------------------------------*/

#endif /* ndef _H_C_util_ */
