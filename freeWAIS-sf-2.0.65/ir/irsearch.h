/* WIDE AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard 
   disclaimer.
   3.26.90	Brewster Kahle, brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* header file for irsearch.c */

#ifndef _H_search_
#define _H_search_

#include "cdialect.h"
#include "irfiles.h"
#include "cutil.h"
#include "futil.h"
#include "irhash.h"
#include "zprot.h"
#include "wprot.h"

/* the default database to use if none is specified */
#define	INFO_DATABASE_NAME	"INFO"
#define MAX_NORMAL_SCORE 1000


extern char *server_name;
extern long tcp_port;

typedef struct hit {
	double 	weight;
	long 	document_id;
	long	start_character;
	long 	end_character;
	char 	filename[MAX_FILE_NAME_LEN + 1];
	char 	headline[MAX_HEADLINE_LEN + 1];
	char 	type[MAX_TYPE_LEN + 1];
	char	date[ANSI_DATE_LENGTH + 1];
	long	number_of_lines;
	long 	document_length;
	long 	best_character;  /* this is the offset into the 
				    document of the character at	
				    the start of the best section */
	long 	best_line;  /* similar to best_character but for lines */
	database *db;
} hit;	

typedef struct search {
	database*		db;	
	double*			document_score_array;
#ifdef BOOLEANS
	double*		prior_score_array;	/* dgg, 12/91 GS TLG */
#endif
	unsigned long		num_best_hits;
	hit*			best_hits;
} search;

/* exported functions */
#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

boolean run_search 
  _AP((SearchAPDU* aSearch,
       WAISDocumentHeader** headers,
       diagnosticRecord*** diags,char* index_directory, 
       char** seed_words_used,
       long waisProtocolVersion,long* headerNum));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif
