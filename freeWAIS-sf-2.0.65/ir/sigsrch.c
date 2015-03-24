/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.    
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/sigsrch.c,v 2.0.1.3 1995/11/14 18:21:42 pfeifer Exp $";
#endif

/* Change log:
 * $Log: sigsrch.c,v $
 * Revision 2.0.1.3  1995/11/14 18:21:42  pfeifer
 * patch51:
 *
 * Revision 2.0.1.2  1995/11/14  16:15:35  pfeifer
 * patch50:
 *
 * Revision 2.0.1.1  1995/09/15  09:47:36  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:01:24  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.3  1994/05/20  12:57:18  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:49  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.28  92/03/05  07:08:41  shen
 * update init_search_engine to take two more parameters: grow percent
 * and text size. Add these two parameters to call to sig_open_database
 * 
 * Revision 1.27  92/02/12  13:47:27  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
*/

/* implements the search part of irext.h 
   (search_word and finished_search_word)
   Stub for tracy shen.
 */

#include "cdialect.h"
#include "irfiles.h"
#include "irsearch.h"
#include "irext.h"
/* #include <string.h> */
#include "sigsrch.h"


/* ==================================
 * ===  Initialization Functions  ===
 * ==================================*/

/* set by init_search_engine when cm_mem_percent is non-zero */
long cm_mem_percent_saved = 0; 
long text_size_saved = 0; 
long grow_percent_saved = 0; 

long init_search_engine(file, initialize, for_search, cm_mem_percent,
text_size, grow_percent)
  char* file;
  boolean initialize;
  boolean for_search;
  long cm_mem_percent;
  long text_size;
  long grow_percent;
{
  long for_update;
  char full_filename[MAX_FILE_NAME_LEN+1];
  char *getenv(), *temps;

  waislog(WLOG_MEDIUM, WLOG_INFO, "Starting Seeker-RAM Engine.");

  if(cm_mem_percent != 0)
    cm_mem_percent_saved = cm_mem_percent;
  if(grow_percent != 0)
    grow_percent_saved = grow_percent;
  if(text_size != 0)
    text_size_saved = text_size;

  temps = getenv("SEEKER_TEXT_SIZE");
  if ( temps )
     text_size_saved = atol( temps);

  if( for_search )
    for_update = 0;
  else
    for_update = 1;

  temps = getenv("SEEKER_NOSECURE_SERVER");
  if ( temps )
     for_update = 1;
  strncpy(full_filename, file, MAX_FILE_NAME_LEN);
  strncat(full_filename, "/", MAX_FILE_NAME_LEN);
  strncat(full_filename, database_file(file), MAX_FILE_NAME_LEN);

  return( sig_init( full_filename, (long)initialize, (long)for_search, 
                    for_update, cm_mem_percent_saved) );
}

long finished_search_engine()
{
  waislog(WLOG_MEDIUM, WLOG_INFO, "Shutting down Seeker-RAM Engine.");
  return( sig_exit());
}


/*
 *  ext_open_database: see irext.h
 */

long ext_open_database (db, initialize, for_search)
     database *db;
     boolean initialize;
     boolean for_search;
{
  long for_update;

  if( for_search )
    for_update = 0;
  else
    for_update = 1;
  return( sig_open_database( db, (long)initialize, (long)for_search, 
                             for_update, cm_mem_percent_saved, 
                             text_size_saved, grow_percent_saved));
}
  


/*
 *  ext_close_database: see irext.h
 */

long ext_close_database (db)
     database *db;
{
  return( sig_close_database(db));
}


char *database_file(database_name)
     char *database_name;
{
  return("INDEX");
}

/*===========================*
 *===  Setting Paramters  ===*
 *===========================*/

long max_hit_retrieved = 0;
char **srcs = NULL;

long set_query_parameter (mask, parameters)
     long mask;
     query_parameter_type * parameters;
{
  switch (mask)
    {
    case SET_MAX_RETRIEVED_MASK:
      max_hit_retrieved = parameters->max_hit_retrieved;
      return(0);
      break;
    case SET_SELECT_SOURCE:
      if(NULL != srcs){
	if(NULL != srcs[0])
	  s_free(srcs[0]);
	s_free(srcs);
      }
      srcs = parameters->srcs;
      {long count = 0;
       if(NULL == srcs)
	 waislog(WLOG_LOW, WLOG_INFO, "No sources specified in set_query_parameter");
       else {
	 for(count = 0; srcs[count] != NULL; count++){
	   waislog(WLOG_LOW, WLOG_INFO,
		   "Setting source to '%s'", srcs[count]);
           }

         sig_set_query_parameter( mask, parameters);
	 }
     }
      break;
    default:
      return(-1);
      break;
    }	
  return(0);
}

/*=============================*	
 *===  Searching for words  ===*
 *=============================*/

/* see irext.h for doc */
long init_search_word (db)
     database* db;
{
  return(sig_init_search_word(db));
}

long search_word(word, char_pos, line_pos, weight, doc_id, 
		 word_pair, db)
     char *word; /* the word to be searched for */
     long char_pos;		/* the position of the start of the word */
     long line_pos;		/* is this needed? not for signature system */
     long weight;		/* how important the word looks syntactically,
				   such as is it bold */
     long doc_id;		/* current document, seed words is 0,
				   then it increments into the relevant 
				   document */
     long word_pair;
     database *db;
{
  long number_of_occurances;
  long word_id = 
    look_up_word_in_dictionary(word, &number_of_occurances, db);

  return( sig_search_word(word, char_pos, line_pos,
			  weight, doc_id, number_of_occurances, 
                          ABSOLUTE(word_id), word_pair));
}

/*===================*
 *===  Best Hits  ===*
 *===================*/


long finished_search_word(db)
     database *db;
{ 
  return(sig_finished_search_word(db));
}

/* see irext.h for doc */
long init_best_hit (db)
     database *db;
{
  return(0);
}

/* returns the next best hit */
long best_hit(db, doc_id, best_character, best_line, score)
     database *db;
     long *doc_id;	
     long *best_character;
     long *best_line;
     long *score;
{
  return(sig_best_hit(doc_id, best_character, best_line, score));
}

long finished_best_hit(db)
     database *db;
{ 
  return(sig_finished_best_hit());
}

