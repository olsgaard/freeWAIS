/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* This file defines the files of an inverted file index.
 *
 * This structure is designed to be flexible rather than particularly
 * optimized for speed or space.
 * Thus this organization can support:
 *   boolean, proximity, weights, and relevance feedback.
 *
 * Ported directly from the Lisp version 1.2 of the search engine.
 *
 * -brewster 6/90
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/irfiles.c,v 2.0.1.9 1996/01/31 14:48:29 pfeifer Exp $";
#endif /* not lint */

/* ==================== */
/* ===  Change Log  === */
/*Created 12/4/89 Brewster full lisp version
 *split from ir-engine 1/11/90 brewster
 *
 *added memory indexing for efficiency
 *added variable index block sizes
 *5/90 ported to C
 *5/90 split from irbuild.c
 *7/90 declared truename() a static function - HWM 
 *7/90 changed filename table and headline table to be null
 *     terminated in the file rather than \newline.
 *     compatibility problems between systems (sigh).
 *     -brewster
 *7/90 added field to document table for WAIStation
 *     -brewster
 *7/90 fixed: BUG: when adding words to the word disk hashtable, watch out 
 *     for the end of the file and wrap.  If it is full, error out.
 *3/91 took out utilities and created futil.c -brewster
 *3/91 took out the inverted file and created irinv.c -brewster
 *
 * $Log: irfiles.c,v $
 * Revision 2.0.1.9  1996/01/31 14:48:29  pfeifer
 * patch63: Check for presence of keywords vefore printing them ;-).
 *
 * Revision 2.0.1.8  1995/12/28 16:44:41  pfeifer
 * patch59:
 *
 * Revision 2.0.1.7  1995/12/18 13:52:36  pfeifer
 * patch56: Rename char* keyword to char* keywrd sice it conficts with
 * patch56: keywords() on some systems.
 *
 * Revision 2.0.1.6  1995/11/17  14:52:46  pfeifer
 * patch52: Index types are now reported in the <database>.src file.
 *
 * Revision 2.0.1.5  1995/11/14  16:13:52  pfeifer
 * patch50: partial more verbose?
 *
 * Revision 2.0.1.4  1995/11/09  14:06:36  pfeifer
 * patch46: Fixed a very tricky memory bug which must be in there since
 * patch46: wais8-b.
 *
 * Revision 2.0.1.3  1995/10/10  12:38:11  pfeifer
 * patch31: Make matching partial words available in search_word.
 *
 * Revision 2.0.1.3  1995/10/10  12:38:11  pfeifer
 * patch31: Make matching partial words available in search_word.
 *
 * Revision 2.0.1.2  1995/10/06  17:38:43  pfeifer
 * patch24: Added code to document date and numeric fields in the source
 * patch24: description.
 *
 * Revision 2.0.1.1  1995/09/11  12:49:13  pfeifer
 * patch4: Replaced getenv("USER") by getuser_env_name().
 *
 * Revision 2.0  1995/09/08  07:59:37  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.39  1995/06/08  11:38:00  pfeifer
 * Tom Snee: replaced bad MAX_FILE_NAME_LEN by MAX_HEADLINE_LEN when
 * reading headlines in read_headline_table_entry.
 *
 * Revision 1.38  1995/05/07  15:28:02  pfeifer
 * Sicherung vor neuen patches von tsnee
 *
 * Revision 1.37  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.36  1995/04/17  13:16:08  huynh1
 * Release 1.1.2
 *
 * Revision 1.35  1994/12/22  14:01:36  pfeifer
 * added :description to  :fields
 *
 * Revision 1.34  1994/12/22  10:41:48  pfeifer
 * Added :fields to source files
 *
 * Revision 1.33  1994/12/13  18:52:45  pfeifer
 * chip@chinacat.unicom.com (Chip Rosenthal) patches.
 * Excluding the merge of libinv and libwais
 *
 * Revision 1.32  1994/09/06  16:52:58  pfeifer
 * Removed #include <stdlib.h> which is now done in cdialect.h
 *
 * Revision 1.31  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.30  1994/08/23  13:06:48  pfeifer
 * removed irlex.h. Code now in Defaults.tmpl
 *
 * Revision 1.29  1994/08/22  14:10:47  pfeifer
 * Some fixes to make purify happy (a the memory not wasted)
 * jp made the getenv code safer
 *
 * Revision 1.28  1994/08/15  16:18:18  pfeifer
 * fixed unchecked usage of getenv("USER") in printf
 *
 * Revision 1.27  1994/08/08  10:29:45  pfeifer
 * Fixed the MAX_OCCURANCES - STOP_WORD_FLAG bug
 *
 * Revision 1.26  1994/08/05  08:53:59  pfeifer
 * Fix for total_word_count bug
 *
 * Revision 1.25  1994/08/05  07:12:09  pfeifer
 * Release beta 04
 *
 * Revision 1.24  1994/07/22  12:30:05  huynh1
 * left for holidays
 *
 * Revision 1.23  1994/07/15  17:11:03  pfeifer
 * negative numerics
 *
 * Revision 1.22  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.21  1994/07/05  17:14:21  huynh1
 * bug by numeric searching (>) corrected.
 *
 * Revision 1.20  1994/06/24  11:18:56  huynh1
 * bug by stemming corrected.
 *
 * Revision 1.19  1994/06/09  18:34:01  pfeifer
 * added ftp:
 *
 * Revision 1.18  1994/06/09  17:14:40  pfeifer
 * fixed the url stuff in write_filename_table_entry to make it not
 * prepend the path.
 *
 * Revision 1.17  1994/06/09  16:51:22  pfeifer
 * added a hack in write_filename_table_entry, to avoid testing of date
 * for files starting withh "http:". So URL can be put in the docid!!
 *
 * Revision 1.16  1994/06/03  10:49:11  huynh1
 * bug in (field_)finished_add_word_to_dictionary corrected.
 *
 * Revision 1.15  1994/05/31  14:29:46  pfeifer
 * dict_number_of_blocks: added 4 to make it run with 2999 words ????
 *
 * Revision 1.14  1994/05/26  15:41:18  huynh1
 * look_up_total_word_count, field_look_up_total_word_count upadted.
 * beta.
 *
 * Revision 1.13  1994/05/20  12:57:24  huynh1
 * beta
 *
 * Revision 1.12  1994/04/28  16:07:07  huynh1
 * *** empty log message ***
 *
 * Revision 1.11  1994/03/25  11:11:00  huynh1
 * fix bugs in partial search corrected.
 * faster numeric search. Patchlevel 07.
 *
 * Revision 1.10  1994/03/23  12:55:42  huynh1
 * openDatabase modified.
 * patchlevel 07.
 *
 * Revision 1.9  1994/03/21  12:23:21  pfeifer
 * *** empty log message ***
 *
 * Revision 1.8  1994/03/10  17:33:36  huynh1
 * Patchlevel 05
 *
 * Revision 1.7  1994/03/08  20:42:18  huynh1
 * Patchlevel 04
 *
 * Revision 1.6  1994/02/14  10:30:29  huynh1
 * functions for field concept added,
 * fix bug in parttial search corrected.
 *
 * Revision 1.5  1993/07/01  19:34:50  warnock
 * explicit declaration of gSavepart in savePartMatch
 *
 * Revision 1.4  93/07/01  19:18:54  warnock
 * gethostname -> mygethostname
 * 
 * Revision 1.3  1993/02/16  17:07:49  freewais
 * added AT&T patches for keyword list
 *
 * Revision 1.2  1993/02/16  15:32:56  freewais
 * changed directory of servers registration to cnidr.org
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.63  92/04/28  16:54:41  morris
 * added boolean support
 * 
 * Revision 1.62  92/03/20  13:57:04  jonathan
 * New and Improved server registration.
 * 
 * Revision 1.61  92/03/19  10:38:27  shen
 * modified lock to prevent more than one indexing at the same time.
 * modified lock to block query while initilaizing a database
 * 
 * Revision 1.60  92/03/19  09:33:35  morris
 * fixed the dictionary header to accurately indicate the number of blocks
 * 
 * Revision 1.59  92/02/27  12:25:27  shen
 * add in locks
 * 
 * Revision 1.58  92/02/25  16:42:28  jonathan
 * Added find_pointer_in_block using binary search from
 * ses@techunix.technion.ac.il. (part of wais-8-b3-ses).
 * 
 * 
 * Revision 1.57  92/02/25  12:49:16  jonathan
 * removed a bunch of \n's from waislog's.
 * 
 * Revision 1.56  92/02/17  16:23:58  jonathan
 * Modified build_catalog so it passes over the first entry (which seems to be
 * empty).
 * 
 * Revision 1.55  92/02/17  12:37:34  jonathan
 * Added code to build a catalog containing all headlines and DocID's for
 * documents in the database.
 * 
 * Revision 1.54  92/02/16  09:50:49  jonathan
 * plugged some memory leaks.  I bet there are more.
 * 
 * Revision 1.53  92/02/16  09:26:39  jonathan
 * ask harry.
 * 
 * Revision 1.52  92/02/12  13:25:12  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 */
/* ==================== */

/* ==================== */
/*     To Do list       
 *
 * Implement a filename hashtable so that we can test quickly when
 *   a file has been indexed.
 * Free up all memory when we can. 
 * Implement logrithmic merging
 * 
 * change DOC_TAB_ENTRY_FILENAME_ID_SIZE to 4 This must be in version 9
 * change DOC_TAB_ENTRY_HEADLINE_ID_SIZE to 4 This must be in version 9
 * change DOC_TAB_ENTRY_NUM_LINES_SIZE to 4 This must be in version 9
 * change MAX_WORD_LENGTH to 15 This must be in version 9
 */

/* A specification for this is called ir-engine.text in microsoft word. */

/* #include <string.h> /* for memset() */
#include "irhash.h"
#include "cutil.h"
#include "irfiles.h"
#include "panic.h"
#include "ustubs.h" /* for strstr */
#include "futil.h"
#include "sockets.h"
#include "irext.h"
#include "lock.h"

#ifdef FIELDS
#include "../config.h"
#include "field_search.h"
#endif /* FIELDS */

 extern char* keywrd[50];
 extern short nKeys;
 char *descript[1000];
 short nDesLines = 0;

#define PRINT_AS_INDEXING false /* also defined in irtfiles.c and irhash.c */

/*      -------------------------------    */
#define DOC_TAB_HEADER_SIZE 2
#define DOC_TAB_MAXIMUM_ENTRIES 8192
#define DOC_TAB_ENTRY_FILENAME_ID_SIZE 3
#define DOC_TAB_ENTRY_START_CHAR_SIZE 4
#define DOC_TAB_ENTRY_END_CHAR_SIZE 4
#ifdef HUGE_HEADLINES
#define DOC_TAB_ENTRY_HEADLINE_ID_SIZE 4
#else /* not HUGE_HEADLINES */
#define DOC_TAB_ENTRY_HEADLINE_ID_SIZE 3
#endif /* not HUGE_HEADLINES */
#define DOC_TAB_ENTRY_DOC_LENGTH_SIZE 4
#define DOC_TAB_ENTRY_NUM_LINES_SIZE 3
#define DOC_TAB_ENTRY_DATE_SIZE 4
#ifdef HUGE_HEADLINES
#define DOC_TAB_ELEMENT_SIZE 26 /* sum of above sizes */
#else /* not HUGE_HEADLINES */
#define DOC_TAB_ELEMENT_SIZE 25 /* sum of above sizes */
#endif /* not HUGE_HEADLINES */

#define FILENAME_TABLE_HEADER_SIZE 4
#define HEADLINE_TABLE_HEADER_SIZE 4
#ifdef BIO
#define DELIMITERS_SIZE 4
#endif /* BIO */

#define FILE_WRITE_DATE_SIZE sizeof(Time_t)
#define NUMBER_OF_OCCURANCES_SIZE 4
#define DOCUMENT_SCORE_LIMIT_SIZE 1
#define DOCUMENT_SCORE_LIMIT 255  /* this is computed from DOCUMENT_SCORE_LIMIT_SIZE */

#define TIME_WAIT_QUERY_END 5
#define TIMEOUT_WAIT_QUERY_END 45

static char* temp_dictionary_filename _AP((char* destination, database* db));
#ifdef FIELDS
static char* field_temp_dictionary_filename _AP((char* destination,
                                                 int field_id, database* db));
#endif /* FIELDS */
static long current_lock_type = INVALID_LOCK;

/*============================
  ===   Database support   ===
  ============================*/

#ifdef STEM_WORDS
boolean stemming = false;
#endif /* STEM_WORDS */

/* looks up the total word count in an existing dictionary. */
 boolean look_up_total_word_count _AP((database *db));
 boolean look_up_total_word_count(db)
database *db;
{
  long word_count;
  long answer = look_up_word_in_dictionary(DICTIONARY_TOTAL_SIZE_WORD,
					   &word_count, db);
#ifdef STEM_WORDS
  if(stemming)
    db->stemming = true;
#endif /* STEM_WORDS */
  if(answer == 0){
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "error finding total_word_count in dictionary %s\n", 
	    db->database_file);
    return(false);
  }
  else if(answer < 0){
    waislog(WLOG_HIGH, WLOG_ERROR,"total_word_count not found in dictionary\n.This is either an error,or the database is old.");
    db->total_word_count = word_count;
    return(false);
  }
  else{
    db->total_word_count = word_count;
  }
  /* printf("Total Words in DB: %ld\n", db->total_word_count); */
  return(true);
}

#ifdef FIELDS
boolean field_look_up_total_word_count _AP((char* field_name, long field_id, database *db));
boolean field_look_up_total_word_count(field_name, field_id, db)
char* field_name;
long field_id;
database *db;
{
  long word_count;
  long answer = field_look_up_word_in_dictionary(field_name, 
						 DICTIONARY_TOTAL_SIZE_WORD,
                                                 &word_count, db);
  if(answer == 0){
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "error finding total_word_count in dictionary %s of field %s\n", 
	    db->database_file, field_name);
    return(false);
  }
  else if(answer < 0){
    waislog(WLOG_HIGH, WLOG_ERROR,"total_word_count not found in dictionary\n.This is either an error,or the database is old.");
    db->fields[field_id].total_word_count = word_count;
    return(false);
  }
  else{
    db->fields[field_id].total_word_count = word_count;
  }
  /* printf("Total Words in DB: %ld\n", db->total_word_count); */
  return(true);
}
#endif /* FIELDS */

