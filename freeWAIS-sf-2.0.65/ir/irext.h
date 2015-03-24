/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer. */

/* Include file for the irhash.c file.
   Implements the building functions in irext.h */

#ifndef IREXT_H
#define IREXT_H


/* An interface for adding new server types into the WAIS system.
 * The idea is to use the parsing and bookkeeping operatios of the serial 
 * indexer, while allowing different invered file and signiture systems 
 * to be added as back ends.
 *
 * - Tracy Shen and Brewster 3/91
 */

/*
 * $Log: irext.h,v $
 * Revision 2.0.1.4  1995/12/06 10:57:32  pfeifer
 * patch53: Change float parameter of search_word to double.
 * patch53: (Patch from Rodney Barnett).
 *
 * Revision 2.0.1.3  1995/11/03  12:36:20  pfeifer
 * patch45: Removed prototype for search_word since it makes the AIX
 * patch45: compiler unhappy.
 *
 * Revision 2.0.1.2  1995/10/10  18:28:43  pfeifer
 * patch33: Additional parameter query_weight for search_word().
 *
 * Revision 2.0.1.1  1995/09/15  09:42:28  pfeifer
 * patch7: Fixed #ifdef in cpp macro call.
 *
 * Revision 2.0  1995/09/08  07:59:27  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.5  1994/09/05  10:19:55  pfeifer
 * removed buggy init_add_word prototype
 *
 * Revision 1.5  1994/09/05  10:19:55  pfeifer
 * removed buggy init_add_word prototype
 *
 * Revision 1.4  1994/05/20  12:57:11  huynh1
 * beta
 *
 * Revision 1.3  1994/03/23  12:58:45  huynh1
 * init_add_word modified.
 * patchlevel 07.
 *
 * Revision 1.2  1994/03/08  20:42:01  huynh1
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.24  92/03/20  11:02:16  jonathan
 * Added word_position boolean to add_word.  This is a new switch to allow an
 * indexer to ignore the word position info (if it cares), based on
 * indexer parameters.
 * 
 * Revision 1.23  92/03/05  07:06:12  shen
 * update init_search_engine prototype to add two more parameters
 * For seeker-ram, the twonew parameters are: grow percent and textsixe
 * 
 * Revision 1.22  92/02/29  20:11:57  jonathan
 * Conditionalized definition of DF_INDEPENDENT, etc.
 * 
 * Revision 1.21  92/02/13  12:24:25  jonathan
 * conditionalized inclusion of irparse.h on BOOL.
 * 
 *
 * Tracy changes:
 *  - in function "add_word", add two more parameters, source and date
 *  - add a new function "set_query_parameter"
 * proposed changes by brewster:
 *  replace date_type with time_t: accepted
 *  take out all "unsigned" type modifiers (tracey will concider this)
 *  replace short with long: accepted
 *  replace int with long (we port to 16 bit machines still): accepted
 *  added source to delete_doc_id parameters: accepted
 * Proposed changes by brewster and tracy:
 *  if routines are successful return 0, otherwise an error code: accepted
 * Proposed changes by harry:
 *  make the dictionary value be any size.  This can be done by 
 *   passing in a size arg or by passing in read and write routines.
 *  have a function that says we will not be calling best_hit anymore.
 * proposed changes by brewster:
 *  took out hash_pos from add word.
 *  change source to a database* db.
 *  added finished_best_hit, finished_search_word(db), finished_add_word
 * NOT ACCEPTED proposed by tracy (9/91)
 *  create a function init_search_word and pass an array of db's
 *  modify search_word dont pass db.
 *  modify finish_search_word dont pass db.
 *  create init_best_hit pass dbs
 *  modify best_hit to pass db (to be modified)
 *  modify a hit structure to contain a db
 *  create se_init, se_exit (tracy will send these in)
 *  create se_open_database, se_close_database (tracy will send these in)
 * ACCEPTED proposed by tracy (9/91)  
 *  create a function init_search_word and pass db
 *  add total word count to db (not set in server side)
 *  create init_best_hit 
 *  change dbs slot query_parameter_type from being a "database** dbs"
 *    to "char** srcs" (not done on server side yet)
 *  modify best_hit to have argument doc_id, best_character, score
 *  create ext_open_database, ext_close_database
 *  modify init_add_word to add char* src. (not done in server side)
 * proposed changes by brewster
 *  modify best_hit to take both best_character and best_line
 * IMPLEMENTED proposed by brewster
 *  specify that search_word take a downcased word
 *  create function char *database_file(char *database_name)
 *	that will return the name of the file that the database is stored in.
 *	on Seeker and Beta this would return "INDEX" always, 
 * 	and serial server this would returns its argument. 
 * IMPLEMENTED proposed by brewster
 *   the srcs list in set_query_parameter will be what the user
 *   passed in the database fields of the Z39.50 request.
 *   This means that if multiple src's are specified separate by comma's
 *   then it is up to the backend to parse those out.
 * IMPLEMENTED proposed by brewster
 *   the src field in init_add_word will always be NULL and
 *   the information will be passed via set_query_parameter.
 * APPROVED proposed by brewster
 *   add init_search_engine and finished_search_engine
 *     this would be called when the server process starts and exists.
 *     these functions could check to make sure everything is sane.
 *     same arguments in ext_open_database
 * proposed by brewster 
 *   change scores to doubles rather than longs.  maybe weights too.
 * IMPLEMENTED proposed by tracy
 *   pass another argument to search_word and add_word:
 *	long word_pair.
 * proposed by tracy: APPROVED
 *   take out src arg from init_add_word
 *   change arg name in search_word from doc_id to relevant_doc_number
 *   take out dictionary_value from search_word
 *   New arguments to ext_open_database:
 *     initialize (same)
 *     for_search  (if true searches can happen, otherwise can not)
 *     for_update  (if true updates can happen, otherwise can not)
 * proposed change by tracy
 *   add 2 more long arguments to init_search_engine and ext_open_database
 *     (for seeker, the first argument should be the percentage of 
 *      CM memory for signatures.  For open_database put in the total 
 *       raw text size).
 *     
 */

