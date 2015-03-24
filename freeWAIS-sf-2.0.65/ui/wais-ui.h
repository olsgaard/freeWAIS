/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* $Log: wais-ui.h,v $
 * Revision 2.0.1.1  1995/09/15 09:55:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:37  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:19:48  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:41  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.4  92/03/17  14:33:37  jonathan
 * Cleaned up, prototyped.
 * 
 */

#ifndef _H_WAIS_UI
#define _H_WAIS_UI

#include "ui.h"

#ifndef MAX_MESSAGE_LEN
#define MAX_MESSAGE_LEN BUFSZ
#endif

#define QUESTION_FILE_VERSION 1

#define CHARS_PER_PAGE 2000

/* function definitions */

void showDiags _AP((diagnosticRecord** d));
void printDiags _AP((diagnosticRecord** d));
void write_text_record_completely _AP((FILE* fp, 
				       WAISDocumentText *record,
				       Boolean quote_string_quotes));
DocList build_response_list _AP((SearchResponseAPDU* response,
				 SourceID source, char** seed_words_used));
void replacecontrolM _AP((char* buffer,long* length));
void SearchWais _AP((Question q));
char* GetWaisDocument _AP((Question q, DocumentID doc,
			   char* type, FILE* fp, long* written));
void RetrieveWaisDocument _AP((void (*start_output)(), void (*end_output)(),
			       Question q, DocumentID doc));

void dumptext _AP((FILE* fp, char* buffer, long size));
void test_connection _AP((Question q));
void close_source _AP((Source source));
DocumentID getNextorPrevDoc _AP((Question q, Source source,
				 DocumentID doc, Boolean nextp));

#endif