#ifdef FIELDS
database* 	
openDatabase(name,initialize,for_search, type_field)
#else /* not FIELDS */
database*
openDatabase(name,initialize,for_search)
#endif /* not FIELDS */
char* name;
boolean initialize;
boolean for_search;
#ifdef FIELDS
boolean type_field;
#endif /* FIELDS */
{
  /* open a database (open all its files), and return an opaque object.
     return NULL if there is an error
   */
  unsigned long pid;
  long timeout;
  char file[MAX_FILE_NAME_LEN + 1 ];
  char tmpfile[MAX_FILE_NAME_LEN + 1];
  char open_mode[4];
  database* db = (database*)s_malloc((size_t)sizeof(database));	
  if (db == NULL){ 
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "can't make a database, out of memory.\n");
    return(NULL);
  }

  db->total_word_count = 0;

  if (for_search == true)  
    strncpy(open_mode,"rb",3); /* read only for searching */
  else 
    strncpy(open_mode,"r+b",4); /* read/write for building */

  /* set the query parameter to the original name */
  {
    query_parameter_type parameters;
    char **list;
    list=(char **)s_malloc(2*sizeof(char*));
    list[0]=s_strdup(name);
    list[1]=NULL;
    parameters.srcs = list;
    set_query_parameter(SET_SELECT_SOURCE,&parameters);
  }
   
  /* ask the backend where the database lives, but put in the 
     directory information that we already have.  This changes
     the 'name' variable. */
  db->database_file = 
    s_strdup(merge_pathnames(database_file(pathname_name(name)),
			     pathname_directory(name, tmpfile)));
  if (for_search == true) {
  
    /* check and set appropriate locks */

    if( utlk_using_lock(db->database_file, LOCK_UPDATE) ) {
      waislog(WLOG_HIGH, WLOG_ERROR,
       "can't search the database as an update is currently running");
        return(NULL);
      }
    if ( utlk_set_lock(db->database_file, LOCK_QUERY) )
       current_lock_type = LOCK_QUERY;
    else
      waislog(WLOG_LOW, WLOG_INFO, "query lock can't be set");

    }

  else {

    if( utlk_using_lock_and_get_pid(db->database_file, LOCK_INDEX, &pid) &&
        (pid != getpid()) ) {
      waislog(WLOG_HIGH, WLOG_ERROR,
       "an indexing is currently running on the database. Try again later.");
        return(NULL);
      }
    if (  utlk_set_lock(db->database_file, LOCK_INDEX) )
       current_lock_type = LOCK_INDEX;
    else
      waislog(WLOG_LOW, WLOG_INFO, "index lock can't be set");
    if ( initialize == true ) {
     /* wait for current query finishing off */
     timeout = 0;
     while ( utlk_using_lock(db->database_file, LOCK_QUERY) ) {
      if ( timeout >= TIMEOUT_WAIT_QUERY_END ) {
         waislog(WLOG_HIGH, WLOG_ERROR,
             "timed out in waiting for a query to finish. Try again later.");
         utlk_unset_lock(db->database_file, LOCK_INDEX);
         return(NULL);
         }
      waislog(WLOG_LOW, WLOG_INFO,
             "waiting for a query to finish to initialize the database...");
      sleep(TIME_WAIT_QUERY_END);
      timeout += TIME_WAIT_QUERY_END;
      }
     if (  utlk_set_lock(db->database_file, LOCK_UPDATE) )
       current_lock_type = LOCK_UPDATE;
     else
       waislog(WLOG_LOW, WLOG_INFO, "update lock can't be set");
     }

    }
  
  if(initialize == true){
    initialize_index_files(db);
  } 
  else 	{

    /* if the global database does not exist it is not nessesary to open
     * the dictionary_stream.
     */
#ifdef FIELDS
    if(type_field == false) {
#endif /* FIELDS */
      db->dictionary_stream = 
        s_fopen(dictionary_filename(file, db),open_mode);
      if (db->dictionary_stream == NULL){ 
        waislog(WLOG_HIGH,WLOG_ERROR,"can't open the word hash file %s\n",file); 
        disposeDatabase(db);
        return(NULL);
      }
#ifdef FIELDS
    }
#endif /* FIELDS */
    /* find the total_word_count from the dictionary */
    if(for_search){
#ifdef FIELDS
      db->number_of_fields = 0;
#endif /* FIELDS */
#ifdef FIELDS
      if(type_field == false) {
#endif /* FIELDS */
        if(false == look_up_total_word_count(db)) { /* side effects db */
          disposeDatabase(db);
          return(NULL);
        }
#ifdef FIELDS
      }
#endif /* FIELDS */
    }
    
    db->filename_table_stream =
      s_fopen(filename_table_filename(file, db),open_mode);
    if (db->filename_table_stream == NULL){ 
      waislog(WLOG_HIGH, WLOG_ERROR,
	      "can't open the filename file %s", file); 
      disposeDatabase(db);
      return(NULL);
    }
		
    db->headline_table_stream = 
      s_fopen(headline_table_filename(file, db),open_mode);
    if (db->headline_table_stream == NULL){
      waislog(WLOG_HIGH, WLOG_ERROR,
	      "can't open the headline file %s", file); 
      disposeDatabase(db);
      return(NULL);
    }

#ifdef BIO
    db->delimiters_stream = 
      s_fopen(delimiters_filename(file, db),open_mode);
    if (db->delimiters_stream == NULL){
      waislog(WLOG_HIGH, WLOG_ERROR,
	      "can't open the delimiters file %s, using defaults", file); 
      /* disposeDatabase(db); */
      /* return(NULL); */
    }
#endif /* BIO */

    db->document_table_stream = 
      s_fopen(document_table_filename(file, db),open_mode);
    if (db->document_table_stream == NULL){ 
      waislog(WLOG_HIGH, WLOG_ERROR,
	      "can't open the document id file %s", file); 
      disposeDatabase(db);
      return(NULL);
    }
      
    /* initialize the allocated entries variable */
    s_fseek(db->document_table_stream, 0L, SEEK_END);
    db->doc_table_allocated_entries = 
      (ftell(db->document_table_stream) - DOC_TAB_HEADER_SIZE) 
	/ DOC_TAB_ELEMENT_SIZE;
  }
  db->index_file_number = 0;
#ifdef FIELDS
  if(type_field == false)
#endif /* FIELDS */
    ext_open_database(db,initialize,for_search);
  return(db);
}


void		
closeDatabase(db)
database* db;
/* close a database and all its files. Do not dispose of the structure. */
{
  
#ifdef FIELDS
  int i, j, k;
#endif /* FIELDS */
  
  if (db == NULL)
    return;
  close_dictionary_file(db);
  if (db->dictionary_stream != NULL)
    s_fclose(db->dictionary_stream);
  if (db->filename_table_stream != NULL)
    s_fclose(db->filename_table_stream);
  if (db->headline_table_stream != NULL)
    s_fclose(db->headline_table_stream);
  if (db->document_table_stream != NULL)
    s_fclose(db->document_table_stream);
  if (db->index_stream != NULL)
    s_fclose(db->index_stream);

#ifdef FIELDS
  if(db->index_fields != NULL) {
    clear_fields(db);
    for (i = 0; i < db->number_of_fieldstreams; i++) {
        if(db->field_dictionary_streams != NULL) {
            if(db->field_dictionary_streams[i] != NULL)
                s_fclose(db->field_dictionary_streams[i]);
        }
        if(db->field_index_streams != NULL) {
            if(db->field_index_streams[i] != NULL)
                s_fclose(db->field_index_streams[i]);
        }
    }
    s_free(db->field_index_streams);
    s_free(db->field_dictionary_streams);

    if (db->fields != NULL)
    for(i=0; i < db->number_of_fields; i++) {
      if(db->fields[i].field_name != NULL)
        s_free(db->fields[i].field_name);
    }
  }
  else {
    for(i=0; i < db->number_of_fields; i++) {
      if (db->field_dictionary_streams != NULL) {
        if (db->field_dictionary_streams[i] != NULL)
          s_fclose(db->field_dictionary_streams[i]);
      }
      if (db->field_index_streams != NULL) {
        if (db->field_index_streams[i] != NULL)
          s_fclose(db->field_index_streams[i]);
      }
      if(db->fields != NULL) {
        if(db->fields[i].field_name != NULL)
          s_free(db->fields[i].field_name);
      }
    }
    if (db->field_dictionary_streams != NULL)
      s_free(db->field_dictionary_streams);
    if (db->field_index_streams != NULL)
      s_free(db->field_index_streams);
  }
  if(db->fields != NULL)
    s_free(db->fields);
  db->number_of_fields = 0;
#endif /* FIELDS */
  
  ext_close_database(db);
  utlk_unset_lock(db->database_file, current_lock_type);
  if ( current_lock_type == LOCK_UPDATE)
    utlk_unset_lock(db->database_file, LOCK_INDEX);
  current_lock_type = INVALID_LOCK;
  
}

void 
disposeDatabase(db)
database* db;
{
	closeDatabase(db);
	s_free(db->database_file);
	s_free(db);
}

/* ==================================== */
/* ===  Initialization of the files === */
/* ==================================== */

#define BLOCK_SIZE 16384 /* size of blocks of zeros to write to a file */

static FILE* initialize_file _AP((long size,char* filename,boolean zero_it));

static FILE* initialize_file(size,filename,zero_it)
long size;
char* filename;
boolean zero_it;
/* initializes a file by opening a new stream, making it the right
 * size and returning the stream.
 */
{
  FILE* file = NULL;
  long i;

#ifdef ANSI_LIKE
  remove(filename);
#endif /* ANSI_LIKE */

  file = s_fopen(filename, "wb");
  if(NULL == file){ 
    panic("The file %s could not be opened\n", filename);
  }

  if(zero_it){
    if(size >= BLOCK_SIZE){	/* then write big blocks of zeros */
      char* zeros = NULL;
      zeros = (char*)s_malloc((size_t)BLOCK_SIZE);
      if(NULL == zeros){
	panic("Could not allocate a large block of Zeros\n");
      }
      memset(zeros, 0, BLOCK_SIZE);
      while(size >= BLOCK_SIZE){	
	/* then write big blocks of zeros */
	if(BLOCK_SIZE != fwrite(zeros, 1, BLOCK_SIZE, file))
	  panic("Write failed");
	size = size - BLOCK_SIZE;
      }
      s_free(zeros);
    }
    for(i = 0; i < size; i++){	/* clean up the rest */
      putc('\0', file); 
    }
  }	
  else{				/* dont zero it */
    grow_file(file, size);
  }	

#ifdef THINK_C
  /* set the mac file type to INDX */
  setFileType(filename, WAIS_INDEX_FILE_TYPE, CREATOR);
#endif /* THINK_C */

  s_fclose(file);
  file = s_fopen(filename, "r+b"); /* open it in read/write */
  if(NULL == file){
    panic("Error in initialization, can not reopen %s.\n", filename);
  }
  return(file);
}

void initialize_index_files (db)
database* db;
/* This creates new index files, deleting any old ones. */
{
  char file[MAX_FILENAME_LEN];

  /* cprintf(PRINT_AS_INDEXING, "initializing index files: %s\n", db->database_file); */

  remove(dictionary_filename(file, db)); /* remove the old one */

  db->index_stream = NULL;

  db->doc_table_allocated_entries = 1; /* the 0th is the null pointer */
  db->document_table_stream =
    initialize_file((DOC_TAB_HEADER_SIZE + DOC_TAB_ELEMENT_SIZE),
		    document_table_filename(file, db), TRUE);
  db->filename_table_stream =
    initialize_file(FILENAME_TABLE_HEADER_SIZE,
		    filename_table_filename(file, db), TRUE);
  db->headline_table_stream =
    initialize_file(HEADLINE_TABLE_HEADER_SIZE,
		    headline_table_filename(file, db), TRUE);
#ifdef BIO
  db->delimiters_stream =
    initialize_file(DELIMITERS_SIZE,
		    delimiters_filename(file, db), TRUE);
#endif /* BIO */
}

/* ========================= */
/* ===  Dictionary File  === */
/* ========================= */

/* The dictionary file is a 1 deep tree of blocks.  
   The header of the file says how long the header block is.
   The "header block" is a set of pointers to the heads of
   the blocks in the dictionary.

   A dictionary block is a list of word and pointer pairs.  The words
   are padded to a fixed length so that it is a fixed length record.
   The pointers are pointers into the inverted file (except in the header
   block where they are pointers into the dictionary file).
*/

/*  SEARCHING DICTIONARY FILES */
   
/* top level function:
   long look_up_word_in_dictionary(char *word, long *word_id, database* db) 
 */

unsigned char *dictionary_header_block = NULL; /* the dictionary header. 
						  loaded once */
long number_of_dictionary_blocks = 0;  /* also the length of the dictionary 
					  header block */

unsigned char *dictionary_block = NULL; /* this is one of the dict blocks */

int dictionary_last_word_occurances; /* This is a temporary hack so I can 
					separate out the relevance feedback 
					changes for posting. DON'T USE THIS 
					ANYWHERE - IT'LL BE GONE SOON
				      */

#ifdef FIELDS
unsigned char *field_dictionary_header_block = NULL;
long field_number_of_dictionary_blocks = 0;
unsigned char *field_dictionary_block = NULL;
int field_dictionary_last_word_occurances;
#endif /* FIELDS */

void close_dictionary_file(db)
     database *db;
{
  if(dictionary_header_block) s_free(dictionary_header_block);
  dictionary_header_block = NULL;
#ifdef FIELDS
  if(field_dictionary_header_block) s_free(field_dictionary_header_block);
  field_dictionary_header_block = NULL;
#endif /* FIELDS */
}


long fread_from_stream(stream,buf,nbytes)
FILE *stream;
unsigned char *buf;
long nbytes;
/* this is a safe version of unix 'fread' it does all the checking
 * and looping necessary
 */
{
  long didRead;
  long toRead = nbytes;
  long totalRead = 0;		/* paranoia */
  /*printf("in Fread_from_stream buffer %ld, nbytes %ld\n", (long)buf, nbytes); */

  while (toRead > 0){
    didRead = fread(buf, sizeof(char), toRead, stream);
    if(didRead == -1)		/* error*/
      return(-1);
    if(didRead == 0)		/* eof */
      return(-2);		/* maybe this should return 0? */
    toRead -= didRead;
    buf += didRead;
    totalRead += didRead;
  }
  if(totalRead != nbytes)	/* we overread for some reason */
    return(- totalRead);	/* bad news */    
  return(totalRead);
}

#ifdef DICT_FUNC
char *dictionary_block_word(i,block)
long i;
unsigned char *block;
/* returns the word field in the ith dictionary block entry */
{
  return((char *)(block + (i * DICTIONARY_ENTRY_SIZE)));
}

long dictionary_block_position(i,block)
long i;
unsigned char *block;
/* returns the position field in the ith dictionary block entry */
{
  /* printf("dictionary_block_position %ld\n",
	 read_bytes_from_memory
	 (NEXT_INDEX_BLOCK_SIZE,
	  block + (i * DICTIONARY_ENTRY_SIZE) + 
	  MAX_WORD_LENGTH + 1)); */
  return(read_bytes_from_memory
	 (NEXT_INDEX_BLOCK_SIZE,
	  block + (i * DICTIONARY_ENTRY_SIZE) + 
	  MAX_WORD_LENGTH + 1));
}

long dictionary_block_word_occurances(i,block)
long i;
unsigned char *block;
/* returns the occurances field in the ith dictionary block entry */
{
  return(read_bytes_from_memory
	 (NEXT_INDEX_BLOCK_SIZE,
	  block + (i * DICTIONARY_ENTRY_SIZE) + 
	  MAX_WORD_LENGTH + 1 + NEXT_INDEX_BLOCK_SIZE));
}
#endif /* DICT_FUNC */


#ifdef PARTIALWORD

typedef struct {
  long            blocknum, wordcount;
  char            word[MAX_WORD_LENGTH + 1];
} saveparttype;
	
static long gMaxpart = 0;
static long gNpart = 0;
static long gAtpart = 0;
static saveparttype *gSavepart = NULL;

void clearPartMatch()
{
  if (gSavepart!=NULL)
    s_free(gSavepart);
  gSavepart= NULL;
  gMaxpart= 0;
  gNpart= 0;
  gAtpart= 0;
}

void 
savePartMatch (blocknum, wordcount, word)
     long            blocknum, wordcount;
     char           *word;
{
  if (gNpart >= gMaxpart) {
    gMaxpart = gNpart + 100;
    if (gSavepart == NULL)
      gSavepart = (saveparttype *) s_malloc (gMaxpart * sizeof (saveparttype));
    else
      gSavepart = (saveparttype *) s_realloc (gSavepart, gMaxpart * sizeof (saveparttype));
  }
  if (gSavepart == NULL)
    panic ("Out of memory");
  gSavepart[gNpart].blocknum = blocknum;
  gSavepart[gNpart].wordcount = wordcount;
  strncpy(gSavepart[gNpart].word,word,MAX_WORD_LENGTH);
  gNpart++;
}
#endif /* PARTIALWORD */

#ifdef FIELDS
long* dictionary_block_pos_set = NULL;
long max_number_of_blocks = 0;
long min_number_of_blocks = 0;
char gnumeric_tag = 0;
double current_search_number = 0;
boolean gnumeric = false;

static void clearDctBlPsSet _AP((void));
static void clearDctBlPsSet()
{
  if(dictionary_block_pos_set != NULL)
    s_free(dictionary_block_pos_set);
  max_number_of_blocks = 0;
  min_number_of_blocks = 0;
  gnumeric_tag = 0;
  current_search_number = 0;
  gnumeric = false;
}

static void saveDctBlPsSet _AP((long blocknum));
static void saveDctBlPsSet(blocknum)
     long  blocknum;
{
  if(min_number_of_blocks >= max_number_of_blocks) {
    max_number_of_blocks = min_number_of_blocks + 100;
    if(dictionary_block_pos_set == NULL)
      dictionary_block_pos_set  = 
        (long*)s_malloc((size_t)(sizeof(long) * max_number_of_blocks));
    else dictionary_block_pos_set  = 
      (long*)s_realloc(dictionary_block_pos_set, 
                       (size_t)(sizeof(long) * max_number_of_blocks));
  }
  dictionary_block_pos_set[min_number_of_blocks] = blocknum;
  min_number_of_blocks++;
}

static long handle_numeric _AP((long number_of_elements, long *number_of_occurances, char* field_name, unsigned char* block, database* db));
static long handle_numeric(number_of_elements, number_of_occurances, field_name, block, db)
     long number_of_elements;
     long *number_of_occurances;
     char* field_name;
     unsigned char *block;
     database* db;
{
  long i,j=0;
  long field_id;
  long block_length = DICTIONARY_BLOCK_SIZE;
  FILE* stream = NULL;
  
  field_id = pick_up_field_id(field_name, db);
  stream = db->field_dictionary_streams[field_id];
  
  for(i=0; i < number_of_elements; i++) {
    long dictionary_block_pos = dictionary_block_pos_set[i];
    block = 
      read_dictionary_block(block, ABSOLUTE(dictionary_block_pos),
                            DICTIONARY_BLOCK_SIZE,stream);
    if(NULL == block)
      { waislog(WLOG_HIGH, WLOG_ERROR,
                "Could not read dictionary block %ld in db %s",
                ABSOLUTE(dictionary_block_pos),
                db->database_file);
        return(0);
      }
    j=0;
    while(j<block_length) {
      char* dictionary_word = dictionary_block_word(j, block);
      if(dictionary_word[0] == '\0')
        break;
      else if(isnumchar(dictionary_word[0])) {
        double dictionary_number;
	if(dictionary_word[0] == '-') {
	  dictionary_number = atof(dictionary_word + 1) * (-1);
	} else dictionary_number = atof(dictionary_word);
        /* if(dictionary_number == 0.0) break; */
        if(gnumeric_tag == GREATER) {
          /* searching for position where 
           * dictionary_number > current_search_number.
           */
          if(dictionary_number < current_search_number) {
            long low = 0;
            long high = block_length;
            long idx = (low+high)/2;
	    boolean greater = false;
            while(low != high) {  /* binary searching */
              dictionary_word = dictionary_block_word(idx, block);
              if(dictionary_word[0] == '\0' || 
                 !strncmp(dictionary_word, DICTIONARY_TOTAL_SIZE_WORD, 
			  strlen(DICTIONARY_TOTAL_SIZE_WORD))) {
                if(high != idx) {
                  high = idx;
                  idx = (low+idx)/2;
                } else {
                  j = block_length; break;
                }
              }
              else if(isnumchar(dictionary_word[0])) {
		if(dictionary_word[0] == '-') 
		  dictionary_number = atof(dictionary_word + 1) * (-1);
		else dictionary_number = atof(dictionary_word);
               /* if(dictionary_number == 0.0) break; */
                if(dictionary_number == current_search_number) {
                  j = idx;
                  break;
                }
                else if(dictionary_number > current_search_number) {
                  if(high != idx) {
                    high = idx;
                    idx = (low+idx)/2;
		    greater = true; 
                  } else {
                    j = idx - 1; break;
                  }
                }
                else {
                  if (low != idx) {
                    low = idx;
                    idx = (0.5+high+idx)/2;
                  } else {
		    if(greater) {
		      j = idx;
		      dictionary_word = dictionary_block_word(j+1, block);
		      if(dictionary_word[0] == '-')
			dictionary_number = atof(dictionary_word + 1) * (-1);
		      else dictionary_number = atof(dictionary_word);
		      if(dictionary_number < current_search_number)
			j++;
		      while(dictionary_number < current_search_number) {
			j++;
			dictionary_word = dictionary_block_word(j, block);
			if(dictionary_word[0] == '-')
			  dictionary_number = atof(dictionary_word + 1) * (-1);
			else dictionary_number = atof(dictionary_word);
			if(dictionary_number > current_search_number)
			  j--;
		      }
		      break;
		    } else {
		      j = block_length; break;
		    }
                  }
                }
              }
            }
          }
          else if(dictionary_number > current_search_number)
            savePartMatch(dictionary_block_position(j, block),
                          dictionary_block_word_occurances(j, block),
                          dictionary_block_word(j, block));
        }
        else if(gnumeric_tag == LESS) {
          if(dictionary_number < current_search_number)
            savePartMatch(dictionary_block_position(j, block),
                          dictionary_block_word_occurances(j, block),
                          dictionary_block_word(j, block));
          else break;
        }
      }
      ++j;
    }
  }
}

static boolean check_numeric _AP((char* word, boolean numeric_field));
static boolean check_numeric(word, numeric_field)
     char* word;
     boolean numeric_field;
{
  char *tmp;
  int word_sign = 1;
  
  /* word is =1990 or <1990 or >1990 */
  if(strlen(word) >  0 && numeric_field &&
     strcmp(word, DICTIONARY_TOTAL_SIZE_WORD)) {
    if(((word[0] != EQ) && (word[0] != GREATER) && (word[0] != LESS)) ||
       ((word[0] == EQ) || (word[0] == GREATER) || (word[0] == LESS))) {
      if((word[0] != EQ) && (word[0] != GREATER) && (word[0] != LESS)){
	gnumeric_tag = '=';
	tmp = word;
      }else {
	gnumeric_tag = word[0];
	tmp = word + 1; /* ignore first character */
      }
      if(word[1] == '-') {
	tmp = word + 2;
	word_sign = -1;
      }
      word = tmp;
      if((tmp = strchr(word, ','))) /* if a number is a real number, */
	word[tmp - word] = '.';     /* e.g. 12,5 -> 12.5 */
      current_search_number = atof(word) * word_sign;
      return(true);
    } else return(false);
  } else return(false);
}
#endif /* FIELDS */

#ifdef FIELDS
static long numeric_find_pointer_in_block _AP ((char *word,
						unsigned char *block,
						long block_length,
						long *position,
						boolean numeric_init,
						boolean numeric,
						char *numeric_tag));

/* Courtesy of Simon Spero <ses@techunix.technion.ac.il> */

static long 
numeric_find_pointer_in_block (word, block, block_length, position,
			       numeric_init, numeric, numeric_tag)
     char           *word;
     unsigned char  *block;
     long            block_length;	    /* in entries */
     long           *position;
     boolean         numeric_init;
     boolean         numeric;
     char           *numeric_tag;

/* returns 0 if an error or if the word is below the lowest block,
   (this confusion between error and NULL is bad, but found late in the 
   design process)
   it returns the positive position if the word is there exactly,
   and the negative of the position of the word before it if the
   word is not there exactly.
   position is set with the entry postion in the block that the word was 
   found.  This is used for searching.
 */
{
  /* find the entry in the dictionary header for this word.
     returns 0 if not found. */
  /* this could be binary search XXX */
  long            i, high, low, tmp, prev_i;

#ifdef PARTIALWORD
  long            wordlen = strlen (word);

#endif /* PARTIALWORD */

  low = 0;
  high = block_length;
  i = (low + high) / 2;

  if (numeric_init) {
    if (!strcmp (DICTIONARY_TOTAL_SIZE_WORD, word)) {
      i = block_length - 1;
      *position = i;
      return (-dictionary_block_position (i, block));
    }
  }
  while (low != high) {
    double          compare;
    char           *dictionary_word = dictionary_block_word (i, block);
    int             dct_sign = 1;
    double          dct_number;

    if (dictionary_word[0] == '\0') {
      if (high != i) {
	high = i;
	i = (low + i) / 2;
      } else {
	*position = i - 1;
	return (-dictionary_block_position (i - 1, block));
      }
    } else {
      long            dctwrd_len = strlen (dictionary_word);

      if (!strcmp (DICTIONARY_TOTAL_SIZE_WORD, word)) {
	compare = -1.0;
	if (!strncmp (dictionary_word, DICTIONARY_TOTAL_SIZE_WORD, strlen (DICTIONARY_TOTAL_SIZE_WORD)) && (dictionary_word[dctwrd_len - 1] == NUMERIC_TAG)) {
	  *position = i;
	  return (dictionary_block_position (i, block));
	}
      } else {
	if (!strncmp (dictionary_word, DICTIONARY_TOTAL_SIZE_WORD, strlen (DICTIONARY_TOTAL_SIZE_WORD)) && (dictionary_word[dctwrd_len - 1] == NUMERIC_TAG)) {
	  compare = 1.0;
	  dictionary_word[dctwrd_len - 1] = '\0';
	} else {
	  if (dictionary_word[0] == '-') {
	    dct_sign = -1;
	    dictionary_word = dictionary_word + 1;
	  }
	  dct_number = atof (dictionary_word) * dct_sign;
	  if (dct_number > current_search_number)
	    compare = 1;
	  else if (dct_number < current_search_number)
	    compare = -1;
	  else
	    compare = 0;
	}
      }
      if (compare == 0.0) {
	if (*numeric_tag == EQ) {
	  *position = i;
	  return (dictionary_block_position (i, block));
	} else if (*numeric_tag == GREATER) {
	  prev_i = i;
	  while (i < block_length) {
	    saveDctBlPsSet (dictionary_block_position (i, block));
	    ++i;
	    gnumeric = true;
	  }
	  *position = prev_i;
	  return (-dictionary_block_position (prev_i, block));
	} else if (*numeric_tag == LESS) {
	  prev_i = --i;
	  while (i >= 0) {
	    saveDctBlPsSet (dictionary_block_position (i, block));
	    --i;
	    gnumeric = true;
	  }
	  *position = prev_i;
	  return (-dictionary_block_position (prev_i, block));
	}
      } else if (compare > 0.0) {
	if (high != i) {
	  high = i;
	  i = (low + i) / 2;
	} else {
	  if (*numeric_tag == EQ) {
	    *position = i - 1;
	    return (-dictionary_block_position (i - 1, block));
	  } else if (*numeric_tag == GREATER) {
	    i = 0;
	    prev_i = i;
	    while (i < block_length) {
	      saveDctBlPsSet (dictionary_block_position (i, block));
	      ++i;
	      gnumeric = true;
	    }
	    *position = prev_i;
	    return (-dictionary_block_position (prev_i, block));
	  } else if (*numeric_tag == LESS) {
	    *numeric_tag = EQ;	/* result is empty */
	    *position = i - 1;
	    return (-dictionary_block_position (i - 1, block));
	  }
	}
      } else {
	if (low != i) {
	  low = i;
	  i = (0.5 + high + i) / 2;
	} else {
	  if (*numeric_tag == EQ) {
	    *position = i;
	    return (-dictionary_block_position (i, block));
	  } else if (*numeric_tag == GREATER) {
	    prev_i = i;
	    while (i < block_length) {
	      saveDctBlPsSet (dictionary_block_position (i, block));
	      ++i;
	      gnumeric = true;
	    }
	    *position = prev_i;
	    return (-dictionary_block_position (prev_i, block));
	  } else if (*numeric_tag == LESS) {
	    prev_i = i;
	    while (i >= 0) {
	      saveDctBlPsSet (dictionary_block_position (i, block));
	      --i;
	      gnumeric = true;
	    }
	    *position = prev_i;
	    return (-dictionary_block_position (prev_i, block));
	  }
	}
      }
    }
  }

  if (i == 0) {
    if (*numeric_tag == GREATER) {
      while (i < block_length) {
	saveDctBlPsSet (dictionary_block_position (i, block));
	i++;
	gnumeric = true;
      }
      *position = 0;
      return (-dictionary_block_position (0, block));
    } else {
      gnumeric = false;
      *position = 0;
      return (0);
    }
  } else {
    gnumeric = false;
    *position = i - 1;
    return (-dictionary_block_position (i - 1, block));
  }
}
#endif /* FIELDS */

static long find_pointer_in_block _AP((char* word,
                                       unsigned char* block,
				       long block_length,
				       long *position,
				       boolean findpart,
                                       boolean findpart_init));

/* Courtesy of Simon Spero <ses@techunix.technion.ac.il> */

static long 
find_pointer_in_block (word, block, block_length, position, findpart, findpart_init)
     char           *word;
     unsigned char  *block;
     long            block_length;	    /* in entries */
     long           *position;
     boolean         findpart;		    /* dgg, partial word match */
     boolean         findpart_init;

/* returns 0 if an error or if the word is below the lowest block,
   (this confusion between error and NULL is bad, but found late in the 
   design process)
   it returns the positive position if the word is there exactly,
   and the negative of the position of the word before it if the
   word is not there exactly.
   position is set with the entry postion in the block that the word was 
   found.  This is used for searching.
 */
{
  /* find the entry in the dictionary header for this word.
     returns 0 if not found. */
  /* this could be binary search XXX */
  long            i, high, low, tmp;

#ifdef PARTIALWORD
  long            wordlen = strlen (word);

#endif /* PARTIALWORD */

  low = 0;
  high = block_length;
  i = (low + high) / 2;
  while (low != high) {
    long            compare;
    char           *dictionary_word = dictionary_block_word (i, block);

    /* 
     * printf("dw = %s, w = %s, low = %d, i = %d, hi = %d\n",
     * dictionary_word,word,low,i,high);
     */
    if (dictionary_word[0] == '\0') {
      if (high != i) {
	high = i;
	i = (low + i) / 2;
      } else {
	*position = i - 1;
	return (-dictionary_block_position (i - 1, block));
      }
    } else {
#ifdef PARTIALWORD
      if (findpart) {
	compare = strncmp (dictionary_word, word, wordlen);
        /*
        if ((0 == compare) && (i < high)) {
            if (!strncmp (dictionary_block_word (i+1, block), word, wordlen))
                compare = -1;
        }
        */
	if (0 == compare) {
	  int             ati = i;

	  /* save partword matches for later... */
	  /* save the initial match too */
	  savePartMatch (dictionary_block_position (i, block),
			 dictionary_block_word_occurances (i, block),
			 dictionary_word);
	  while (i > low && 0 == compare) {
	    --i;
	    dictionary_word = dictionary_block_word (i, block);
	    compare = strncmp (dictionary_word, word, wordlen);
	    if (0 == compare)
	      savePartMatch (dictionary_block_position (i, block),
			     dictionary_block_word_occurances (i, block),
			     dictionary_word);
	  }
	  i = ati;
	  compare = 0;
	  while (i < high && 0 == compare) {
	    ++i;
	    dictionary_word = dictionary_block_word (i, block);
	    compare = strncmp (dictionary_word, word, wordlen);
	    if (0 == compare)
	      savePartMatch (dictionary_block_position (i, block),
			     dictionary_block_word_occurances (i, block),
			     dictionary_word);
	  }
	  *position = ati;
	  return (dictionary_block_position (ati, block));
	}
      } else {
	if (findpart_init)
	  compare = strncmp (dictionary_word, word, wordlen);
	else {
#ifdef FIELDS
	  long            dctwrd_len = strlen (dictionary_word);

	  if (!strcmp (word, DICTIONARY_TOTAL_SIZE_WORD) &&
	      !strncmp (dictionary_word, DICTIONARY_TOTAL_SIZE_WORD, strlen (DICTIONARY_TOTAL_SIZE_WORD)) &&
	      (
#ifdef STEM_WORDS
		(dictionary_word[dctwrd_len - 1] == STEMMING_TAG) ||
#endif /* STEM_WORDS */
		(dictionary_word[dctwrd_len - 1] == NUMERIC_TAG))) {
	    if (dictionary_word[dctwrd_len - 1] == STEMMING_TAG)
	      stemming = true;
	    else
	      gnumeric = true;
	    dictionary_word[dctwrd_len - 1] = '\0';
	  }
#endif /* FIELDS */
	  compare = strcmp (dictionary_word, word);
	}
      }
#else /* not PARTIALWORD */
      compare = strcmp (dictionary_word, word);
#endif /* not PARTIALWORD */

      if (0 == compare) {
	dictionary_last_word_occurances =
	  dictionary_block_word_occurances (i, block);
#ifdef PARTIALWORD
	if (findpart_init) {
	  long            prev_i = i;

	  if (i > 0)
	    saveDctBlPsSet (dictionary_block_position (i - 1, block));
	  saveDctBlPsSet (dictionary_block_position (i, block));
	  i++;
	  dictionary_word = dictionary_block_word (i, block);
	  if (strncmp (dictionary_word, word, wordlen)) {
	    *position = prev_i;
	    return (dictionary_block_position (prev_i, block));
	  }
	  while (i <= block_length) {
	    dictionary_word = dictionary_block_word (i, block);
	    if (!strncmp (dictionary_word, word, wordlen))
	      saveDctBlPsSet (dictionary_block_position (i, block));
	    ++i;
	  }
	  *position = prev_i;
	  return (dictionary_block_position (prev_i, block));
	} else {
	  *position = i;
	  return (dictionary_block_position (i, block));
	}
#else /* not PARTIALWORD */
	*position = i;
	return (dictionary_block_position (i, block));
#endif /* not PARTIALWORD */
      }
      if (compare > 0) {
	if (high != i) {
	  high = i;
	  i = (low + i) / 2;
	} else {
	  *position = i - 1;
	  return (-dictionary_block_position (i - 1, block));
	}
      } else {
	if (low != i) {
	  low = i;
	  i = (0.5 + high + i) / 2;
	} else {
	  *position = i;
	  return (-dictionary_block_position (i, block));
	}
      }
    }
  }
  if (i == 0) {
    *position = 0;
    return (0);
  } else {
    *position = i - 1;
    return (-dictionary_block_position (i - 1, block));
  }
}



unsigned char *read_dictionary_block(block,position,length,stream)
unsigned char *block;
long position;
long length;
FILE *stream;
/* reads the dictionary block from the disk and returns it.
   block is the place to put it, if it is NULL, then it is malloc'ed.
   position is the position in the dictionary file to start reading.
   length is th enumber of entries (not bytes) in the block.
   stream is the dictionary stream.
   
   it returns NULL if it loses.
 */
	
{
  static long last_position = -1;
  static unsigned char* last_block = NULL;
  static FILE* last_dict_file = NULL; /* there may be more than one dict */

  if (stream != last_dict_file)
   { /* invalidate the cache */
     last_position = -1;
     last_dict_file = stream;
   }
  
  if(NULL == block) {
    block = (unsigned char *)s_malloc((size_t)(length*DICTIONARY_ENTRY_SIZE));
    last_block = NULL;          /* we may get the last_block from malloc!!! */
  }
  if ((block != last_block) || 
      (position != last_position)) {
    last_position = position;
    last_block = block;
    s_fseek(stream, position, SEEK_SET);
    if(0 > fread_from_stream(stream, block, (length * DICTIONARY_ENTRY_SIZE))){
      waislog(WLOG_HIGH, WLOG_ERROR,
	      "Could not read the dictionary block %ld, length %ld",
	      block, length);
      return(NULL);
    }
  }
  return(block);
}

#ifdef PARTIALWORD
#ifdef FIELDS
long 
look_up_partialword_in_dictionary (field_name, word, number_of_occurances, match_word, db)
#else /* not FIELDS */
long 
look_up_partialword_in_dictionary (word, number_of_occurances, db)
#endif					    /* not FIELDS */
#ifdef FIELDS
     char           *field_name;
     char          **match_word;
#endif /* FIELDS */
     char           *word;
     long           *number_of_occurances;
     database       *db;
{
  long            answer;
  boolean         findpart = false;

  if (word != NULL) {
    clearPartMatch ();
#ifdef FIELDS
    clearDctBlPsSet ();

    if (field_name[0] == '\0')
      answer = look_up_word_in_dictionary (word, number_of_occurances, db);
    else
      answer = field_look_up_word_in_dictionary (field_name, word,
						 number_of_occurances, db);
#else /* not FIELDS */
    answer = look_up_word_in_dictionary (word, number_of_occurances, db);
#endif /* not FIELDS */
    if (answer > 0) {
        if (gSavepart != NULL && match_word != NULL) {
            *match_word = gSavepart[gAtpart].word;
            gAtpart++;
        }
        return (answer);		/* got a match */
    }
  }
#ifdef FIELDS
  if (gnumeric) {
    if (min_number_of_blocks > 0) {
      answer = handle_numeric (min_number_of_blocks, number_of_occurances,
			       field_name, field_dictionary_block, db);
      min_number_of_blocks = 0;
      gnumeric = false;
    }
  }
#endif /* FIELDS */

  {
    if (gAtpart >= gNpart) {
      clearPartMatch ();
      return (-1);
    } else {
      answer = gSavepart[gAtpart].blocknum;
      /*
         fprintf(stderr, "look_up_partialword_in_dictionary:: %s\n", 
         gSavepart[gAtpart].word);
       */
      if (match_word != NULL) {
	*match_word = gSavepart[gAtpart].word;
      }
      if (NULL != number_of_occurances) {
	if (answer > 0)
	  *number_of_occurances = gSavepart[gAtpart].wordcount;
	else
	  *number_of_occurances = 0;
      }
      gAtpart++;
      return (answer);
    }
  }
}
#endif /* PARTIALWORD */
#ifdef PARTIALWORD
static void check_partialword _AP((char* word, boolean* findpart));
    static void check_partialword(word, findpart)
     char* word;
     boolean* findpart;
{
  int l = strlen(word) - 1;
  if (l > 0 && word[l] == PARTWORD_WILDCARD) {
    *findpart= true;
    word[l]= '\0';
  }
}
#endif /* PARTIALWORD */

long 
look_up_word_in_dictionary(word, number_of_occurances, db)
char *word;
long *number_of_occurances;
database* db;
/* looks up the word in the dictionary file. Returns the pointer
   into the inverted file or negative number if not found, 
   or 0 if error.
    It sets number_of_occurances (if it is not NULL) to the number
      registered in the file.  This is used during searching.  
      It is set to 0 if error or word not found.
      If it is NULL, then it is not touched.
 */
{
  long position,prev_position = 0;
  long answer,prev_answer;
  FILE *stream = db->dictionary_stream;
  long dictionary_block_pos;
  boolean findpart = false; /* dgg, PARTIALWORD flag */

#ifdef PARTIALWORD
  { check_partialword(word, &findpart); }
#endif /* PARTIALWORD */
  
  if(NULL == dictionary_header_block)
   {
     s_fseek(stream, 0L, SEEK_SET);
     number_of_dictionary_blocks = read_bytes(DICTIONARY_HEADER_SIZE,stream);
     dictionary_header_block =
       read_dictionary_block(dictionary_header_block,DICTIONARY_HEADER_SIZE,
			     number_of_dictionary_blocks,stream);
     if(NULL == dictionary_header_block)
      {	waislog(WLOG_HIGH, WLOG_ERROR,
		"Could not read dictionary header block in db %s.",
		db->database_file);
	return(0);
      }
   }
  
#ifdef PARTIALWORD
  if(findpart)
    dictionary_block_pos = 
      find_pointer_in_block(word,
                            dictionary_header_block,
                            number_of_dictionary_blocks,
                            &position, false, true);
  else
#endif /* PARTIALWORD */
    dictionary_block_pos = 
      find_pointer_in_block(word,
                            dictionary_header_block,
                            number_of_dictionary_blocks,
                            &position, false, false);
  
  if(0 == dictionary_block_pos)
   { /* waislog(WLOG_HIGH, WLOG_ERROR, "Could not find pointer for word '%s' (location %ld) in block in db %s!",
	     word, word, db->database_file); */
     return(-1);  /* not an error, necessarily if the word is before the first entry */
   }
  
#ifdef PARTIALWORD
  if(findpart && min_number_of_blocks) { /* tung, 3/94 */
    while(min_number_of_blocks > 0) {
      dictionary_block_pos = dictionary_block_pos_set[min_number_of_blocks-1];
      dictionary_block = 
        read_dictionary_block(dictionary_block,ABSOLUTE(dictionary_block_pos),
                              DICTIONARY_BLOCK_SIZE,stream);
      if(NULL == dictionary_block)
        { waislog(WLOG_HIGH, WLOG_ERROR,
                  "Could not read dictionary block %ld in db %s",
                  ABSOLUTE(dictionary_block_pos),
                  db->database_file);
          return(0);
        }
      answer = find_pointer_in_block(word, dictionary_block, 
                                     DICTIONARY_BLOCK_SIZE, 
                                     &position, findpart,false);
      if(prev_position == 0) {
        prev_position = position;
        prev_answer = answer;
      }
      --min_number_of_blocks;
    }
  }
  else {
#endif /* PARTIALWORD */
    dictionary_block = 
      read_dictionary_block(dictionary_block,ABSOLUTE(dictionary_block_pos),
                            DICTIONARY_BLOCK_SIZE,stream);
    if(NULL == dictionary_block)
      { waislog(WLOG_HIGH, WLOG_ERROR,
                "Could not read dictionary block %ld in db %s",
                ABSOLUTE(dictionary_block_pos),
                db->database_file);
        return(0);
      }
    answer = find_pointer_in_block(word, dictionary_block, 
                                   DICTIONARY_BLOCK_SIZE, 
                                   &position, findpart,false);
#ifdef PARTIALWORD
  }
#endif /* PARTIALWORD */
  
#ifdef PARTIALWORD
  if(findpart && prev_position) { /* tung, 3/94 */
    if((NULL != number_of_occurances)) {
      if (prev_answer > 0)
        *number_of_occurances = 
          dictionary_block_word_occurances(prev_position, dictionary_block);
      else
        *number_of_occurances = 0;
    }
    answer = prev_answer;
  }
  else
#endif /* PARTIALWORD */
  if((NULL != number_of_occurances)) {
    if (answer > 0)
      *number_of_occurances = 
        dictionary_block_word_occurances(position, dictionary_block);
    else
      *number_of_occurances = 0;
  }
  
  return(answer);
}

