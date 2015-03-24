
/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* this is a simple ui for building other ui's on top.
 * -brewster
 */

/* to do:
 *   generate multiple queries for long documents.
 *     this will crash if the file being retrieved is larger than 100k.
 *   
 */

#ifndef UI_H
#define UI_H

#include "cdialect.h"
#include "zprot.h"
#include "zutil.h"
#include "wprot.h"
#include "transprt.h"
#include "wmessage.h"
#include "panic.h"
#include "server.h"

#define SERVICE_NAME "z39_50"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */


char *
generate_search_apdu _AP((char* buff,  /* buffer to hold the apdu */
			  long *buff_len, /* number of bytes written to the buffer */
			  char *seed_words, /* string of the seed words */
			  char *database_name,
			  DocObj** docobjs,
			  long maxDocsRetrieved
			  ));

char *
generate_retrieval_apdu _AP((char *buff, 
			     long *buff_len, 
			     any *docID,
			     long chunk_type,
			     long start_line, long end_line,
			     char *type,
			     char *database_name));


/* moved into individual ui codes -brewster and bonnie
void
dsply_text_record_completely _AP((WAISDocumentText *record));

void
display_search_response _AP((SearchResponseAPDU *response));
*/

/* to initialize a conection.  Returns max buffer size. */

long
init_connection _AP((char* inBuffer,
		     char* outBuffer,
		     long bufferSize,
		     FILE *conection, 
		     char *userInfo));

long
interpret_message _AP((char *request_message,
		       long request_length,
		       char *response_message,
		       long response_buffer_length, /* length of the buffer (modified)*/
		       FILE *connection,
		       boolean verbose));

long close_connection _AP((FILE *connection));

void
display_text_record_completely _AP((WAISDocumentText *record, 
			       boolean quote_string_quotes));

char *trim_junk _AP((char *headline));

char *delete_seeker_codes _AP((char* string,long* length));

void read_environment_variables _AP((char* host, char* port));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */



#endif /* ndef UI_H */