#include "cdialect.h"
#include "irfiles.h" /* for database */

#ifdef BOOL
#include "irparse.h" /* for boolean searches */
#endif

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

/* ============================
 * ===  Control Functions  ===
 * ============================*/

/*
 * SE_init - Search Engine initialization function
 *
 * Parameter description:
 *   if_update - if update is to be performed in this run, value
 *               be True (1L) or False(0L).
 *   if_query  - if query is to be performed in this run, value
 *               be True (1L) or False (0L).
 *
 * Functional description:
 *   This function should be the first function call FE(front end)
 *   make to the BE(back end) SE(search engine).
 *   It gives the SE a chance to initialize its global variables
 *   to best serve FE's requests efficiently.
 *   It only needs to * be called once each run. Sebsequent calls
 *   will be ignored.
 *   For a batch update ( eg, waisindex run),
 *   parameters if_update should be set to True, and if_quesry be False.
 *   For serving query (eg, a waisserver run), the if_query will be True.
 *   To allow on-line update while serving query, if_update should be
 *   set to True, otherwise be False.
 *   The waisserver has to be able to take on-line update request and
 *   update the search engine's database.
 *
 */

long SE_init _AP(( long if_update, long if_query));

/*
 * SE_exit - Search Engine exit function
 *
 * Parameter description:
 *   None
 *
 * Functional description:
 *   This function should be the last function call FE(front end)
 *   makes to SE. It gives the SE a chance to flush data kept in
 *   buffers, clean up temporary files,  and free up resourecs.
 */

long SE_exit _AP(( void));



long SE_open_database _AP (( database *db,
                       long if_initialize,
                       long if_update,
                       long if_query,
                       long *parameter1,
                       long *parameter2));