#ifdef FIELDS
long 
field_look_up_word_in_dictionary(field_name, word, number_of_occurances, db)
char* field_name;
char *word;
long *number_of_occurances;
database* db;
/* looks up the word in the dictionary file. Returns the pointer
   into the inverted file or negative number if not found, 
   or 0 if error.
    It sets number_of_occurances (if it is not NULL) to the number
      registered in the file.  This is used during searching.  
      It is set to 0 if error or word not found.
      If it is NULL, then it is not touched.
 */
{
  long field_id;
  long position, prev_position = 0;
  long answer, prev_answer;
  FILE *stream; /* = db->dictionary_stream; */
  long dictionary_block_pos;
  boolean findpart = false; /* dgg, PARTIALWORD flag */
  boolean numeric = false;

  field_id = pick_up_field_id(field_name, db);

  if(field_id == -1) {
    *number_of_occurances = 0;
    return(-1);
  }
  stream = db->field_dictionary_streams[field_id];

#ifdef PARTIALWORD
  { check_partialword(word, &findpart); }
#endif /* PARTIALWORD */

  numeric = check_numeric(word, db->fields[field_id].numeric);
    
  field_dictionary_header_block = NULL;
  if(NULL == field_dictionary_header_block)
   {
     s_fseek(stream, 0L, SEEK_SET);
     field_number_of_dictionary_blocks = read_bytes(DICTIONARY_HEADER_SIZE,stream);
     field_dictionary_header_block =
       read_dictionary_block(field_dictionary_header_block,
                             DICTIONARY_HEADER_SIZE,
			     field_number_of_dictionary_blocks,stream);
     if(NULL == field_dictionary_header_block)
      {	waislog(WLOG_HIGH, WLOG_ERROR,
		"Could not read dictionary header block of field %s in db %s.",
		field_name, db->database_file);
        s_free(field_dictionary_header_block);
	return(0);
      }
   }

  if(numeric) { /* word is a number */
    dictionary_block_pos = 
      numeric_find_pointer_in_block(word,
                                    field_dictionary_header_block,
                                    field_number_of_dictionary_blocks,
                                    &position, false, numeric, &gnumeric_tag);
    if(gnumeric_tag != EQ) {
      s_free(field_dictionary_header_block);
      return(-1);
    }
  }
  else
#ifdef PARTIALWORD
    if(findpart)   /* word is a partial word */
      dictionary_block_pos = 
        find_pointer_in_block(word,
                              field_dictionary_header_block,
                              field_number_of_dictionary_blocks,
                              &position, false,true);
    else
#endif /* PARTIALWORD */
      { /* if a field is a numeric field and 
	   the search word is DICTIONARY_TOTAL_SIZE_WORD (search for block)*/

        if(db->fields[field_id].numeric)
          dictionary_block_pos = 
            numeric_find_pointer_in_block(word,
                                          field_dictionary_header_block,
                                          field_number_of_dictionary_blocks,
                                          &position, true, numeric, 
                                          &gnumeric_tag);
        else {
          dictionary_block_pos = 
            find_pointer_in_block(word,
                                  field_dictionary_header_block,
                                  field_number_of_dictionary_blocks,
                                  &position, false, false);
	}
      }
  s_free(field_dictionary_header_block);
  if(0 == dictionary_block_pos)
    { /* waislog(WLOG_HIGH, WLOG_ERROR, "Could not find pointer for word '%s' (location %ld) in block in db %s!",
         word, word, db->database_file); */
      return(-1);  /* not an error, necessarily if the word is before the first entry */
    }

#ifdef PARTIALWORD
  if(findpart && min_number_of_blocks) { /* tung, 3/94 */
    while(min_number_of_blocks > 0) {
      dictionary_block_pos = dictionary_block_pos_set[min_number_of_blocks-1];
      dictionary_block = 
        read_dictionary_block(dictionary_block,ABSOLUTE(dictionary_block_pos),
                              DICTIONARY_BLOCK_SIZE,stream);
      if(NULL == dictionary_block)
        { waislog(WLOG_HIGH, WLOG_ERROR,
                  "Could not read dictionary block %ld in db %s",
                  ABSOLUTE(dictionary_block_pos),
                  db->database_file);
          return(0);
        }
      answer = find_pointer_in_block(word, dictionary_block, 
                                     DICTIONARY_BLOCK_SIZE, 
                                     &position, findpart,false);
      if(prev_position == 0) {
        prev_position = position;
        prev_answer = answer;
      }
      --min_number_of_blocks;
    }
  }
  else {
#endif /* PARTIALWORD */
    field_dictionary_block = 
      read_dictionary_block(field_dictionary_block,ABSOLUTE(dictionary_block_pos),
                            DICTIONARY_BLOCK_SIZE,stream);
    if(NULL == field_dictionary_block)
      { waislog(WLOG_HIGH, WLOG_ERROR,
                "Could not read dictionary block %ld of field in db %s",
                ABSOLUTE(dictionary_block_pos),
                field_name, db->database_file);
        return(0);
      }
#ifdef FIELDS
    if(numeric) 
      answer = numeric_find_pointer_in_block(word,
                                             field_dictionary_block,
                                             DICTIONARY_BLOCK_SIZE,
                                             &position, false, numeric, 
                                             &gnumeric_tag);
    else {
      /* if a field is a numeric field and 
	 the search word is DICTIONARY_TOTAL_SIZE_WORD (search for word) */
      if(db->fields[field_id].numeric) 
        answer = numeric_find_pointer_in_block(word,
                                               field_dictionary_block,
                                               DICTIONARY_BLOCK_SIZE,
                                               &position, false, numeric, 
                                               &gnumeric_tag);
      else
#endif /* FIELDS */
	{
	  boolean tmp_numeric = gnumeric;
	  answer = find_pointer_in_block(word, field_dictionary_block, 
					 DICTIONARY_BLOCK_SIZE, &position,
					 findpart,false);
	  if(gnumeric) {
	    db->fields[field_id].numeric = true;
	    gnumeric = tmp_numeric;
	  }
	}
#ifdef PARTIALWORD
    }
#endif /* PARTIALWORD */
#ifdef FIELDS
  }
#endif /* FIELDS */

#ifdef PARTIALWORD
  if(findpart && prev_position) { /* tung, 3/94 */
    if((NULL != number_of_occurances)) {
      if (prev_answer > 0)
        *number_of_occurances = 
          dictionary_block_word_occurances(prev_position, dictionary_block);
      else
        *number_of_occurances = 0;
    }
    answer = prev_answer;
  }
  else
#endif /* PARTIALWORD */
    if((NULL != number_of_occurances)) {
      if (answer > 0)
        *number_of_occurances = 
          dictionary_block_word_occurances(position, field_dictionary_block);
      else
        *number_of_occurances = 0;
    }
#ifdef STEM_WORDS
  if(stemming && db->fields[field_id].stemming == false) {
    db->fields[field_id].stemming = stemming;
    stemming = false;
  }
#endif /* STEM_WORDS */
  return(answer);
}
#endif /* FIELDS */

/*  BUILDING DICTIONARY FILES */


long number_of_dictionary_entries; /* number allocated */

char *block_of_zeros = NULL;

static void write_zeros_to_stream _AP((long n_bytes,FILE* stream));

static void write_zeros_to_stream(n_bytes,stream)
long n_bytes;
FILE *stream;
/* writes zeros to a file quickly */
{	
  long i;
  if(n_bytes >= BLOCK_SIZE){	/* then write big blocks of zeros */
    if(NULL == block_of_zeros){
      block_of_zeros = (char*)s_malloc((size_t)BLOCK_SIZE);
      memset(block_of_zeros, 0, BLOCK_SIZE);
    }
    while(n_bytes >= BLOCK_SIZE){	
      /* then write big blocks of zeros */
      if(BLOCK_SIZE != 
	 fwrite(block_of_zeros, sizeof(char), BLOCK_SIZE, stream))
	panic("Write failed");
      n_bytes -= BLOCK_SIZE;
    }
  }
  for(i = 0; i < n_bytes; i++){	/* clean up the rest */
    putc('\0', stream); 
  }
}	

/* returns 0 if successful */
long init_dict_file_for_writing(db)
database *db;
{
  char filename[MAX_FILENAME_LEN];

  if (db->dictionary_stream != NULL)
    fclose(db->dictionary_stream);
  db->dictionary_stream = 
    s_fopen(temp_dictionary_filename(filename, db), "w+b");

  db->total_word_count = 0;
  init_dict_file_detailed(db->dictionary_stream,db->number_of_words);
  return(0);
}

#ifdef FIELDS
/* returns 0 if successful */
long field_init_dict_file_for_writing(field_id, db)
long field_id;
database *db;
{
  char filename[MAX_FILENAME_LEN];

  if (*(db->field_dictionary_streams) != NULL)
    fclose(*(db->field_dictionary_streams));
  *(db->field_dictionary_streams) = 
    s_fopen(field_temp_dictionary_filename(filename, field_id, db), "w+b");

  db->fields[field_id].total_word_count = 0; 
  init_dict_file_detailed(*(db->field_dictionary_streams),
			  db->fields[field_id].number_of_words);
  return(0);
}
#endif /* FIELDS */

static long dict_number_of_blocks _AP((long number_of_words));

static long
dict_number_of_blocks(number_of_words)
long number_of_words;
{
  long number_of_blocks;
  number_of_blocks = (number_of_words / DICTIONARY_BLOCK_SIZE) +
    ((0 == (number_of_words % DICTIONARY_BLOCK_SIZE)) ? 0 : 1);
  /*
   *  fprintf(stderr, "dict_number_of_blocks %ld->%ld\n", number_of_words,
   *          number_of_blocks);
   */

  return(number_of_blocks);
}

void
record_num_blocks_in_dict(dictionary_stream,number_of_words)
FILE* dictionary_stream;
long number_of_words;
{ /* write the number of blocks */
  s_fseek(dictionary_stream, 0L, SEEK_SET);
  write_bytes(dict_number_of_blocks(number_of_words),
	      DICTIONARY_HEADER_SIZE, dictionary_stream);
  fseek(dictionary_stream, 0L, SEEK_END);
}

void
init_dict_file_detailed(dictionary_stream,number_of_words)
FILE* dictionary_stream;
long number_of_words;
{
  /* create space for the table in the front of the file */
  write_zeros_to_stream(DICTIONARY_HEADER_SIZE + 
			(DICTIONARY_ENTRY_SIZE * 
			 dict_number_of_blocks(number_of_words)),
			dictionary_stream);
  record_num_blocks_in_dict(dictionary_stream,number_of_words);
  number_of_dictionary_entries = 0;
}

/* this must be called in alphabetical order, and writes the word to
   the dictionary file. */	
long add_word_to_dictionary(word,position,number_of_occurances,db)
char *word;
long position;
long number_of_occurances;
database *db;
     /* Puts a word into the dictionary file. */
{
  /* assumes the stream has been initialized, and it is positioned
     at the end */
  FILE *stream = db->dictionary_stream;
  char padded_word[MAX_WORD_LENGTH + 1];

  memset(padded_word, 0, MAX_WORD_LENGTH + 1); /* clear the word */
  strcpy(padded_word, word);
#ifdef STEM_WORDS
  if(strcmp(word, DICTIONARY_TOTAL_SIZE_WORD) == 0) {
    if(db->stemming) 
      padded_word[strlen(padded_word)] = STEMMING_TAG;
  }
#endif /* STEM_WORDS */
  
  if(0 == (number_of_dictionary_entries % DICTIONARY_BLOCK_SIZE)){
    /* then add an entry in the header */
    long original_position = s_ftell(stream);
    long header_entry = number_of_dictionary_entries / DICTIONARY_BLOCK_SIZE; 
    /* printf("Adding header entry %ld %s original pos %ld\n", 
       header_entry, padded_word, original_position); */
    fseek(stream, DICTIONARY_HEADER_SIZE + 
	  (header_entry * DICTIONARY_ENTRY_SIZE), SEEK_SET);
    if((MAX_WORD_LENGTH + 1) != 
       fwrite(padded_word, sizeof(char), MAX_WORD_LENGTH + 1, stream))
      panic("Write failed");
    write_bytes(original_position, NEXT_INDEX_BLOCK_SIZE, stream);
    write_bytes(0L, NUMBER_OF_OCCURANCES_SIZE, stream);
    fseek(stream, original_position, SEEK_SET); /* go back to the end */
    /* zero the next block */
    write_zeros_to_stream(DICTIONARY_ENTRY_SIZE * DICTIONARY_BLOCK_SIZE,
			  stream); 
    fseek(stream, original_position, SEEK_SET);      
  }
  /* write the word */	
  if((MAX_WORD_LENGTH + 1) !=
     fwrite(padded_word, sizeof(char), MAX_WORD_LENGTH + 1, stream))
    panic("Write failed");
  write_bytes(position, NEXT_INDEX_BLOCK_SIZE, stream);
  write_bytes(number_of_occurances, NUMBER_OF_OCCURANCES_SIZE, stream);
  number_of_dictionary_entries++;	
  db->total_word_count += number_of_occurances;
  return(0);
}

