/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: question.h,v $
 * Revision 2.0.1.1  1995/09/15 09:53:49  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:17  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:30:14  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:36  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.2  92/03/12  16:24:47  jonathan
 * Renamed to question.h, as this file is really more like source.h.
 * Updated all function declarations to use ANSI prototypes, if available.
 * 
 */

#ifndef _H_QUESTION
#define _H_QUESTION

#include <cdialect.h>

#ifndef MAX_MESSAGE_LEN
#define MAX_MESSAGE_LEN BUFSZ
#endif

typedef struct question{
  char *type;
  char name[STRINGSIZE];
  char keywords[5000];
  char* keywords_used;
  short version;
  short view;
  char* sourcepath;
  int numsources;
  SourceList Sources;
  int CurrentSource;
  int numdocs;
  DocList RelevantDocuments;
  int CurrentRelDoc;
  int numresdocs;
  int maxresdocs;
  DocList ResultDocuments;
  int CurrentResDoc;
  char request_message[MAX_MESSAGE_LEN]; /* arbitrary message limit */
  char response_message[MAX_MESSAGE_LEN]; /* arbitrary message limit */
  SearchResponseAPDU  *query_response;
  SearchResponseAPDU  *retrieval_response;
  WAISSearchResponse  *query_info;
  Boolean modified;
} _Question, *Question;

short readRect _AP((Question question, FILE* file));
short ReadQuestion _AP((Question question, FILE* file));
void WriteQuestionfp _AP((FILE* fp, Question question));
void WriteQuestion _AP((char* filename, Question question));

#endif