/*
 * SE_open_database - open a database
 *
 * Parameter description:
 *   db - pointer to a database structure. The structure should
 *               contain a field "SE_private_tag" of type void *.
 *               The SE will fill in this field when the
 *               database is open. This is the search enginer's pointer
 *               to its data structure of the database.
 *   if_initialize - if initialize this databse. If value is
 *               True, the database will be set empty. If
 *               one already exists, it will be purged
 *               or marked old  according to the system maintenance
 *               policy employed.
 *   if_update - if update is to be performed on this database, value
 *               be True (1L) or False(0L).
 *   parameter1 and parameter2 - these are additional info the
 *               SE needs from the FE. They are pointers. If
 *               a SE does not need extra info, FE will just pass
 *               NULL.
 *               For new seeker, parameter1 is the databases max size
 *               in percentage to the full-system-load.
 *               CM memory is a limited resource to be shared by
 *               multiple databases, and is not efficient
 *               in dynamic re-allocation.
 *               Seeker requires the FE to tell it the maximum size the
 *               database can grow to thus it can pre-allocate the
 *               right amount of processors to the databse and will
 *               wrap around when it reachs the limit to squeeze out
 *               old data.
 *               The It is specified as the
 *               percentage of the full-load CM signature pool.
 *               For example, on a 8K CM with small memory, it can
 *               holds up to 200 megabytes raw text size data.
 *               If parameter
 *		
 */

long SE_close_database _AP (( database *db));

long SE_checkpointing _AP(( database *db));




/* this is called when the server or indexer is started up 
 *   before any other operations are run.  
 *
 * If this is a server starting, then file is the directory of the index.
 * If this is an indexer starting, then file is the index file.
 * NOTE - This routine may be called more than once
 *
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 *          -1 insufficient resources
 */

long init_search_engine _AP((char* file, 
			     boolean initialize, 
			     boolean for_search,
			     long cm_mem_percent,
			     long rawtext_size,
			     long grow_percent));


/* this is called when the server is shut down.
 *
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 */

long finished_search_engine _AP((void));


/*
 *  ext_open_database: This function will be called on a database before
 *  	any operations are done on it.  It might be called multiple times 
 *	with the same database before a close is done.
 *	initialize: means that the database should be cleared of all state 
 *	  	since it will be rebuilt from scratch.
 *	for_search: means that the database will only be used for searching.
 *		if this is false, then it can be searched and added to.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 *
 */

long ext_open_database _AP((database *db, 
			    boolean initialize, 
			    boolean for_search));

/*
 *  ext_close_database: This function will be called after all operations 
 *	on this database are done.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long ext_close_database _AP((database *db));

char *database_file _AP((char *database_name));

/* ============================
 * ===  Building Functions  ===
 * ============================*/

/* init_add_word add_word... finished_search_word 
 *  is the sequence for creating an update.  When a finished_add_word is done,
 *  then the it is safe (and expected) that the builder will flush things to files.
 *  set query parameter can be called at any time between documents during an add.
 */

/*
 *  init_add_word: called before any calls to add_word.  finished_add_word
 *   	will be called before another init_add_word is called.
 *	db is the one that will be added to.
 * 	parameter1 and parameter2 are implementation specific.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 * 
 */

long init_add_word _AP ((database *db, 
			 long parameter1, long parameter2));

/*
 *  add_word: add this word to the database
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */
long add_word _AP((
		   char *word,	/* the word to be indexed, this could be a
				   word pair. If NULL there are no more words
				   to be indexed */
		   long char_pos, /* the position of the start of the
				     word */
		   long line_pos, /* this is passed for the best
				     section calculation */
		   long weight,	/* how important the word looks
				   syntactically (such as is it bold)
				   NOT used by signature system */
		   long doc_id,	/* current document, this will never be 0 */
		   time_t date, /* display day of this document, 0 if not known */
		   long word_pair, /* 1 if it is, 0 if not */
		   database* db, /* database to insert the document */
		   boolean word_position /* whether the position is valid or not */
		   ));

/*
 *  finished_add_word: states that there are no more words to add
 *   to this database.
 *
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long finished_add_word _AP((database *db));

#ifdef FIELDS /* tung, 1/94 */
long field_finished_add_word _AP((database *db, long field_id));
#endif
/* ===============================
 * ===  Maintenance Functions  ===
 * ===============================*/