#ifdef FIELDS
/* this must be called in alphabetical order, and writes the word to
   the dictionary file. */	
long field_add_word_to_dictionary(word, position,
                                  number_of_occurances, field_id, db)
char *word;
long position;
long number_of_occurances;
long field_id;
database *db;
     /* Puts a word into the dictionary file. */
{
  /* assumes the stream has been initialized, and it is positioned
     at the end */
  FILE *stream = *(db->field_dictionary_streams);
  char padded_word[MAX_WORD_LENGTH + 1];

  memset(padded_word, 0, MAX_WORD_LENGTH + 1); /* clear the word */
  strcpy(padded_word, word);
  
  if(strcmp(word, DICTIONARY_TOTAL_SIZE_WORD) == 0)
    {
#ifdef STEM_WORDS
      if(db->fields[field_id].stemming) 
	padded_word[strlen(padded_word)] = STEMMING_TAG;
      else
#endif /* STEM_WORDS */
	if(db->fields[field_id].numeric)
	  padded_word[strlen(padded_word)] = NUMERIC_TAG; 
    }
  if(0 == (number_of_dictionary_entries % DICTIONARY_BLOCK_SIZE)){
    /* then add an entry in the header */
    long original_position = s_ftell(stream);
    long header_entry = 
      number_of_dictionary_entries / DICTIONARY_BLOCK_SIZE; 
    /* printf("Adding header entry %ld %s original pos %ld\n", 
       header_entry, padded_word, original_position); */
    fseek(stream, DICTIONARY_HEADER_SIZE + 
	  (header_entry * DICTIONARY_ENTRY_SIZE), SEEK_SET);
    if((MAX_WORD_LENGTH + 1) != 
       fwrite(padded_word, sizeof(char), MAX_WORD_LENGTH + 1, stream))
      panic("Write failed");
    write_bytes(original_position, NEXT_INDEX_BLOCK_SIZE, stream);
    write_bytes(0L, NUMBER_OF_OCCURANCES_SIZE, stream);
    fseek(stream, original_position, SEEK_SET); /* go back to the end */
    /* zero the next block */
    write_zeros_to_stream(DICTIONARY_ENTRY_SIZE * DICTIONARY_BLOCK_SIZE,
			  stream); 
    fseek(stream, original_position, SEEK_SET);      
  }
  /* write the word */	
  if((MAX_WORD_LENGTH + 1) !=
     fwrite(padded_word, sizeof(char), MAX_WORD_LENGTH + 1, stream))
    panic("Write failed");
  write_bytes(position, NEXT_INDEX_BLOCK_SIZE, stream);
  write_bytes(number_of_occurances, NUMBER_OF_OCCURANCES_SIZE, stream);
  number_of_dictionary_entries++; 
  db->fields[field_id].total_word_count += number_of_occurances; 
  return(0);
}
#endif /* FIELDS */

/* this is called after all add_words are done, but before the file 
   is closed. Returns 0 if successful. */
long
finished_add_word_to_dictionary(db)
     database* db;
{
  char temp_filename[MAX_FILENAME_LEN];
  char filename[MAX_FILENAME_LEN];

  waislog(WLOG_LOW, WLOG_INFO, "Total word count for dictionary is: %ld",
	  db->total_word_count);
  if(0 != add_word_to_dictionary(DICTIONARY_TOTAL_SIZE_WORD, 
				1, db->total_word_count, db))
    return(-1);

  db->number_of_words++;
  record_num_blocks_in_dict(db->dictionary_stream,db->number_of_words);

  fflush(db->dictionary_stream); /* so that any new opens will see a 
				    valid file */

  /* rename the .dcttmp file to dct */
  temp_dictionary_filename(temp_filename, db);
  dictionary_filename(filename, db);
  /* printf("renaming %s to %s\n", temp_filename, filename); */
  if(0 != rename(temp_filename, filename))
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "could not rename file %s to %s",
	    temp_filename, filename);
  return(0);
}

#ifdef FIELDS
long
field_finished_add_word_to_dictionary(field_id, db)
     long field_id;
     database* db;
{
  char temp_filename[MAX_FILENAME_LEN];
  char filename[MAX_FILENAME_LEN];

  waislog(WLOG_LOW, WLOG_INFO, "Total word count for dictionary of field %s is: %ld",
	  db->fields[field_id].field_name, db->fields[field_id].total_word_count);
  if(0 != field_add_word_to_dictionary(DICTIONARY_TOTAL_SIZE_WORD, 
                                       1,db->fields[field_id].total_word_count,
                                       field_id, db))
    return(-1);
  
  db->fields[field_id].number_of_words++;
  record_num_blocks_in_dict(*(db->field_dictionary_streams),
                            db->fields[field_id].number_of_words);

  fflush(*(db->field_dictionary_streams)); /* so that any new opens 
                                                     will see a valid file */
  
  /* rename the .dcttmp file to dct */
  field_temp_dictionary_filename(temp_filename, field_id, db);
  field_dictionary_filename(filename, db->fields[field_id].field_name, db);
  /* printf("renaming %s to %s\n", temp_filename, filename); */
  if(0 != rename(temp_filename, filename))
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "could not rename file %s to %s",
	    temp_filename, filename);
  return(0);
}
#endif /* FIELDS */

void 
print_dictionary_block (block, size)
     unsigned char  *block;
     long            size;

/* this prints the contents of a dictionary block */
{
  long            i;

  for (i = 0; i < size; i++) {
    char           *word = dictionary_block_word (i, block);

    if (word[0] == '\0')
      break;
    /* I assume this is only for debugging - JG */
    printf ("Entry %3ld: %21s %7ld %7ld\n", i, word,
	    dictionary_block_position (i, block),
	    dictionary_block_word_occurances (i, block));
  }
}

void print_dictionary _AP ((database * db));

void 
print_dictionary (db)
     database       *db;
{
  /* prints the contents of a dictionary */
  FILE           *stream = db->dictionary_stream;
  long            i;
  long            new_number_of_dictionary_blocks;

  if (NULL == stream)
    panic ("dictionary stream is not open");
  s_fseek (stream, 0L, SEEK_SET);
  new_number_of_dictionary_blocks = read_bytes (DICTIONARY_HEADER_SIZE, stream);
  if (new_number_of_dictionary_blocks > number_of_dictionary_blocks)
    dictionary_header_block = NULL;
  number_of_dictionary_blocks = new_number_of_dictionary_blocks;
  printf ("Number of dictionary blocks %ld\n", number_of_dictionary_blocks);
  if (NULL == (dictionary_header_block =
	       read_dictionary_block (dictionary_header_block,
				      DICTIONARY_HEADER_SIZE,
				      number_of_dictionary_blocks,
				      stream)))
    panic ("Could not read dictionary header block");
  printf ("The Dictionary Header Block:\n");
  print_dictionary_block (dictionary_header_block, number_of_dictionary_blocks);
  for (i = 0; i < number_of_dictionary_blocks; i++) {
    long            pos = dictionary_block_position (i, dictionary_header_block);

    if (NULL == (dictionary_block =
		 read_dictionary_block (dictionary_block,
					pos, DICTIONARY_BLOCK_SIZE, stream)))
      panic ("Could not read dictionary block %ld", pos);
    printf ("\n\nDictionary block %ld (position %ld):\n", i, pos);
    print_dictionary_block (dictionary_block, DICTIONARY_BLOCK_SIZE);
  }
  fseek (stream, 0L, SEEK_END);
}

#ifdef testing
/* dictionary testing code */

static void check_dictionary_entry _AP((char* word,long expected_position,
					database* db));

static void check_dictionary_entry(word,expected_position,db)
char *word;
long expected_position;
database *db;
{
  if(expected_position != look_up_word_in_dictionary(word, NULL, db)) {
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "%s should be %ld is %ld in db %s", 
	    word, expected_position, 
	    look_up_word_in_dictionary(word, NULL, db),
	    db->database_file);
  }
}
  
static void test_dictionary _AP((database* db));

static void test_dictionary(db)
database *db;
/* this is just an trivial test */
{

  db->number_of_words = 3;
  init_dict_file_for_writing(db);
  add_word_to_dictionary("aardvark", 123L, 0l, db);
  add_word_to_dictionary("house", 234L, 0L, db);
  add_word_to_dictionary("mary", 345L, 0L, db);
  fflush(db->dictionary_stream);
  print_dictionary(db);
  check_dictionary_entry("aardvark", 123L, db);
  check_dictionary_entry("house", 234L, db);
  check_dictionary_entry("mary", 345L, db);
  check_dictionary_entry("food", -123L, db);
  check_dictionary_entry("zebra", -345L, db);
  check_dictionary_entry("aaarf", 0L, db);
}
#endif /* testing */


/*========================*
 *===  Document Table  ===*
 *========================*/

boolean
read_document_table_entry(doc_entry,number,db)
document_table_entry* doc_entry;
long number;
database* db;
/* returns a document_table_entry on the stack */
{
  long position;
  FILE *stream = db->document_table_stream;
	
  position = (DOC_TAB_HEADER_SIZE + 
	      ((long)number * (long)DOC_TAB_ELEMENT_SIZE));

  if (0 != fseek(stream, position, SEEK_SET))
	{ 
	  waislog(WLOG_HIGH, WLOG_ERROR,
		  "fseek failed into the document table to position %ld in db %s", 
		  position,
		  db->database_file);
	  return(false);
	}
	
  doc_entry->filename_id = read_bytes(DOC_TAB_ENTRY_FILENAME_ID_SIZE, 
				     stream);
  doc_entry->headline_id = read_bytes(DOC_TAB_ENTRY_HEADLINE_ID_SIZE, 
				     stream);	
  doc_entry->start_character = 
    read_bytes(DOC_TAB_ENTRY_START_CHAR_SIZE, stream);
  doc_entry->end_character = 
    read_bytes(DOC_TAB_ENTRY_END_CHAR_SIZE, stream);
  doc_entry->document_length = 
    read_bytes(DOC_TAB_ENTRY_DOC_LENGTH_SIZE, stream);
  doc_entry->number_of_lines = 
    read_bytes(DOC_TAB_ENTRY_NUM_LINES_SIZE, stream);
  doc_entry->date = 
    read_bytes(DOC_TAB_ENTRY_DATE_SIZE, stream);
  if (doc_entry->date == EOF) { 
    return(false);
  }

/*printf("read_document_table_entry pos %ld val %lx\n",position,doc_entry->date);*/

  return(true);
}


boolean
writeUserValToDocIDTable(userVal,doc,db)
unsigned long userVal;
long doc;
database* db;
/* the docIDTable needs to keep a user value for use by other indexing
   systems.  Currently it is stuffed in the date field. 

   This routine needs to be updated if read_document_table_entry changes
 */
{
  long position;
  
  position = (DOC_TAB_HEADER_SIZE +
	      ((long)doc * (long)DOC_TAB_ELEMENT_SIZE) +
	      DOC_TAB_ENTRY_FILENAME_ID_SIZE +
	      DOC_TAB_ENTRY_HEADLINE_ID_SIZE + 
	      DOC_TAB_ENTRY_START_CHAR_SIZE +
	      DOC_TAB_ENTRY_END_CHAR_SIZE +
	      DOC_TAB_ENTRY_DOC_LENGTH_SIZE +
	      DOC_TAB_ENTRY_NUM_LINES_SIZE);

  if (0 != fseek(db->document_table_stream,position,SEEK_SET))
   { waislog(WLOG_HIGH, WLOG_ERROR,
	     "fseek failed into the document table to position %ld in db %s", 
	     position,db->database_file);
     return(false);
   }

/*printf("writeUserValToDocIDTable pos %ld val %lx\n",position,userVal);*/

  write_bytes(userVal,DOC_TAB_ENTRY_DATE_SIZE,db->document_table_stream);
  fflush(db->document_table_stream);
  return(true);
}



#ifdef testing

static  boolean check_document_id _AP((long doc_id,database* db));

static  boolean
check_document_id(doc_id,db)
long doc_id;
database* db;
/* returns true if that is a valid doc_id (corresponds to a file
   that has not been deleted */
{
  long position;
  FILE *stream = db->document_table_stream;
  long filename_id;
  char filename[MAX_FILE_NAME_LEN];

  position = (DOC_TAB_HEADER_SIZE + 
	      ((long)doc_id * (long)DOC_TAB_ELEMENT_SIZE));

  if (0 != fseek(stream, position, SEEK_SET)) { 
    waislog(WLOG_HIGH, WLOG_ERROR,
	    "fseek failed into the document table to position %ld in db %s",
	    position,
	    db->database_file);
    return(false);
  }
	
  filename_id = read_bytes(DOC_TAB_ENTRY_FILENAME_ID_SIZE, stream);
  /* probe the file.  Is there a faster way? */
  return(probe_file_possibly_compressed(read_filename_table_entry(filename_id, filename,NULL,db)));
}
#endif /* testing */

long write_document_table_entry(doc_table_entry, db)
document_table_entry* doc_table_entry;
database* db;
{
  /* returns the document_id */
  s_fseek(db->document_table_stream,
	     (DOC_TAB_HEADER_SIZE +
	      (db->doc_table_allocated_entries *
	       DOC_TAB_ELEMENT_SIZE)),
	     SEEK_SET);
  /* write the pieces */
  write_bytes(doc_table_entry->filename_id,
	      DOC_TAB_ENTRY_FILENAME_ID_SIZE,
	      db->document_table_stream);
  write_bytes(doc_table_entry->headline_id,
	      DOC_TAB_ENTRY_HEADLINE_ID_SIZE,
	      db->document_table_stream);
  write_bytes(doc_table_entry->start_character,
	      DOC_TAB_ENTRY_START_CHAR_SIZE,
	      db->document_table_stream);
  write_bytes(doc_table_entry->end_character,
	      DOC_TAB_ENTRY_END_CHAR_SIZE,
	      db->document_table_stream);
  write_bytes(doc_table_entry->document_length,
	      DOC_TAB_ENTRY_DOC_LENGTH_SIZE,
	      db->document_table_stream);
  /*  printf("Writing %ld lines\n", document_table_entry->number_of_lines); */
  write_bytes(doc_table_entry->number_of_lines,
	      DOC_TAB_ENTRY_NUM_LINES_SIZE,
	      db->document_table_stream);
  write_bytes(doc_table_entry->date,
	      DOC_TAB_ENTRY_DATE_SIZE,
	      db->document_table_stream);
  db->doc_table_allocated_entries++;
  return(db->doc_table_allocated_entries);
}

long next_document_id(db)
database* db;
{
  return(db->doc_table_allocated_entries);
}


/*========================*
 *===  Filename table  ===*
 *========================*/

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif /* not MAXPATHLEN */

static char *read_filename_table_stream _AP((long position, 
					    char* filename,
					    char* type, 
					    time_t* file_write_date,
					    FILE *stream));

static char *read_filename_table_stream(position,filename,type,
					file_write_date, stream)
long position;
char* filename;
char* type;
time_t* file_write_date;
FILE *stream;
{
  /* Returns the filename array after side effecting it,
   *  or NULL if an error.
   * The type of the file is put in the argument "type".  This will
   * not be longer than MAX_FILE_NAME_LEN.
   *
   * if type is NULL then ignore it,
   * if file_write_date is NULL then ignore it,
   * If position is -1, then it does not seek.
   *
   * Leave the file positioned at the start of the next entry.
   */	
  long file_write_date_internal;
  char type_internal[MAX_TYPE_LEN];

  if(NULL == stream)
    return(NULL);

  if(NULL == type)  /* this means we do not care, so set up a dummy */
    type = type_internal;

  filename[0] = '\0';	/* init to the empty string */
  if(NULL != type)
    type[0] = '\0';	/* init to the empty string */

  if(position != -1){
    if (0 != fseek(stream, position, SEEK_SET)){
      waislog(WLOG_HIGH, WLOG_ERROR, "fseek failed into the filename index to position %ld", 
	      position);
      return(NULL);
    }
  }
  if(false == read_string_from_file(stream, filename, MAX_FILE_NAME_LEN)){
    return(NULL);
  }
  else{
    file_write_date_internal = read_bytes(FILE_WRITE_DATE_SIZE, stream);
    if(file_write_date){
      *file_write_date = (time_t)file_write_date_internal;
    }
    if(false == read_string_from_file(stream, type, MAX_TYPE_LEN)){
      return(NULL);
    } 
  }
  return(filename);
}
     
char *read_filename_table_entry(position,filename,type,file_write_date,db)
long position;
char* filename;
char* type;
time_t* file_write_date;
database* db;
{
  /* Returns the filename array after side effecting it,
   *  or NULL if an error.
   * The type of the file is put in the argument "type".  This will
   * not be longer than MAX_FILE_NAME_LEN.
   *
   * if type is NULL then ignore it,
   * if file_write_date is NULL then ignore it,
   * If position is -1, then it does not seek.
   *
   * Leave the file positioned at the start of the next entry.
   */	
  FILE *stream = db->filename_table_stream;
  return(read_filename_table_stream(position,filename,type,
				    file_write_date,stream));
}

long write_filename_table_entry(filename,type,db)
char* filename;
char *type;
database* db;
{
  /* writes the filename (NULL terminated),
     followed by 4 bytes of creation date,
     followed by the file type (NULL terminated),
     Returns the postion of the filename
     */
  long free_position,count,i,j;
  char full_path[MAXPATHLEN];
  char savedFileName[MAX_FILENAME_LEN + 1];
  char* tmp_type = NULL;   /* temporary type */
  char* tmp_type_pointer = NULL;   /* temporary type pointer */

  s_fseek(db->filename_table_stream, 0L, SEEK_END);
  free_position = ftell(db->filename_table_stream);
  /* add the filename to the hashtable not done yet XXX
     (setf (gethash filename *filename_table_hashtable*)
     (file_write_date filename))
     */

  if(FILE_WRITE_DATE_SIZE != sizeof(time_t)){ /* check if these are the same */
    panic("We have a problem with the file_write_date_size\n");
  }

  /* this is a hack, but shold not do any harm except for files really
     starting with "http:" */
  if (strncmp("http:", filename, 5) && strncmp("ftp:", filename, 4)) {
    fprintf(db->filename_table_stream, "%s", truename(filename, full_path));
    fputc(0, db->filename_table_stream);
    write_bytes((long)file_write_date(filename),
                FILE_WRITE_DATE_SIZE, db->filename_table_stream);
  } else {
    fprintf(db->filename_table_stream, "%s", filename);
    fputc(0, db->filename_table_stream);
    write_bytes((long)0,
                FILE_WRITE_DATE_SIZE, db->filename_table_stream);
  }
/*  fwrite(type, sizeof(char), strlen(type) + 1, db->filename_table_stream);*/


/* francois - multitype extensions */
/* 
   Here we just add the document types to the file entry, we need to 
   check to see if each file is there so we probe them.
*/

   if ( strstr(type,",") == NULL ) {
      fprintf(db->filename_table_stream, "%s",type);
      fputc(0,db->filename_table_stream);
   }
   else {
   
	   /* count up the number of document types */
	   count = 1L;
	   for (i = 0L; i < strlen(type); i++){
	      if ( type[i] == ',' )
	         count++;
	   }
 
           /* duplicate the type and save the pointer */
           tmp_type = s_strdup(type);
           tmp_type_pointer = tmp_type;

	  
           /* append types - NULL out the pointer so that strtok can grab the subsequent entries */
	   for (i = 0L; i < count; i++ ) {
	      tmp_type_pointer = s_strdup(strtok(tmp_type_pointer,","));
	      
	      strcpy(savedFileName,filename);
              if ( strcmp(savedFileName+(strlen(savedFileName)-2), ".Z") == 0 ) {
               /* it's a .Z file.  First, remove the suffix or many things get confused. */
                 savedFileName[(strlen(savedFileName)-2)] = 0;
              }
         
              /* strip the current extension, but not the period */
              for ( j = strlen(savedFileName); j >= 0L; j-- ) {
                 if (savedFileName[j] == '.') {
                    savedFileName[j+1] = 0;
                    break;
                 }
              }
         
              /* append the type to the file name */
              strcat(savedFileName,tmp_type_pointer);

      
              if(probe_file_possibly_compressed(savedFileName)) {

                 fprintf(db->filename_table_stream, "%s",tmp_type_pointer);
                 fprintf(db->filename_table_stream, ",");
              }
              
              s_free(tmp_type_pointer);
	      tmp_type_pointer = NULL;
	  
	   }
	   
	   /* release the tmp_type allocations */
	   s_free(tmp_type);
     
           /* terminate the string */
           fputc(0,db->filename_table_stream);

   }




  return(free_position);
}

/* functions to figure out if the file is in the index already */
		
static boolean filename_in_filename_stream _AP((char *filename, char *type, 
						time_t *file_write_date, 
						FILE *stream));
		     
static boolean filename_in_filename_stream(filename, type, 
					   file_write_date, stream)
char *filename;
char *type;
time_t *file_write_date;
FILE *stream;
     /* returns true if it is there (and side effects type and 
	  file_write_date). 
        leaves the stream at the end of the file.
	If type or file_write_date is NULL, then it is a dont care.
	type, if it is an array, should be MAX_FILENAME_LEN long at least.
	*/
{
  /* this is slow because it loops through the whole file every time.
     this might want to be optimized by making a hashtable. */
  char next_filename[MAX_FILENAME_LEN];
  
  s_fseek(stream, FILENAME_TABLE_HEADER_SIZE, SEEK_SET);
  while(!feof(stream)){
    char new_type[MAX_FILENAME_LEN];
    if(NULL == 
       read_filename_table_stream(-1, next_filename, new_type, 
				  file_write_date, stream))
      return(false);
    if(0 == strcmp(next_filename, filename))
      return(true);
  }
}

boolean filename_in_database(filename,type,file_write_date,db)
char *filename;
char *type;
time_t *file_write_date;
database *db;
{
  return(filename_in_filename_stream(filename, type, file_write_date, 
				     db->filename_table_stream));
}

/* this caches the last filename that was found to be in the filename file,
   this way repeated attempts to figure out if a file is there will be fast.
   This is the case when retrieving successive blocks of a file. */   
char last_filename_found_in_file[MAX_FILE_NAME_LEN];
char last_filename_file[MAX_FILE_NAME_LEN];

boolean filename_in_filename_file(filename,type,file_write_date, filename_file)
char *filename;
char *type;
time_t *file_write_date;
char *filename_file;
{
  if(NULL == filename)
    return(false);
  
  if(0 == strcmp(last_filename_found_in_file, filename) &&
     0 == strcmp(last_filename_file, filename_file))
    return(true);
  else
   { FILE *stream = s_fopen(filename_file, "r");
     boolean answer;

     if(NULL == stream)
      { s_fclose(stream);
	return(false);
      }
     answer = 
       filename_in_filename_stream(filename,type,file_write_date, stream);
     if(answer == true)
      { /* record it in the cache */
	strncpy(last_filename_file, filename_file, MAX_FILE_NAME_LEN);
	strncpy(last_filename_found_in_file, filename, MAX_FILE_NAME_LEN);
      }
     s_fclose(stream);
     return(answer);
   }
}


/*========================*
 *===  Headline Table  ===*
 *========================*/

char *read_headline_table_entry(position,db)
long position;
database* db;
  /* returns the headline array after side effecting it.  Beware that 
   * the next call to this function will overwrite the the headline_array
   */
{
  /* this is the headline that gets returned */
  static char headline_array[MAX_HEADLINE_LEN]; 
  FILE *stream = db->headline_table_stream;
  headline_array[0] = '\0';	/* init to the empty string */
	
  if (0 != fseek(stream, position, SEEK_SET)) { 
    waislog(WLOG_HIGH, WLOG_ERROR, 
	    "fseek failed into the headline index to position %ld in db %s", 
	    position, db->database_file);
    return(headline_array);
  }
  if(false == read_string_from_file(db->headline_table_stream, 
				    headline_array, MAX_HEADLINE_LEN)){ 
   waislog(WLOG_HIGH, WLOG_ERROR, 
	    "headline table is corrupt at %ld in db %s", 
	    position, db->database_file);
  }
  return(headline_array);
}

/* writes the string to the file followed by a NULL.
 * The returned number is the position in the file to start reading.
 */
long write_headline_table_entry(headline,db)
char* headline;
database* db;
{
  /* writes the headline followed by a newline.
     Returns the postion of the headline.
     */
  long free_position;
  s_fseek(db->headline_table_stream, 0L, SEEK_END);
  free_position = ftell(db->headline_table_stream);
  /* printf("Headline position: %ld, next headline length: %ld\n",
	 free_position, strlen(headline)); */
  fprintf(db->headline_table_stream, "%s", headline);
  fputc(0, db->headline_table_stream);
  return(free_position);
}

#ifdef BIO
/*========================*
 *=== delimiters - dgg ===*
 *========================*/

char *read_delimiters(db)
database* db;
  /* returns the word delimiters for a database.  Beware that 
   * the next call to this function will overwrite the the headline_array
   */
{
  static char delimiters[MAX_HEADLINE_LEN+1]; 
  FILE *stream = db->delimiters_stream;
  delimiters[0] = '\0';	/* init to the empty string */
	
  if(false == read_string_from_file(db->delimiters_stream, 
				    delimiters, MAX_HEADLINE_LEN)){ 
   waislog(WLOG_HIGH, WLOG_ERROR, 
	    "delimiters are corrupt in db %s",  db->database_file);
  }
  /* need to weed out .dlm files that have no symbols... */
  if (delimiters[0] == '\0') return(NULL);
  else return(delimiters);
}

/* writes the string to the file followed by a NULL.
 * The returned number is the position in the file to start reading.
 */
long write_delimiters(delimiters,db)
char* delimiters;
database* db;
{
  /* writes the headline followed by a newline.
     Returns the postion of the headline.
     */
  long free_position;
  s_fseek(db->delimiters_stream, 0L, SEEK_SET); /* _SET, only one set of delims / file ? */
  free_position = ftell(db->delimiters_stream);
  fprintf(db->delimiters_stream, "%s", delimiters);
  fputc(0, db->delimiters_stream);
  return(free_position);
}
#endif /* BIO */

/* =================== */
/* === Source file === */
/* =================== */

/* the source file is an ascii file for describing a source.
   it is defined in ../doc/source.txt */

/* Registers the src structure with the directory of servers.
   Return true if successful */
boolean register_src_structure(filename)
char *filename;
{
  char string[200], *editor;
  long answer;
  char *env_user;

  if((editor = (char*)getenv("EDITOR")) == NULL &&
     (editor = (char*)getenv("VISUAL")) == NULL) {
    printf("Could not get EDITOR environment variable.\n"); 
    printf("Please check over the source structure: %s\n", filename);
    printf("Then mail it to wais-directory-of-servers@cnidr.org\n");
    return (false);
  }

  /* register the server with the directory of servers */
  printf("Please look over the Source description.  Be sure it contains\n");
  printf("an IP address and DNS name, as well as the port you intend\n");
  printf("to use for the server.\n");
  printf("When you are finished it will be mailed to the directory of servers.\n");
  fflush(stdout);

  sprintf(string, "exec %s %s", editor, filename);
  system(string);

  printf("Sending source struture to the directory of servers...");

  env_user = getuser_env_name();
  sprintf(string,
	  "cat %s | mail wais-directory-of-servers@cnidr.org %s\n", 
	  filename, env_user);

  answer = system(string);
  printf("Done.\n");      
  return((answer == 0)?true:false);
}


/* Writes a source structure to a file.
   If the export_database arg is set, then the tcp_port is used in the 
   tcp-port slot.
   Returns true if successful. */
#ifdef FIELDS
boolean write_src_structure(filename, database_name, typename, 
			    filenames, count, export_database, tcp_port, db)
#else /* not FIELDS */
boolean write_src_structure(filename, database_name, typename, 
			    filenames, count, export_database, tcp_port)