/*
 *  delete_doc_id : delete a document
 *   return values:  0, successfull
 *                  -1, document not found
 *
 */
long delete_doc_id _AP((long doc_id, database *db));

/* =============================
 * ===  Searching Functions  ===
 * =============================*/


/*
 *  set_query_parameter : set query parameter
 *      set search attributes such as date factor, document source ids,
 *      and maximum number of documents returned in a search ( the last
 *      one is an important performance factor to signature  type system)
 *      The search artributes applies to all comming queries until
 *      they are re-set by next set_query_parameter call.
 *
 *   return values:  none
 *
 */
#define SET_MAX_RETRIEVED_MASK 1
#define SET_DATE_FACTOR_MASK   2
#define SET_SELECT_SOURCE      4

/* enum literals for date_factor */
#ifndef DF_INDEPENDENT
#define DF_INDEPENDENT          1
#define DF_LATER                2
#define DF_EARLIER              3
#endif

typedef struct {
  long max_hit_retrieved;
  /* max number of hits can be returned by
   * the search engine. For a signature
   * type system, the default value is 20
   */
  long date_factor;		/* default is DF_INDEPENDENT */
  long num_db;			/* value of zero indicating select all,
				 * default is selecting all
				 */
  char **srcs;			/* string of sources to be searched */
}  query_parameter_type;

/*
 *  set_query_parameter: set a mode variable for the search engine
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long set_query_parameter _AP ((
			 long mask,
			 query_parameter_type *parameters
			 /* fields in the query parameter structure are only
			  * interpreted when the corresponding mask bit 
			  * is set in the mask argument.
			  */
			 ));



/*
 *  init_search_word: called before any search_word is called in this query.
 * 	The only operations that occur after this is search_word.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long init_search_word _AP((database* db));

/*
 *  search_word: searches for a word in the index.  it side effects 
 *               internal state so that best_hit will return the correct 
 *               results.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

#ifdef FIELDS /* tung, 5/94 */	
/* AIX does not like this prototype!? */
long search_word 
  _AP ((char *word,             /* the word to be searched for */
	char *field_name,
	long char_pos,		/* the position of the start of the word */
	long line_pos,		/* is this needed? not for signature system */
	long weight,		/* missused by Tung */
	double query_weight,	/* how important the word looks syntactically,
				   such as is it bold */
	long doc_id,            /* current document, seed words is 0,
				   then it increments into the relevant 
				   document */
	long word_pair,         /* 1 if it is, 0 if not */
	database *db
	));
#else
long search_word 
  _AP ((char *word, /* the word to be searched for */
	long char_pos,		/* the position of the start of the word */
	long line_pos,		/* is this needed? not for signature system */
	long weight,		/* how important the word looks syntactically,
				   such as is it bold */
	long relevant_doc_number,/* current document, seed words is 0,
				   then it increments into the relevant 
				   document */
	long word_pair, /* 1 if it is, 0 if not */
	database *db
	));
#endif


/*
 *  finished_search_word: states that there are no more words that will
 *   be searched for before best_hit will be called.
 *
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long finished_search_word _AP((database *db));



/*
 *  init_best_hit: called before any best_hit is called in this query.
 * 	The only operations that occur after this is best_hit.
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long init_best_hit _AP((database *db));

/*
 *  best-hit :
 *
 *   return values:  0, successfull
 *                  -1, no more documents to return
 *		    Other values returned to signal future signals.
 *
 */
long best_hit _AP ((database* db,long *doc_id, long *best_character, 
		    long *best_line, double *score, long *start,
		 long *end,long *date,long *length,long *nlines,
		 char *headline,char *filename,char *type));


/*
 *  finished_best_hit: states that there are no more best_hits will be called
 *   before the next set of search_words or add_words.
 *
 *   return values: 0 if successful, non-0 if error
 *       defined error conditions:
 *
 */

long finished_best_hit _AP((database* db));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* ndef IREXT_H */