#endif /* not FIELDS */
     char *filename;
     char *database_name;
     char *typename;
     char **filenames;
     long count;
     boolean export_database;
     long tcp_port;
     database* db;
{
  long i,j;
  char hostname[120];
  struct hostent *h;
  
#ifndef THINK_C

  FILE *source_stream = s_fopen(filename, "w");
  
  fprintf(source_stream, "\n\n(:source \n");
  fprintf(source_stream, "   :version  3 \n");
  if(export_database){
    mygethostname(hostname, 120);
    h = gethostbyname(hostname);
    if (h != NULL && 
	h->h_addr_list != NULL &&
	h->h_addr_list[0] != NULL) {
      fprintf(source_stream,
	      "   :ip-address \"%d.%d.%d.%d\"\n",
	      (unsigned char)h->h_addr_list[0][0], 
	      (unsigned char)h->h_addr_list[0][1],
	      (unsigned char)h->h_addr_list[0][2],
	      (unsigned char)h->h_addr_list[0][3] );
    }
    fprintf(source_stream, "   :ip-name \"%s\"\n", hostname );
    fprintf(source_stream, "   :tcp-port %ld\n", tcp_port);
  }
  fprintf(source_stream, "   :database-name \"%s\"\n", database_name);
  fprintf(source_stream, "   :cost 0.00 \n");
  fprintf(source_stream, "   :cost-unit :free \n");
  fprintf(source_stream, "   :maintainer \"%s\"\n", 
	  current_user_name());
   fprintf(source_stream, "   :keyword-list (\n");
  for (j=0; j< nKeys; j++) {
    if (keywrd[j])
      fprintf(source_stream, "                  %s\n", keywrd[j]);
  }
  fprintf(source_stream, "                  )\n");

#ifdef FIELDS
/*
  :fields           ((:field
                      :name "database-name"
                      :description "database name")
                     (:field
                      :name "ip-name"
                      :description "server name"))
 */

  if(db->number_of_fields > 0) {
      fprintf(source_stream, "   :fields (\n");
      for(i=0; i<db->number_of_fields; i++) {
          if(db->index_fields[i]->field_name != NULL) {
              fprintf(source_stream, "                  (:field\n");
              fprintf(source_stream, "                   :name \"%s\"\n", 
                      db->index_fields[i]->field_name);
              fprintf(source_stream, "                   :type (");
              { 
                  extern fields_struct **index_fields;
                  int fpos = index_fields[i]->number_of_Ftable-1;
                  char ** indextypes = 
                      index_fields[i]->Ftable[fpos].indextypes;
                  while (*indextypes != NULL) {
                      fprintf(source_stream, " %s",
                              *indextypes);
                      indextypes++;
                  }
              }
              fprintf(source_stream, " )\n");
              fprintf(source_stream, 
                      "                   :description \"%s%s\")\n", 
                      (db->index_fields[i]->field_description)?
                      db->index_fields[i]->field_description : "None given",
                      (db->index_fields[i]->is_date)?" (date)":
                      (db->index_fields[i]->numeric?" (numeric)":""));
          }
      }
      fprintf(source_stream, "           )\n");
  }
#endif /* FIELDS */
          if(!nDesLines){
    fprintf(source_stream, "   :description \"Server created with %s on %s by %s\n",
            VERSION_STRING, printable_time(), current_user_name());
    if(count > 0){
      fprintf(source_stream, "The files of type %s used in the index were:\n",
              typename);
      for(i = 0; i < count; i++){
        char full_path[MAX_FILENAME_LEN + 1];
        fprintf(source_stream, "   %s\n", truename(filenames[i], full_path));
      }
    }
#ifdef FIELDS
    if(db->number_of_fields > 0) {
      char fde_filename[MAX_FILENAME_LEN + 1];
      char line[1001];
      FILE* fde_stream = NULL;
      strncpy(fde_filename, db->database_file, 255);
      strncat(fde_filename, ".fde", 255);
      fde_stream = s_fopen(fde_filename, "r");
      fprintf(source_stream, "fields list:\n");
      fprintf(source_stream, "   ");

      if(fde_stream == NULL) {
        for(i=0; i<db->number_of_fields; i++) {
          if(db->index_fields[i]->field_name != NULL)
            fprintf(source_stream, "%s ", db->index_fields[i]->field_name);
        }
      }
      else {
        while(fgets(line, 1000, fde_stream)) {
          fprintf(source_stream, "%s", line);
          fprintf(source_stream, "   ");
        }
        s_fclose(fde_stream);
      }
      fprintf(source_stream, "\n");
    }
#endif /* FIELDS */
    fprintf(source_stream, "\"\n");
  } else
    for (j=0; j<nDesLines; j++)
      fprintf(source_stream, "%s", descript[j]);
  fprintf(source_stream, ")\n");      
  s_fclose(source_stream);
  
#endif /* not THINK_C */

  return(true);
}

boolean
build_catalog(db)
database* db;
{
  char catalog_name[MAX_FILENAME_LEN];
  document_table_entry doc_entry;
  char filename[MAX_FILE_NAME_LEN], type[100];
  FILE *catalog;
  long i;

  sprintf(catalog_name,"%s%s",db->database_file, catalog_ext);
  if((catalog = s_fopen(catalog_name, "w")) == NULL) {
    waislog(WLOG_HIGH, WLOG_ERROR, 
	    "Unable to open catalog file for write: %s.", catalog_name);
    return(false);
  }

  fprintf(catalog, "Catalog for database: %s\n", db->database_file);
  fprintf(catalog, "Date: %s\n", printable_time());

  /* the first document is empty - JG */

  fprintf(catalog, "%ld total document%s\n\n",
	  db->doc_table_allocated_entries-1,
	  (db->doc_table_allocated_entries==2) ? "":"s");
  
  for(i=1; i<db->doc_table_allocated_entries; i++) {
    fprintf(catalog, "Document # %ld\n", i);
    if (read_document_table_entry(&doc_entry, i, db) 
	== true){
      char *hl;
      long hll;
      read_filename_table_entry(doc_entry.filename_id, 
				filename,
				type,
				NULL,
				db);
      hl = read_headline_table_entry(doc_entry.headline_id,db);
      hll = strlen(hl);
      fprintf(catalog, "Headline: %s", hl);
      if((hll== 0) || (hl[hll-1] != '\n')) fprintf(catalog,"\n");

      fprintf(catalog, "DocID: %d %d %s\n\n",
	      doc_entry.start_character, doc_entry.end_character,
	      filename);
    }
    else {
      fprintf(catalog, "Unable to read document table!\n\n");
    }
  }
  s_fclose(catalog);
  return(true);
}

/*****************************/
/***   Database support    ***/
/*****************************/

char* dictionary_filename(destination,db)
char* destination;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination,dictionary_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

#ifdef FIELDS
char* field_dictionary_filename(destination, field_name, db)
char* destination;
char* field_name;
database* db;
{
  strncpy(destination, db->database_file, MAX_FILE_NAME_LEN);
  s_strncat(destination, field_ext, MAX_FILE_NAME_LEN, MAX_FILE_NAME_LEN);
  s_strncat(destination,field_name,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination,dictionary_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}
#endif /* FIELDS */

/* for use in building so that the real one does not get overstomped */
static char* temp_dictionary_filename(destination,db)
char* destination;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination,dictionary_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination,"tmp",MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

#ifdef FIELDS
/* for use in building so that the real one does not get overstomped */
static char* field_temp_dictionary_filename(destination, field_id, db)
char* destination;
int field_id;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination, field_ext, MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination, db->fields[field_id].field_name, 
            MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination,dictionary_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination,"tmp",MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}
#endif /* FIELDS */

char* document_table_filename(destination,db)
char* destination;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination,document_table_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

char* filename_table_filename(destination,db)
char* destination;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination,filename_table_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

char* headline_table_filename(destination,db)
char* destination;
database* db;
{
	strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
	s_strncat(destination,headline_table_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
	return(destination);
}

#ifdef BIO
char* delimiters_filename(destination,db)
char* destination;
database* db;
{
	strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
	s_strncat(destination,delimiters_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
	return(destination);
}
#endif /* BIO */

char* index_filename(destination,db)
char* destination;
database* db;
{
	strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
	s_strncat(destination,index_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
	return(destination);
}

/* this is used during index creation.  if the version is -2, then it means
   the real index_filename.  This is a kludge */
char* index_filename_with_version(version,destination,db)
long version;
char* destination;
database* db;
{
  if(version == -2L){
    return(index_filename(destination, db));
  }
  else{
    sprintf(destination, "%s%s%ld", db->database_file,
	    index_ext, version);
    return(destination);
  }
}

#ifdef FIELDS
char* field_index_filename(destination, field_name, db)
char* destination;
char* field_name;
database* db;
{
   strncpy(destination, db->database_file, MAX_FILE_NAME_LEN);
  s_strncat(destination, field_ext, MAX_FILE_NAME_LEN, MAX_FILE_NAME_LEN);
  s_strncat(destination,field_name,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  s_strncat(destination,index_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

char* field_index_filename_with_version(version,destination, field_id, db)
long version;
char* destination;
long field_id;
database* db;
{
  if(version == -2L){
    return(field_index_filename(destination, 
                                db->fields[field_id].field_name, db));
  }
  else{
    sprintf(destination, "%s%s%s%s%ld", db->database_file, field_ext,
	    db->fields[field_id].field_name, index_ext, version);
    return(destination);
  }
}
#endif /* FIELDS */

char* source_filename(destination,db)
char* destination;
database* db;
{
  strncpy(destination, db->database_file,MAX_FILE_NAME_LEN);
  s_strncat(destination,source_ext,MAX_FILE_NAME_LEN,MAX_FILE_NAME_LEN);
  return(destination);
}

char*
get_doc(destination, document_id, db, headline)
char* destination;
long document_id;
database* db;
boolean headline;
{
  document_table_entry doc_entry;
  char filename[MAX_FILE_NAME_LEN], type[100];
  char *hl;

  if (read_document_table_entry(&doc_entry, document_id, db) 
      == true){
    read_filename_table_entry(doc_entry.filename_id, 
			      filename,
			      type,
			      NULL,
			      db);

/* francois - multitype extension */
	if ( strstr(type,",") != NULL ) {
	   type[strstr(type,",") - type] = '\0';
	}


    if (headline == TRUE) {
      hl = read_headline_table_entry(doc_entry.headline_id,db);
      sprintf(destination, "%d %d %s, \"%s\"", 
	      doc_entry.start_character, doc_entry.end_character,
	      filename, hl);
    }
    else
      sprintf(destination, "%d %d %s", 
	      doc_entry.start_character, doc_entry.end_character,
	      filename);
    return(s_strdup(type));
  }
  else return NULL;
}

long next_doc(destination, docID, db)
char* destination;
char* docID;
database* db;
{
  long i, start, end;
  char doc[MAX_FILE_NAME_LEN+50], fn[MAX_FILE_NAME_LEN];
  char *type, *loc;

  for(i = 0; i < db->doc_table_allocated_entries; i++) {
    if ((type = get_doc(doc, i, db, FALSE)) != NULL) {
      s_free(type);
      if (strcmp(doc, docID) == 0) {
	type = get_doc(doc, i+1, db, TRUE);
	sscanf(doc, "%d %d %s", &start, &end, fn);
	if((loc = strstr(doc, ",")) == NULL) return -1;
	fn[loc-doc] = 0;
	sprintf(destination, "%s, %s", doc, type);
	s_free(type);
	if( end != 0)
	  return(end-start);
	else {	
	  /* whole file, find file length from the file */
	  long size;
	  FILE* file = NULL;
	  if (((file = s_fopen(fn, "r")) != NULL) &&
	      (fseek(file, 0L, SEEK_END) == 0)  &&
	      ((size = ftell(file)) != -1)) {
	    s_fclose(file);
	    return(size);	/* we are done, bytes is set */
	  }
	  else {
	    s_fclose(file);
	    return(-1);		/* something went wrong with the file */
	  }
	}
      }
    }
  }
  return -1;
}

long previous_doc(destination, docID, db)
char* destination;
char* docID;
database* db;
{
  long i, start, end;
  char doc[MAX_FILE_NAME_LEN+50], fn[MAX_FILE_NAME_LEN];
  char *type, *loc;

  for(i = 0; i < db->doc_table_allocated_entries; i++) {
    if ((type = get_doc(doc, i, db, FALSE)) != NULL) {
      s_free(type);
      if (strcmp(doc, docID) == 0) {
	if (i != 0) {
	  type = get_doc(doc, i-1, db, TRUE);
	  sscanf(doc, "%d %d %s", &start, &end, fn);
	  if((loc = strstr(doc, ",")) == NULL) return -1;
	  fn[loc-doc] = 0;
	  sprintf(destination, "%s, %s", doc, type);
	  s_free(type);
	  if( end != 0)
	    return(end-start);
	  else {	
	    /* whole file, find file length from the file */
	    long size;
	    FILE* file = NULL;
	    if (((file = s_fopen(fn, "r")) != NULL) &&
		(fseek(file, 0L, SEEK_END) == 0)  &&
		((size = ftell(file)) != -1)) {
	      s_fclose(file);
	      return(size);	/* we are done, bytes is set */
	    }
	    else {
	      s_fclose(file);
	      return(-1);	/* something went wrong with the file */
	    }
	  }
	}
      }
    }
  }
  return(-1);
}

long next_docid(docID, db)
char* docID;
database* db;
{
  long i;
  char doc[MAX_FILE_NAME_LEN+50];

  for(i = 0; i < db->doc_table_allocated_entries; i++) {
    if (get_doc(doc, i, db, FALSE) != NULL) {
      if (strcmp(doc, docID) == 0) {
	return (i+1);
      }
    }
  }
  return -1;
}

long previous_docid(docID, db)
char* docID;
database* db;
{
  long i;
  char doc[MAX_FILE_NAME_LEN+50];

  for(i = 0; i < db->doc_table_allocated_entries; i++) {
    if (get_doc(doc, i, db, FALSE) != NULL) {
      if (strcmp(doc, docID) == 0) {
	return (i-1);
      }
    }
  }
  return -1;
}

