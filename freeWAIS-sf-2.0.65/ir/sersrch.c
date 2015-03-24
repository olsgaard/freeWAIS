/*                               -*- Mode: C -*- 
 * 
 * sersrch.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Jonathan Goldman
 * Created On      : Fri Jun 23 14:05:18 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Dec 29 13:26:37 1995
 * Language        : C
 * Update Count    : 44
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 */

/* Copyright (c) CNIDR (Work in progress) */

/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.    
   Brewster@think.com
 */

/* implements the search part of irext.h 
   (search_word and finished_search_word)
   -brewster

   Split from irsearch.c

   5/31/91 Added scale_scores.  Fixed document_score_array to long.
   7/8/91 Removed scale_scores, handled in search_word with doc_id > 0.
   2/4/92 Made document_score_array a double.

   - Jonny G
   * $Log: sersrch.c,v $
   * Revision 2.0.1.10  1995/12/29 12:30:54  pfeifer
   * patch60: String search in global field did not work if other fields
   * patch60: occurred in the query.
   *
   * Revision 2.0.1.9  1995/12/06  11:03:55  pfeifer
   * patch53: Change float parameter of search_word() and to double.
   * patch53: (Patch from Rodney Barnett).
   *
   * Revision 2.0.1.8  1995/11/14  18:20:50  pfeifer
   * patch51: Fix for literal searches from Ellen G. White.
   *
   * Revision 2.0.1.7  1995/11/09  14:07:17  pfeifer
   * patch46:
   *
   * Revision 2.0.1.5  1995/10/20  17:09:01  pfeifer
   * patch40: Fixed weigting bug for numeric searches.
   *
   * Revision 2.0.1.4  1995/10/10  18:33:51  pfeifer
   * patch33: Fixed bug (?) from tung. The field 'text' was not recognized
   * patch33: in every case.
   *
   * Revision 2.0.1.4  1995/10/10  18:33:51  pfeifer
   * patch33: Fixed bug (?) from tung. The field 'text' was not recognized
   * patch33: in every case.
   *
   * Revision 2.0.1.3  1995/10/10  12:40:27  pfeifer
   * patch31: Fixed weighting of partial matches for numeric fields.  Patch
   * patch31: depends on the internal_weight of numeric matches to be 1.0
   *
   * Revision 2.0.1.2  1995/09/12  12:28:30  pfeifer
   * patch6: Removed inclusion of byte_order.h.
   *
   * Revision 2.0.1.1  1995/09/11  12:51:27  pfeifer
   * patch4: Added typecasts for readCompressedInteger() to make gcc happy.
   *
   * Revision 2.0  1995/09/08  08:01:08  pfeifer
   * Metaconfig baseline
   *
   * Revision 1.64  1995/06/29  13:42:12  pfeifer
   * Hack for LITTLEENDIAN in serach_word
   *
   * Revision 1.63  1995/06/23  12:06:46  pfeifer
   * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS | indent
   *
   * Revision 1.62  1995/06/14  17:58:05  pfeifer
   * compare_doc_ids: _AP wrap for K&R compilers
   *
   * Revision 1.61  1995/05/07  16:04:57  pfeifer
   * More fixes by Tom Snee. Literal serach now works again!
   *
   * Revision 1.57  1995/04/30  15:46:56  pfeifer
   * Vor unifdef PROXIMITY
   *
   * Revision 1.56  1995/04/27  12:06:19  pfeifer
   * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
   *
   * Revision 1.55  1995/04/17  13:18:29  huynh1
   * Release 1.1.2
   *
   * Revision 1.54  1994/12/13  17:03:58  pfeifer
   * *** empty log message ***
   *
   * Revision 1.53  1994/11/14  15:58:17  pfeifer
   * Patch by Archie Warnoc in c.i.w (must be made size dependent?)
   *
   * Revision 1.52  1994/09/06  16:53:48  pfeifer
   * Syn cache patch
   *
   * Revision 1.51  1994/08/05  09:46:46  pfeifer
   * No more 'MAXINT redefined' complaints.
   *
   * Revision 1.50  1994/08/05  07:12:38  pfeifer
   * Release beta 04
   *
   * Revision 1.49  1994/07/13  07:52:36  huynh1
   * Uli
   *
   * Revision 1.48  1994/05/27  09:13:21  huynh1
   * boolean code updated. beta
   *
   * Revision 1.47  1994/05/26  14:33:57  huynh1
   * search_word updated (read_weight_from_stream).
   * beta.
   *
   * Revision 1.46  1994/05/20  12:49:58  pfeifer
   * beta
   *
   * Revision 1.45  1994/05/19  12:44:39  huynh1
   * search_word updated.
   *
   * Revision 1.44  1994/05/18  17:28:13  huynh1
   * new term weighting
   * higher retrieval quality.
   *
   * Revision 1.40  1994/04/28  16:28:01  huynh1
   * stemming
   *
   * Revision 1.39  1994/04/06  23:52:04  huynh1
   * 08, autoconf, Uli
   *
   * Revision 1.38  1994/03/23  13:11:07  pfeifer
   * removed include iso.h
   *
   * Revision 1.37  1994/03/08  20:46:12  huynh1
   * Patchlevel 04
   *
   * Revision 1.36  1994/02/14  10:33:04  huynh1
   * new code for field concept added.
   *
   * Revision 1.36  1993/12/08  17:38:00  huynh1
   * bug by mixing literal and nested boolean corrected!
   *
   * Revision 1.10  1993/10/13  14:14:20  huynh1
   * new code added for encapsulated boolean queries and
   * modified literal search
   *
   * Revision 1.3  1993/07/13  08:19:56  pfeifer
   * Sicherung vor Aenderungen Tung
   *
   * Revision 1.1  1993/02/16  15:05:35  freewais
   * Initial revision
   *
   * Revision 1.24  92/04/28  16:56:54  morris
   * added boolean to serial engine
   * 
   * Revision 1.23  92/03/15  10:15:18  jonathan
   * Added Simon Spero's ASSIGN replacement for read_bytes.
   * 
   * Revision 1.22  92/03/05  07:09:54  shen
   * add two more dummy arguments to call to init_search_engine
   * 
   * Revision 1.21  92/02/12  17:29:52  jonathan
   * Conditionalized inclusion of object code.
   * 
   * Revision 1.20  92/02/12  13:40:06  jonathan
   * Added "$Log" so RCS will put the log message in the header
   * 
 */

#include "cutil.h"
#include "irfiles.h"
#ifdef BIO
#include "irtfiles.h"		/* dgg, for wordDelimiter */
#endif /* BIO */
#include "irsearch.h"
#include "irext.h"
#include <ctype.h>

#include <math.h>

#ifdef MAXINT
#undef MAXINT
#endif /* MAXINT */
#define MAXINT (unsigned long)2^(sizeof(long)*8-1)
#define VALUE 1000000L
/* francois */
#include "stemmer.h"

/* tung, 10/93 */
#include "boolean_op.h"
/* tung, 10/93 */

#include "field_search.h"

#ifdef NEW_WEIGHT
#include "weight.h"
#endif /* NEW_WEIGHT */

#ifdef BOOL
#include "obj.h"
#include "irparse.h"
object         *currentQuery = NULL;	    /* kludge until irext goes away */

#endif /* BOOL */

/* weighting for relevant document terms - 
   this may become a parameter to the query.
 */

#define RF_WEIGHTING 0.1

/* ==================================
 * ===  Initialization Functions  ===
 * ==================================*/

long 
init_search_engine (file, initialize, for_search, cm_mem_percent,
		    text_size, grow_percent)
     char           *file;
     boolean         initialize;
     boolean         for_search;
     long            cm_mem_percent;	    /* unused */
     long            text_size;		    /* unused */
     long            grow_percent;	    /* unused */
{
  static boolean  inited = false;

  if (inited == false) {
#ifdef BOOL
    initObj ();
    initBool ();
#endif /* BOOL */
    inited = true;
  }
  return (0);
}

long 
finished_search_engine ()
{
#ifdef CACHE_SYN
  /* clean up shared memory segments */
  if (cacheSynId) {
    int             i;
    char           *pcs;
    t_cacheSyn     *syn_Cache, *cs;

    if ((syn_Cache = (t_cacheSyn *) shmat (cacheSynId, 0, 0)) !=
	((t_cacheSyn *) - 1)) {
      pcs = (char *) syn_Cache;
      for (i = 0, cs = (t_cacheSyn *) pcs; i < MAX_SYN_CACHE && cs->id;
	   i++, pcs += sizeof (t_cacheSyn), cs = (t_cacheSyn *) pcs)
	if (shmctl (cs->id, IPC_RMID, (t_cacheSyn *) 0) < 0)
	  waislog (WLOG_HIGH, WLOG_WARNING, "Error detatching shared memory segment (id=%d)", cs->id);
      if (shmctl (cacheSynId, IPC_RMID, (t_cacheSyn *) 0) < 0)
	waislog (WLOG_HIGH, WLOG_WARNING, "Error detatching shared memory segment (id=%d)", cacheSynId);
    }
  }
#endif /* CACHE_SYN */
  return (0);
}

/*
 *  ext_open_database: see irext.h
 */

long 
ext_open_database (db, initialize, for_search)
     database       *db;
     boolean         initialize;
     boolean         for_search;
{				/* this has to deal with the .inv file */
  char            file[MAX_FILE_NAME_LEN];

  if (initialize)		/* make a new one */
    db->index_stream = s_fopen (index_filename (file, db), "w+b");
  else if (for_search)		/* just search */
    db->index_stream = s_fopen (index_filename (file, db), "rb");
  else				/* write to an existing db */
    db->index_stream = s_fopen (index_filename (file, db), "r+b");

  if (db->index_stream == NULL) {
    waislog (WLOG_HIGH, WLOG_ERROR, "2can't open the inverted index file %s\n",
	     file);
    disposeDatabase (db);
    return (1);
  }
  return (0);
}

/*
 *  ext_close_database: see irext.h
 */

long 
ext_close_database (db)
     database       *db;
{
  return (0);
}

char           *
database_file (database_name)
     char           *database_name;
{
  return (database_name);
}

/*===========================*
 *===  Setting Paramters  ===*
 *===========================*/

long            max_hit_retrieved = 0;
char          **srcs = NULL;

long 
set_query_parameter (mask, parameters)
     long            mask;
     query_parameter_type *parameters;
{
  switch (mask) {
  case SET_MAX_RETRIEVED_MASK:
    max_hit_retrieved = parameters->max_hit_retrieved;
    return (0);
    break;
  case SET_SELECT_SOURCE:
    if (NULL != srcs) {
      if (NULL != srcs[0])
	s_free (srcs[0]);
      s_free (srcs);
    }
    srcs = parameters->srcs;
    break;
  default:
    return (-1);
    break;
  }
  return (0);
}

/*==============================*
 *===  Document Score Array  ===*
 *==============================*/

double         *document_score_array = NULL;
long            document_score_array_len = 0;

/* tung, 10/93 */
search_result_struct *search_result_array = NULL;
long            operand_id = 0;

static void clear_search_result_array _AP ((long *number_of_elements));
static void 
clear_search_result_array (number_of_elements)
     long           *number_of_elements;
{
  long            count;

  if (*number_of_elements > 1 && search_result_array != NULL) {
    for (count = 0; count < *number_of_elements; count++) {
      if (search_result_array[count].doc_ids_array != NULL)
	s_free (search_result_array[count].doc_ids_array);
    }
    s_free (search_result_array);
  }
  *number_of_elements = 1;
}

static void make_search_result_array _AP ((long length));
static void 
make_search_result_array (length)
     long            length;
{
  int             i;

  if (search_result_array == NULL) {
    search_result_array =
      (search_result_struct *)
      s_malloc ((size_t) (length * 1.5 * sizeof (search_result_struct)));
    /* multiply by 1.5 just in case each pair of operands is capitalized.
       Whenever WAIS encounters two capitalized words, it automatically
       generates a third concatenated word, so we have to be ready to
       handle three words when the user only speicifes two.  This is part
       of "word pairs".  I have never heard of anyone using the word pair
       functionality, and it generates unpredictable results with databases
       not built with waisindex -pairs, so I wish we could get rid of it. */
    for (i = 0; i < length; i++)
      search_result_array[i].doc_ids_array = NULL;
    operand_id = 0;
  }
}

/* make_doc_ids_array
   Parameter pos is an index into the global array search_result_array.
   If the doc_ids_array field of the pos'th element of search_result_array
   is NULL, then allocate a doc_descr_struct array of size length for it.
   If the array already exists, grow it by length elements and copy the
   data into the top of the array, so the bottom length elements can be
   safely used.

   effects: returns true if the array was made.  Adds the array to the
   doc_ids_array field of the pos'th element of the global array
   search_result_array.
 */

static boolean make_doc_ids_array _AP ((long pos, long length));
static boolean 
make_doc_ids_array (pos, length)
     long            pos;
     long            length;
{
  long           new_size, original_size;
  doc_descr_struct *new_array;

  if (search_result_array[pos].doc_ids_array == NULL) {
    search_result_array[pos].number_of_hits = length;
    search_result_array[pos].doc_ids_array =
      (doc_descr_struct *)
      s_malloc ((size_t) (sizeof (doc_descr_struct) * length));
  } else {
    /* if the array already exists, make it larger by "length" elements.  */
    original_size = search_result_array[pos].number_of_hits;
    new_size = original_size + length;
    new_array =
      (doc_descr_struct *)
      s_malloc ((size_t) (sizeof (doc_descr_struct) * new_size));
    if (new_array == NULL) {
      waislog (WLOG_HIGH, WLOG_ERROR, "Out of memory");
      return (false);
    } else {
      /* copy the existing data to the end of the array so it will not be
         overwritten. */
      memcpy (new_array + length,
	      search_result_array[pos].doc_ids_array,
	      sizeof (doc_descr_struct) * original_size);
      s_free (search_result_array[pos].doc_ids_array);
      search_result_array[pos].doc_ids_array = new_array;
    }
    search_result_array[pos].number_of_hits = new_size;
  }
  if (search_result_array[pos].doc_ids_array == NULL) {
    waislog (WLOG_HIGH, WLOG_ERROR, "Out of memory");
    return (false);
  }
  return (true);
}
/* tung, 10/93 */

/* make_document_score_array insures that the document_score_array
   array is long enough, if not it makes it long enough */
static void make_document_score_array _AP ((long length));
static void 
make_document_score_array (length)
     long            length;
{
  if (length <= document_score_array_len)
    return;
  /* we have to make a new one.  free the old one first (if any) */
  if (document_score_array != NULL) {
    s_free (document_score_array);
  }
  document_score_array = (double *) s_malloc ((size_t) (length * sizeof (double)));

  document_score_array_len = length;
}

static void destroy_document_score_array _AP ((void));
static void 
destroy_document_score_array ()
{
  s_free (document_score_array);
  document_score_array_len = 0;
}

void 
clear_document_score_array ()
     /* side effects the document_score_array. */
{
  memset (document_score_array, 0,
	  document_score_array_len * sizeof (double));
}

/* for debugging purposes */
void 
print_document_score_array (start, stop)
     unsigned long   start;
     unsigned long   stop;

/* assumes start >= 0, stop < db->doc_table_allocated_entries */
{
  long            i;

  for (i = start; i <= stop; i++) {
    printf ("entry number %ld: %f \n",
	    i, document_score_array[i]);
  }
}

/*=========================*
 *===  Best Hits Array  ===*
 *=========================*/

hit            *best_hits_array = NULL;
long            best_hits_array_len = 0;
long            current_best_hit = 0;
long            doc_start = 0;		    /* tung, 5/94 */
long            doc_end = 0;		    /* tung, 5/94 */

/* see irext.h for doc */
long 
init_best_hit (db)
     database       *db;
{

#ifdef BOOL
  if (currentQuery != NULL)
    send (currentQuery, InitBestHit, db);
#endif /* BOOL */

  return (0);
}

/* make_best_hits_array insures that the best_hits_array
   array is long enough, if not it makes it long enough */
static void make_best_hits_array _AP ((long length));
static void 
make_best_hits_array (length)
     long            length;
{
  if (length <= best_hits_array_len)
    return;
  /* we have to make a new one.  free the old one first (if any) */
  if (best_hits_array != 0) {
    s_free (best_hits_array);
  }
  best_hits_array = (hit *) s_malloc ((size_t) (length * sizeof (hit)));
  best_hits_array_len = length;
}

static void destroy_best_hits_array _AP ((void));
static void 
destroy_best_hits_array ()
{
  s_free (best_hits_array);
  best_hits_array_len = 0;
}

void 
clear_best_hits_array ()
/* side effects the best_hits_array.  XXX could use memset */
{
  memset ((char *) best_hits_array, 0, best_hits_array_len * sizeof (hit));
}

/* for debugging purposes */
void 
print_best_hits ()
{
  long            i;

  for (i = 0; i < best_hits_array_len; i++) {
    if (best_hits_array[i].weight != 0) {
      printf ("Best hit %ld: weight %f, doc_id %ld, headline %s, filename %s, lines %ld\n",
	      i, best_hits_array[i].weight,
	      best_hits_array[i].document_id,
	      best_hits_array[i].headline,
	      best_hits_array[i].filename,
	      best_hits_array[i].number_of_lines);
    }
  }
}

void 
sort_best_hits (db)
     database       *db;
{
  /* returns nothing.
   * side effects best_hits and document_score_array
   */

  long            i, doc;
  double          worst_weight_to_make_it = 0.0;
  document_table_entry doc_entry;
  long            best_hit_number = 0;

  /* snuff the scores */
  for (i = 0; i < max_hit_retrieved; i++) {
    best_hits_array[i].weight = 0.0;

  }

  /* loop over the doc, and keep the doc_id and weight in best hit table */
  /* for(doc = 0; doc < db->doc_table_allocated_entries; doc++){ */
  for (doc = doc_start; doc <= doc_end; doc++) {
    double          weight = document_score_array[doc];

    /* jmf */
    if (weight > 0) {
#ifndef NEW_WEIGHT
      read_document_table_entry (&doc_entry, doc, db);	/* if this could be
							   removed, we'd gain speed */
      if (doc_entry.document_length)
	weight /= doc_entry.document_length;
      else
	weight = 0;
#endif /* not NEW_WEIGHT */
      if (worst_weight_to_make_it < weight) {
	/* merge it into the best_hits array. start at the bottom */
	for (i = (max_hit_retrieved - 1); i >= 0; i--) {
	  if (weight > best_hits_array[i].weight
	  /* && (check_document_id(doc, db) == true) too slow. */
	    ) {
	    /* move this entry down */
	    if ((i + 1) < max_hit_retrieved) {
	      best_hits_array[i + 1].weight = best_hits_array[i].weight;
	      best_hits_array[i + 1].document_id = best_hits_array[i].document_id;
	    }
	    best_hits_array[i].document_id = doc;
	    best_hits_array[i].weight = weight;
	  } else
	    break;
	}
      }
    }
  }
  doc_start = doc_end = 0;	/* tung, 5/94 */
  for (i = 0; i < max_hit_retrieved; i++) {
    if (best_hits_array[i].weight <= 0.0)
      return;
    if (read_document_table_entry (&doc_entry,
				   best_hits_array[i].document_id,
				   db)
	== true) {
      best_hits_array[best_hit_number].weight = best_hits_array[i].weight;

      best_hits_array[best_hit_number].document_id = best_hits_array[i].document_id;
      best_hits_array[best_hit_number].start_character = doc_entry.start_character;
      best_hits_array[best_hit_number].end_character = doc_entry.end_character;
      best_hits_array[best_hit_number].document_length = doc_entry.document_length;
      best_hits_array[best_hit_number].number_of_lines = doc_entry.number_of_lines;
      sprintf (best_hits_array[best_hit_number].date, "%d", doc_entry.date);
      read_filename_table_entry (doc_entry.filename_id,
				 best_hits_array[best_hit_number].filename,
				 best_hits_array[best_hit_number].type,
				 NULL,
				 db),
	strncpy (best_hits_array[best_hit_number].headline,
		 read_headline_table_entry (doc_entry.headline_id, db),
		 MAX_HEADLINE_LEN);
      best_hit_number++;
    }
    beFriendly ();
  }
  for (i = best_hit_number; i < max_hit_retrieved; i++) {
    best_hits_array[best_hit_number].weight = 0.0;
  }
  /* print_best_hits(s);  for debugging */
}

/* returns the next best hit */
long 
best_hit (db, doc_id, best_character, best_line, score, start, end, date,
	  length, nlines, headline, filename, type)
     database       *db;
     long           *doc_id;
     long           *best_character;
     long           *best_line;
     double         *score;
     long           *start, *end, *date, *length, *nlines;
     char           *headline, *filename, *type;
{
  double          tmp;

  *best_character = 0;
  *best_line = 0;

#ifdef BOOL
  if (currentQuery != NULL) {	/* for boolean */
    send (currentQuery, GetBestHit, db, doc_id, best_character, best_line, score);
    if (*doc_id > 0)
      return (0);		/* ok */
    else
      return (-1);		/* no more docs */
  }
#endif /* BOOL */

  if (current_best_hit > best_hits_array_len)
    return (1);
  if (best_hits_array[current_best_hit].weight == 0.0)
    return (1);
  *doc_id = best_hits_array[current_best_hit].document_id;
  tmp = ((double) (best_hits_array[current_best_hit].weight * VALUE));
  *score = tmp;
  *start = best_hits_array[current_best_hit].start_character;
  *end = best_hits_array[current_best_hit].end_character;
  *date = atol (best_hits_array[current_best_hit].date);
  *length = best_hits_array[current_best_hit].document_length;
  *nlines = best_hits_array[current_best_hit].number_of_lines;
  strcpy (headline, best_hits_array[current_best_hit].headline);
  strcpy (filename, best_hits_array[current_best_hit].filename);
  strcpy (type, best_hits_array[current_best_hit].type);
  current_best_hit++;
  return (0);
}

long 
finished_best_hit (db)
     database       *db;
{

#ifdef BOOL
  if (currentQuery != NULL) {	/* for boolean */
    send (currentQuery, Delete);
    currentQuery = NULL;
    return (0);
  }
#endif /* BOOL */

  /* if we are on a small machine, we might want to 
     destroy_document_score_array */
  clear_document_score_array ();
  clear_best_hits_array ();
  current_best_hit = 0;
  return (0);
}

/*=============================*	
 *===  Searching for words  ===*
 *=============================*/

/* see irext.h for doc */
long 
init_search_word (db)
     database       *db;
{
  char            fn[256];

  strcpy (fn, db->database_file);
  strcat (fn, synonym_ext);
  syn_ReadFile (fn, &db->syn_Table, &db->syn_Table_Size);

  return (0);
}

int compare_doc_ids _AP ((const void *a, const void *b));
int 
compare_doc_ids (a, b)
     const void     *a;
     const void     *b;
{
  const doc_descr_struct *first = a, *second = b;

  return (first->doc_id - second->doc_id);
}

/* tung, 10/93 */
extern long     number_of_operands;

/* tung, 10/93 */

#ifdef BOOLEANS
static boolean  gLastAnd = false;
static boolean  gLastNot = false;

#endif /* BOOLEANS */

/* tung 2,95 */
#define MAX_LINE_LENGTH 1000
static long     current_index_block_size = 0;
unsigned char  *posting_list = NULL;
unsigned char  *index_block_header = NULL;

/* see irext.h for doc */
long 
search_word (word,
	     field_name,
	     char_pos, line_pos, weight, query_weight, doc_id,
	     word_pair, db)
     char           *word;		    /* the word to be searched for */
     char           *field_name;
     long            char_pos;		    /* the position of the start of the word */
     long            line_pos;		    /* is this needed? not for signature system */
     long            weight;		    /* Tung missused this for different flags */
     double          query_weight;          /* how important the word looks syntactically,
					       such as is it bold */
     long            doc_id;		    /* current document, seed words is 0,

					       then it increments into the relevant 
					       document */
     long            word_pair;
     database       *db;
{
  /* this side effects the document_score_array,
   * and downcases the word.
   * Returns 0 if successful or word not present, 
   * returns non-0 if an error.
   *
   */

  long            not_full_flag = INDEX_BLOCK_FULL_FLAG;	/* start out full so it will go on looking */
  long            count, index_block_size;
  long            internal_document_id, number_of_valid_entries;
  double          internal_weight;
  long            index_file_block_number;
  long            number_of_occurances;

  FOUR_BYTE       index_buffer_data[INDEX_ELEMENT_SIZE * (1024 / 4)];
  char           *index_buffer;
  FILE           *stream = NULL;

#ifdef LITERAL
  long            txt_pos, icnt, wcnt, pcnt;	/* 2/92 GS TLG */
  document_table_entry doc_entry;	    /* 2/92 GS TLG */
  static FILE    *txt_stream = NULL;	    /* 2/92 GS TLG */
  char            cmpr_word[MAX_PHRASE_LENGTH + 1];	/* 2/92 GS TLG */

  /*char phrase[MAX_PHRASE_LENGTH + 1]; *//* 2/92 GS TLG */
  char            txt_filename[MAX_FILENAME_LEN + 1];	/* 2/92 GS TLG */
  char           *temp_txt_filename = NULL; /* francois */
  char            prev_txt_filename[MAX_FILENAME_LEN + 1];	/* 2/92 GS TLG */
  char            txt_type[MAX_TYPE_LEN + 1];	/* 2/92 GS TLG */
  long            phraselen = 0, txt_pos_fix = 0;

  long            phrase_count = 0;	    /* tung , 10/93 */
  char           *tmpindex;		    /* tung , 2/95 */
  char            document_line[MAX_LINE_LENGTH + 1];	/* tung , 2/95 */
  long            char_list_size_readed = 0;	/* tung , 2/95 */
  long            char_list_size = 0;	    /* tung , 2/95 */
  long            first_txt_pos;	    /* tung , 2/95 */
  long            distance = 0;		    /* tung , 2/95 */
  long            prev_distance = 0;	    /* tung , 2/95 */
  boolean         literal_match = false;    /* tung , 2/95 */
  boolean         first_char_pos_readed = false;	/* tung , 2/95 */
  unsigned char  *char_list = NULL;	    /* tung , 2/95 */
  unsigned char  *tmp_char_list = NULL;	    /* tung , 2/95 */
  unsigned char  *prev_char_list = NULL;    /* tung , 2/95 */
  int FreadStatus;		/* Ellen G. White Estate, Novemeber 13, 1995 */

#endif /* LITERAL */

  long            posting_list_pos = 0;	    /* tung , 2/95 */

#ifdef NEW_WEIGHT
  double          query_wgt;
  double          idf;

#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
  double          idf;

#endif /* not NEW_WEIGHT */
  long            field_id = -1;
  boolean         SearchField = false;

  /* do synonym conversion */

  /* in theory, one can replace a word with a boolean phrase */
  char           *newword;
#ifdef PARTIALWORD
  char           *matchword;
#endif /* PARTIALWORD */
  newword = lookup_Synonym (word, db->syn_Table, db->syn_Table_Size);
  waislog (WLOG_HIGH, WLOG_INFO, "Word %s Syn %s", word, newword);
  strncpy (word, newword, MAX_WORD_LENGTH);

  /*if (db->number_of_fields > 0) {*/
    if (*field_name != '\0') {
      if (strcmp (field_name, FREE_TEXT_FIELD) == 0) {	/* global database */
	field_name = "\0";
	SearchField = false;
	field_id = -1;
      } else {
	SearchField = true;
	field_id = pick_up_field_id (field_name, db);
      }
    }
  /*}*/
/* tung, 10/93 */
  if (number_of_operands > 1) {
    make_search_result_array (number_of_operands);
    if ((weight != LITERAL_FLAG) && IsOperator (word)) {
      boolean_operations (word, search_result_array);
      return (0);
    }
    if (strlen (word) == 1) {
      search_result_array[operand_id].number_of_hits = 0;
      search_result_array[operand_id].operand_id = operand_id;
      if (!save_operand_id (operand_id, search_result_array))
	return (-1);
      ++operand_id;
      return (0);
    }
  }
/* tung, 10/93 */

  /* francois - call the stemmer */
#ifdef LITERAL
  if (weight != LITERAL_FLAG && weight != FIELD_FLAG && weight != NUMERIC_FLAG) {
#else
  if (weight != FIELD_FLAG && weight != NUMERIC_FLAG) {
#endif /* not LITERAL */
#ifdef STEM_WORDS
    if (field_id > -1) {
      if (db->fields[field_id].stemming)
	stemmer (word);
    } else {
      if (db->stemming)
	stemmer (word);
    }
#endif /* STEM_WORDS */
  }
#ifdef LITERAL
  if (weight == LITERAL_FLAG) {
    /* printf("search_word: literal word is [%s]\n", word); */
  } else
#endif /* LITERAL */

    index_buffer = (char *) index_buffer_data;

#ifndef PROXIMITY
#ifdef LITERAL
  if (weight == LITERAL_FLAG) {
    /* note: we found the first word of phrase once in map_over_words, 
       but i'm too lazy to put another parameter in that cascade of function
       calls it takes to get here. */

    char            word1[MAX_WORD_LENGTH + 1];
    register int    i, j, len;
    register boolean more;

    phraselen = MINIMUM (MAX_PHRASE_LENGTH, strlen (word));
    len = MINIMUM (MAX_WORD_LENGTH, phraselen);
    j = 0;
    for (i = 0, more = true; i < len && more;) {
      word1[j++] = word[i++];
#ifdef BIO
      if (wordDelimiter (word[i]) != NOT_DELIMITER)
#endif /* BIO */
#ifndef BIO
	if (isalnum (word[i]) == false)
#endif /* not BIO */
	{
	  word1[j] = '\0';
	  txt_pos_fix = i + 1;
#ifdef STEM_WORDS
	  if (field_id > -1) {
	    if (db->fields[field_id].stemming)
	      stemmer (word1);
	  } else {
	    if (db->stemming)
	      stemmer (word1);
	  }
#endif /* STEM_WORDS */
	  /* printf("search_word: literal word1 is [%s]\n", word1); */
	  if ((db->number_of_fields == 0) || !SearchField)
	    index_file_block_number =
	      look_up_word_in_dictionary (word1, &number_of_occurances, db);
	  else
	    index_file_block_number =
	      field_look_up_word_in_dictionary (field_name, word1, &number_of_occurances, db);
	  if (index_file_block_number >= 0)
	    more = false;
	  i++;
	  j = 0;
	}
    }
  } else
#endif /* LITERAL */

#endif /* not PROXIMITY */
#ifdef PARTIALWORD
    index_file_block_number =
      look_up_partialword_in_dictionary (field_name,
					 word, &number_of_occurances, &matchword, db);
  /*
  if (matchword != NULL)
      fprintf(stderr, "look_up_partialword_in_dictionary:: %s\n", matchword);
  */
#else /* PARTIALWORD */
    index_file_block_number =
      look_up_word_in_dictionary (word, &number_of_occurances, db);
#endif /* not PARTIALWORD */

  current_best_hit = 0;		/* so that the best hits willstart from 0 */

  /* check the document_score_array */
  if (document_score_array_len < db->doc_table_allocated_entries)
    make_document_score_array (db->doc_table_allocated_entries);

  if (index_file_block_number >= 0) {
#ifdef PARTIALWORD
    while (index_file_block_number > 0) {	/* dgg, need 2nd loop here for multiple partwords */
#endif /* PARTIALWORD */

      if (SearchField && *field_name != '\0')
	stream = db->field_index_streams[pick_up_field_id (field_name, db)];
      else
	stream = db->index_stream;

      while ((not_full_flag != INDEX_BLOCK_NOT_FULL_FLAG) &&
	     (index_file_block_number != 0)) {
	/* read the index block */
	if (0 != fseek (stream, (long) index_file_block_number,
			SEEK_SET)) {
	  waislog (WLOG_HIGH, WLOG_ERROR,
		   "fseek failed into the inverted file to position %ld",
		   (long) index_file_block_number);
#ifdef BOOLEANS
	  gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	  return (-1);
	}
/* tung, 2/95 */
	if (index_block_header == NULL) {
	  index_block_header = (unsigned char *) calloc ((size_t) (INDEX_BLOCK_HEADER_SIZE * sizeof (char)),
						                (size_t) 1);
	}
	if (index_block_header != NULL) {
	  if (0 > fread_from_stream (stream, index_block_header,
				     INDEX_BLOCK_HEADER_SIZE)) {
	    waislog (WLOG_HIGH, WLOG_ERROR,
		     "Could not read the index block");
	    return (-1);
	  }
	  not_full_flag = read_bytes_from_memory (INDEX_BLOCK_FLAG_SIZE,
						  index_block_header);
#ifdef LITERAL
	  number_of_valid_entries = read_bytes_from_memory (NEXT_INDEX_BLOCK_SIZE,
				index_block_header + INDEX_BLOCK_FLAG_SIZE);
#endif /* LITERAL */
#ifndef LITERAL
	  index_file_block_number = read_bytes_from_memory (NEXT_INDEX_BLOCK_SIZE,
				index_block_header + INDEX_BLOCK_FLAG_SIZE);
#endif /* not LITERAL */
	  index_block_size = read_bytes_from_memory (INDEX_BLOCK_SIZE_SIZE,
						     index_block_header +
			     INDEX_BLOCK_FLAG_SIZE + NEXT_INDEX_BLOCK_SIZE);
	} else {
	  not_full_flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, stream);
#ifdef LITERAL
	  number_of_valid_entries = read_bytes (NEXT_INDEX_BLOCK_SIZE, stream);
#endif /* LITERAL */
#ifndef LITERAL
	  index_file_block_number = read_bytes (NEXT_INDEX_BLOCK_SIZE, stream);
#endif /* not LITERAL */
	  index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, stream);
	}

/* Tung, 2/95 */
	if ((posting_list == NULL) && (index_block_size > 0)) {
	  posting_list = (unsigned char *) calloc ((size_t) (index_block_size - INDEX_BLOCK_HEADER_SIZE) * sizeof (char),
						                (size_t) 1);

	  current_index_block_size = index_block_size;
	} else if (index_block_size > current_index_block_size) {
	  s_free (posting_list);
	  posting_list = (unsigned char *) calloc ((size_t) (index_block_size - INDEX_BLOCK_HEADER_SIZE) * sizeof (char),
						                (size_t) 1);

	  current_index_block_size = index_block_size;
	}
	if (posting_list != NULL) {
	  if (0 > fread_from_stream (stream, posting_list,
			      index_block_size - INDEX_BLOCK_HEADER_SIZE)) {
	    waislog (WLOG_HIGH, WLOG_ERROR,
		     "Could not read the index block");
	    return (-1);
	  }
	}
	/*  Jim's debug code commented out
	   printf("flag = %d, block_num = %d, block_size = %d\n",
	   not_full_flag, 
	   index_file_block_number,
	   index_block_size);
  	   fflush (stdout);
	 */

	if (EOF == index_block_size) {
	  waislog (WLOG_HIGH, WLOG_ERROR,
		   "reading from the index file failed");
#ifdef BOOLEANS
	  gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	  return (-1);
	}
	if (not_full_flag == INDEX_BLOCK_NOT_FULL_FLAG) {
	  /* not full */
#ifdef LITERAL
	  number_of_valid_entries = 0;
#endif /* LITERAL */
#ifndef LITERAL
	  number_of_valid_entries = index_file_block_number;
#endif /* not LITERAL */
	} else if (not_full_flag == INDEX_BLOCK_FULL_FLAG) {
	  /* full */
#ifndef LITERAL
	  number_of_valid_entries = index_block_size - INDEX_BLOCK_HEADER_SIZE;
#endif /* not LITERAL */
	} else {		/* bad news, file is corrupted. */
	  waislog (WLOG_HIGH, WLOG_ERROR,
	   "Expected the flag in the inverted file to be valid.  it is %ld",
		   not_full_flag);
#ifdef BOOLEANS
	  gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	  return (-1);
	}
	/* printf("number of valid bytes: %ld\n", number_of_valid_entries); */

	/* add the array to the document_score_array */
#ifndef LITERAL
	number_of_valid_entries /= INDEX_ELEMENT_SIZE;
#endif /* not LITERAL */

/* tung, 10/93 */
	if ((number_of_operands > 1) && (search_result_array != NULL)) {
	  if (!make_doc_ids_array (operand_id, number_of_valid_entries))
	    return (-1);
	}
/* tung, 10/93 */

#ifdef NEW_WEIGHT
	query_wgt = 1;
	idf = log ((double) db->doc_table_allocated_entries /
		   (double) number_of_valid_entries);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	/* ses - idf is a fist approximation to the inverse document freq. */
	/* what it actually is  is the inverse occurance frequency which says
	 * that the significance of a word is inversly proportional to the number
	 * of times it occurs in the database */

	idf = 1.0 / number_of_occurances;
#endif /* not NEW_WEIGHT */
	for (count = 0; count < number_of_valid_entries; count++) {
	  int             wgt = 0;
	  int             did;

	  /*
	     if(count%1024 == 0) {
	     read(fileno(stream),index_buffer,INDEX_ELEMENT_SIZE*
	     MINIMUM(1024,number_of_valid_entries-count));
	     i=index_buffer;
	     }
	   */

	  if (posting_list == NULL) {

	    posting_list_pos = ftell (stream);

#ifdef LITERAL
	    did = read_bytes (DOCUMENT_ID_SIZE, stream);
	    char_list_size = read_bytes (NUMBER_OF_OCCURANCES_SIZE, stream);
#ifdef NEW_WEIGHT
	    internal_weight = read_weight_from_stream (NEW_WEIGHT_SIZE, stream);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	    wgt = read_bytes (WEIGHT_SIZE, stream);
#endif /* not NEW_WEIGHT */
#endif /* LITERAL */
#ifndef LITERAL
	    did = read_bytes (DOCUMENT_ID_SIZE, stream);
	    (void) read_bytes (WORD_POSITION_SIZE, stream);
	    read_bytes (CHARACTER_POSITION_SIZE, stream);
	    wgt = read_bytes (WEIGHT_SIZE, stream);
#ifdef NEW_WEIGHT
	    internal_weight = read_weight_from_stream (NEW_WEIGHT_SIZE, stream);

#endif /* NEW_WEIGHT */
#endif /* not LITERAL */
	  } else {
#ifdef LITERAL
	    did = read_bytes_from_memory (DOCUMENT_ID_SIZE,
					  posting_list + posting_list_pos);
#ifdef LITTLEENDIAN
	    char_list_size = htonl(read_bytes_from_memory (NUMBER_OF_OCCURANCES_SIZE,
						     posting_list +
				     (posting_list_pos + DOCUMENT_ID_SIZE)));
#else
	    char_list_size = read_bytes_from_memory (NUMBER_OF_OCCURANCES_SIZE,
						     posting_list +
				     (posting_list_pos + DOCUMENT_ID_SIZE));
#endif
#ifdef NEW_WEIGHT
	    internal_weight = read_weight_from_memory (NEW_WEIGHT_SIZE,
						       posting_list +
						       (posting_list_pos +
							DOCUMENT_ID_SIZE +
						NUMBER_OF_OCCURANCES_SIZE));
#else /* NEW_WEIGHT */
	    internal_weight = read_bytes_from_memory (WEIGHT_SIZE,
						      posting_list +
						      (posting_list_pos +
						       DOCUMENT_ID_SIZE +
						NUMBER_OF_OCCURANCES_SIZE));
#endif /* not NEW_WEIGHT */
#endif /* LITERAL */
#ifndef PROXIMITY
#ifndef LITERAL
	    did = read_bytes_from_memory (DOCUMENT_ID_SIZE,
					  posting_list + posting_list_pos);
	    read_bytes_from_memory (CHARACTER_POSITION_SIZE,
				    posting_list +
				    (posting_list_pos +
				     DOCUMENT_ID_SIZE));
#ifdef NEW_WEIGHT
	    internal_weight = read_weight_from_memory (NEW_WEIGHT_SIZE,
						       posting_list +
						       (posting_list_pos +
							DOCUMENT_ID_SIZE +
						  CHARACTER_POSITION_SIZE));
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	    wgt = read_bytes_from_memory (WEIGHT_SIZE,
					  posting_list +
					  (posting_list_pos +
					   DOCUMENT_ID_SIZE +
					   CHARACTER_POSITION_SIZE));
#endif /* not NEW_WEIGHT */
#endif /* not LITERAL */
	  }

	  /*
	     ASSIGN(wgt,WEIGHT_SIZE,    
	     i+DOCUMENT_ID_SIZE+WORD_POSITION_SIZE+CHARACTER_POSITION_SIZE,
	     INDEX_ELEMENT_SIZE,
	     DOCUMENT_ID_SIZE+WORD_POSITION_SIZE+CHARACTER_POSITION_SIZE);
	     ASSIGN(did,DOCUMENT_ID_SIZE,i,INDEX_ELEMENT_SIZE,0);
	   */

/* Commented out as suggested by Stan Isaacs at hp.com to come up with correct
 * weights when there are multiple documents in a file
 *
 *      if(wgt>5L)
 *              wgt-=5L;
 */
#endif /* PROXIMITY */
#ifndef PROXIMITY
#ifndef NEW_WEIGHT
	  internal_weight = log ((double) wgt);
	  internal_weight += 10.0;
#endif /* not NEW_WEIGHT */
	  internal_document_id = did;
	  if ((doc_start == 0) && (doc_end == 0))	/* tung, 5/94 */
	    doc_start = doc_end = did;	/* tung, 5/94 */
	  doc_start = MINIMUM (doc_start, did);		/* tung, 5/94 */
	  doc_end = MAXIMUM (doc_end, did);	/* tung, 5/94 */

	  /*
	     printf("entry %ld, Doc_id: %ld, weight %lf \n",
	     count, internal_document_id, internal_weight);
	     fflush(stdout);
	   */
	  if (EOF == wgt) {
	    waislog (WLOG_HIGH, WLOG_ERROR,
		     "reading from the doc-id table failed");
#ifdef BOOLEANS
	    gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	    return (-1);
	  }
#ifdef LITERAL
	  if ((weight == LITERAL_FLAG) && (0 == doc_id)) {	/* 2/92 GS TLG */
	    if (true == read_document_table_entry (&doc_entry,
						internal_document_id, db)) {	/* 2/92 GS TLG */
	      read_filename_table_entry (doc_entry.filename_id,
					 txt_filename, txt_type, NULL, db);	/* 2/92 GS TLG */
	      /* printf("search_word: document is [%s]\n", txt_filename); */
	      if (NULL == txt_stream) {
		/* francois */
		if (probe_file (txt_filename)) {
		  txt_stream = s_fopen (txt_filename, "rb");
		} else if (probe_file_possibly_compressed (txt_filename)) {
		  temp_txt_filename = s_fzcat (txt_filename);
		  if (temp_txt_filename) {
		    txt_stream = s_fopen (temp_txt_filename, "rb");
		  }
		}
		strcpy (prev_txt_filename, txt_filename);
	      } else if (0 != strcmp (txt_filename, prev_txt_filename)) {
		s_fclose (txt_stream);
		/* francois */
		if (temp_txt_filename != NULL) {
		  unlink (temp_txt_filename);
		  s_free (temp_txt_filename);
		}
		if (probe_file (txt_filename)) {
		  txt_stream = s_fopen (txt_filename, "rb");
		} else if (probe_file_possibly_compressed (txt_filename)) {
		  temp_txt_filename = s_fzcat (txt_filename);
		  if (temp_txt_filename) {
		    txt_stream = s_fopen (temp_txt_filename, "rb");
		  }
		}
		strcpy (prev_txt_filename, txt_filename);	/* 2/92 GS TLG */
	      }
	      while ((char_list_size > char_list_size_readed) && (literal_match == false)) {
#endif /* LITERAL */
#else /* PROXIMITY */
/* You can't tell how many entries are in the character list based on the
 * char_list_size, since each entry is compressed and can take from one to
 * sizeof(int) bytes (the name "NUMBER_OF_OCCURANCES_SIZE" is misleading).
 * However, assuming that each entry takes up one byte
 * gives an upper bound on the size--better allocate a few bytes too many
 * than a few bytes too few.  TJS, 4/24/95
 */
	  if ((number_of_operands > 1) && (search_result_array != NULL)) {
	    search_result_array[operand_id].doc_ids_array[count].word_position
	      = s_malloc ((char_list_size - CHARACTER_POSITION_SIZE + 1)
			  * sizeof (long));
	  }
#endif /* PROXIMITY */
#ifdef PROXIMITY
#ifdef LITERAL
/* Now we will read the word position information for each occurrence of
 * the word.  This is necessary for the proximity code.
 */
	  while (char_list_size > char_list_size_readed) {
#endif /* LITERAL */
#endif /* PROXIMITY */
#ifdef LITERAL
	    if (posting_list == NULL) {
	      if (first_char_pos_readed == false) {
		first_char_pos_readed = true;
		txt_pos = read_bytes (CHARACTER_POSITION_SIZE, stream);
#ifdef PROXIMITY
		if ((number_of_operands > 1) && (search_result_array != NULL)) {
		  search_result_array[operand_id].doc_ids_array[count].word_position[0] = txt_pos;
		}
#endif /* PROXIMITY */
		first_txt_pos = txt_pos;
		char_list_size_readed += CHARACTER_POSITION_SIZE;
	      } else {
		if (char_list == NULL) {
		  char_list = (unsigned char *) calloc ((size_t) ((char_list_size - CHARACTER_POSITION_SIZE) * sizeof (char)),
						                (size_t) 1);

		  if (char_list == NULL) {
		    waislog (WLOG_HIGH, WLOG_ERROR, "Out of memory");
		    return (-1);
		  }
		  if (0 > fread_from_stream (stream, char_list,
				char_list_size - CHARACTER_POSITION_SIZE)) {
		    waislog (WLOG_HIGH, WLOG_ERROR,
			     "Could not read the index block");
		    return (-1);
		  }
		  tmp_char_list = char_list;
		}
		prev_char_list = tmp_char_list;
		tmp_char_list = (unsigned char *)
		  readCompressedInteger (&distance, tmp_char_list);
		txt_pos = first_txt_pos + prev_distance + distance;
		prev_distance += distance;
		char_list_size_readed += tmp_char_list - prev_char_list;
#ifdef PROXIMITY
		if ((number_of_operands > 1) && (search_result_array != NULL)) {
		  search_result_array[operand_id].doc_ids_array[count].word_position[search_result_array[operand_id].doc_ids_array[count].numwords++] = txt_pos;
		}
#endif /* PROXIMITY */
	      }
	    } else {
	      if (first_char_pos_readed == false) {
		first_char_pos_readed = true;
#ifdef NEW_WEIGHT
		txt_pos = read_bytes_from_memory (CHARACTER_POSITION_SIZE,
						  posting_list +
						  (posting_list_pos +
						   DOCUMENT_ID_SIZE +
						 NUMBER_OF_OCCURANCES_SIZE +
						   NEW_WEIGHT_SIZE));
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
		txt_pos = read_bytes_from_memory (CHARACTER_POSITION_SIZE,
						  posting_list +
						  (posting_list_pos +
						   DOCUMENT_ID_SIZE +
						 NUMBER_OF_OCCURANCES_SIZE +
						   WEIGHT_SIZE));
#endif /* not NEW_WEIGHT */
		first_txt_pos = txt_pos;
		char_list_size_readed += CHARACTER_POSITION_SIZE;
#ifdef PROXIMITY
		if ((number_of_operands > 1) && (search_result_array != NULL)) {
		  search_result_array[operand_id].doc_ids_array[count].word_position[search_result_array[operand_id].doc_ids_array[count].numwords++] = txt_pos;
		}
#endif /* PROXIMITY */
	      } else {
#ifdef NEW_WEIGHT
		tmp_char_list = posting_list + (posting_list_pos +
						DOCUMENT_ID_SIZE +
						NUMBER_OF_OCCURANCES_SIZE +
						NEW_WEIGHT_SIZE +
						char_list_size_readed);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
		tmp_char_list = posting_list + (posting_list_pos +
						DOCUMENT_ID_SIZE +
						NUMBER_OF_OCCURANCES_SIZE +
						WEIGHT_SIZE +
						char_list_size_readed);
#endif /* not NEW_WEIGHT */
		prev_char_list = tmp_char_list;
		tmp_char_list = (unsigned char *)
		  readCompressedInteger (&distance, tmp_char_list);
		txt_pos = first_txt_pos + prev_distance + distance;
		prev_distance += distance;
		char_list_size_readed += tmp_char_list - prev_char_list;
#ifndef PROXIMITY
	      }
	    }
	    txt_pos += doc_entry.start_character - txt_pos_fix;
	    if (txt_pos >= doc_entry.start_character) {
	      s_fseek (txt_stream, txt_pos, SEEK_SET);	/* 2/92 GS TLG */
	      FreadStatus=fread_from_stream(txt_stream, document_line, phraselen);
              if ((0 > FreadStatus) && (FreadStatus != -2)) {
		waislog (WLOG_HIGH, WLOG_ERROR,
			 "Could not read the phrase");
		return (-1);
	      }
	      document_line[phraselen] = '\0';
	      while ((tmpindex = index (document_line, '\r')) ||
		     (tmpindex = index (document_line, '\n'))) {
		long            newlinepos = phraselen - strlen (tmpindex);

		document_line[newlinepos] = ' ';
	      }
	      string_downcase (document_line);

	      if (strcmp (document_line, word) == 0) {
		literal_match = true;
		break;
	      } else {
		literal_match = false;
#else /* PROXIMITY */
		if ((number_of_operands > 1) && (search_result_array != NULL)) {
		  search_result_array[operand_id].doc_ids_array[count].word_position[search_result_array[operand_id].doc_ids_array[count].numwords++] = txt_pos;
		}
#endif /* PROXIMITY */
	      }
	    }
	  }
	  char_list_size_readed = 0;
	  first_char_pos_readed = false;
#ifndef PROXIMITY
	  if (literal_match == false)
	    internal_weight = 0.0;
	  literal_match = false;
	  first_txt_pos = prev_distance = distance = 0;
	}
      }
#else /* PROXIMITY */
	  first_txt_pos = prev_distance = distance = 0;
#endif /* PROXIMITY */
#endif /* LITERAL */
#ifdef PROXIMITY
#ifndef LITERAL
	  did = read_bytes_from_memory (DOCUMENT_ID_SIZE,
					posting_list + posting_list_pos);
	  read_bytes_from_memory (CHARACTER_POSITION_SIZE,
				  posting_list +
				  (posting_list_pos +
				   DOCUMENT_ID_SIZE));
#ifdef NEW_WEIGHT
	  internal_weight = read_weight_from_memory (NEW_WEIGHT_SIZE,
						     posting_list +
						     (posting_list_pos +
						      DOCUMENT_ID_SIZE +
						  CHARACTER_POSITION_SIZE));
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	  wgt = read_bytes_from_memory (WEIGHT_SIZE,
					posting_list +
					(posting_list_pos +
					 DOCUMENT_ID_SIZE +
					 CHARACTER_POSITION_SIZE));
#endif /* not NEW_WEIGHT */
#endif /* not LITERAL */
	}

	/*
	   ASSIGN(wgt,WEIGHT_SIZE,    
	   i+DOCUMENT_ID_SIZE+WORD_POSITION_SIZE+CHARACTER_POSITION_SIZE,
	   INDEX_ELEMENT_SIZE,
	   DOCUMENT_ID_SIZE+WORD_POSITION_SIZE+CHARACTER_POSITION_SIZE);
	   ASSIGN(did,DOCUMENT_ID_SIZE,i,INDEX_ELEMENT_SIZE,0);
	 */

/* Commented out as suggested by Stan Isaacs at hp.com to come up with correct
 * weights when there are multiple documents in a file
 *
 *      if(wgt>5L)
 *              wgt-=5L;
 */
#ifndef NEW_WEIGHT
	internal_weight = log ((double) wgt);
	internal_weight += 10.0;
#endif /* not NEW_WEIGHT */
	internal_document_id = did;
	if ((doc_start == 0) && (doc_end == 0))		/* tung, 5/94 */
	  doc_start = doc_end = did;	/* tung, 5/94 */
	doc_start = MINIMUM (doc_start, did);	/* tung, 5/94 */
	doc_end = MAXIMUM (doc_end, did);	/* tung, 5/94 */

	/*
	   printf("entry %ld, Doc_id: %ld, weight %lf \n",
	   count, internal_document_id, internal_weight);
	   fflush(stdout);
	 */
	if (EOF == wgt) {
	  waislog (WLOG_HIGH, WLOG_ERROR,
		   "reading from the doc-id table failed");
#ifdef BOOLEANS
	  gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	  return (-1);
	}
#endif /* PROXIMITY */

	{
	  /* if(doc_id > 0) we are doing a relevant document */
	  /*
	     printf("wgt: %ld, internal weight: %lf, idf: %lf occurances: 
	     %ld\n", wgt,internal_weight, idf,number_of_occurances);
	     fflush(stdout);
	   */

            if (weight == NUMERIC_FLAG) {
#ifdef FANCY_NUMERIC_WEIGHTING
                /* This should be a nummeric term */
                if ((word[0] == GREATER) && (matchword != NULL)) {
                    double qv, av;
                    qv = atof(word+1);
                    av = atof(matchword);
                    internal_weight = (av - qv)/MAXIMUM(abs(qv),abs(av));
                } else  if ((word[0] == LESS) && (matchword != NULL)) {
                    double qv, av;
                    qv = atof(word+1);
                    av = atof(matchword);
                    internal_weight = (qv - av)/MAXIMUM(abs(qv),abs(av));
                } else {
                }
/*
                fprintf(stderr, "internal_weight=%lf %s %s\n",
                (double) internal_weight, word, matchword);
                */
#endif
            } else {
                /* ses - for inverse doc. freq. */
                internal_weight *= idf;	
            }
            if (query_weight > 0) {
/*
                fprintf(stderr, "Reweighting: %lf %lf", internal_weight,
                        query_weight);
                        */
                internal_weight *= query_weight;
/*
                fprintf(stderr, "-> %lf\n", internal_weight);
                */
            }
/* tung, 10/93 */
	  if (number_of_operands == 1) {
#ifdef NEW_WEIGHT
	    document_score_array[internal_document_id] +=
	      (query_wgt * internal_weight);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	    document_score_array[internal_document_id] +=
	      (doc_id) ? (internal_weight * RF_WEIGHTING) : internal_weight;
#endif /* not NEW_WEIGHT */
	  } else {
#ifndef PROXIMITY
	    if ((number_of_operands > 1) && (search_result_array != NULL)) {
	      if (weight == LITERAL_FLAG) {
#ifdef NEW_WEIGHT
		((search_result_array[operand_id]).doc_ids_array[phrase_count]).score +=
		  (query_wgt * internal_weight);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
		((search_result_array[operand_id]).doc_ids_array[phrase_count]).score +=
		  (doc_id) ? (internal_weight * RF_WEIGHTING) : internal_weight;
#endif /* not NEW_WEIGHT */
		if (((search_result_array[operand_id]).doc_ids_array[phrase_count]).score > 0) {
		  ((search_result_array[operand_id]).doc_ids_array[phrase_count]).doc_id = internal_document_id;
		  phrase_count++;
		  search_result_array[operand_id].number_of_hits = phrase_count;
		}
	      } else {
		((search_result_array[operand_id]).doc_ids_array[count]).doc_id = internal_document_id;
#else /* PROXIMITY */
	    if ((number_of_operands > 1) && (search_result_array != NULL)) {
	      search_result_array[operand_id].doc_ids_array[count].doc_id = internal_document_id;
#endif /* PROXIMITY */
#ifdef NEW_WEIGHT
	      search_result_array[operand_id].doc_ids_array[count].score +=
		(query_wgt * internal_weight);
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
	      search_result_array[operand_id].doc_ids_array[count].score +=
		(doc_id) ? (internal_weight * RF_WEIGHTING) : internal_weight;
#endif /* not NEW_WEIGHT */
#ifndef PROXIMITY
	    }
#endif /* not PROXIMITY */
	  }
	}
/* tung, 10/93 */

      }
/*
   printf("Score array: %lf\n",document_score_array[internal_document_id]);
   fflush(stdout);
 */

      /* i+=INDEX_ELEMENT_SIZE;  Purify (umr): uninitialized memory read: (up) */

#ifdef LITERAL
#ifdef NEW_WEIGHT
      posting_list_pos += DOCUMENT_ID_SIZE + NUMBER_OF_OCCURANCES_SIZE +
	NEW_WEIGHT_SIZE + char_list_size;
#endif /* NEW_WEIGHT */
#ifndef NEW_WEIGHT
      posting_list_pos += DOCUMENT_ID_SIZE + NUMBER_OF_OCCURANCES_SIZE +
	WEIGHT_SIZE + char_list_size;
#endif /* not NEW_WEIGHT */
      if (posting_list == NULL)
	s_fseek (stream, posting_list_pos, SEEK_SET);
#endif /* LITERAL */
    }
#ifdef LITERAL
    posting_list_pos = 0;
    index_file_block_number = 0;
#endif /* LITERAL */
  }
#ifdef PARTIALWORD
  index_file_block_number =
    look_up_partialword_in_dictionary (field_name,
				       NULL, &number_of_occurances, &matchword, db);
  /*
  if (matchword != NULL)
      fprintf(stderr, "look_up_partialword_in_dictionary:: %s\n", matchword);
  */
}
#endif /* PARTIALWORD */

/* tung, 10/93 */
if ((number_of_operands > 1) && (search_result_array != NULL)) {
  qsort (search_result_array[operand_id].doc_ids_array,
	 search_result_array[operand_id].number_of_hits,
	 sizeof (doc_descr_struct), compare_doc_ids);
  if (!save_operand_id (operand_id, search_result_array))
    return (-1);
  search_result_array[operand_id].operand_id = operand_id;
  ++operand_id;
}
/* tung, 10/93 */

#ifdef BOOLEANS
gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
return (0);
	}

	else
	if (0 == index_file_block_number) {
	  /* an error occurred on looking up the word */
#ifdef BOOLEANS
	  gLastNot = gLastAnd = false;
#endif /* BOOLEANS */
	  return (-1);
	} else {		/* index_file_block_number is negative */
	  if ((number_of_operands > 1) && (search_result_array != NULL)) {
	    if (!save_operand_id (operand_id, search_result_array))
	      return (-1);
	    search_result_array[operand_id].operand_id = operand_id;
	    search_result_array[operand_id].number_of_hits = 0;
	    ++operand_id;
	  }
	  return (0);		/* word not present */
	}
	}

/* now collect the best hits */
long 
finished_search_word (db)
     database       *db;
{
  long            number_of_hits;	    /* tung, 10/93 */

#ifdef LITERAL
  if (posting_list != NULL)
    s_free (posting_list);
  if (index_block_header != NULL)
    s_free (index_block_header);
  current_index_block_size = 0;
#endif /* LITERAL */

#ifdef BOOL
  if (currentQuery != NULL)
    return;			/* do nothing for boolean */
#endif /* BOOL */

/* tung, 10/93 */
  if ((number_of_operands > 1) && (search_result_array != NULL)) {
    number_of_hits = retriev_result (search_result_array, document_score_array);
    clear_search_result_array (&number_of_operands);
  }
/* tung, 10/93 */

  /* check the document_score_array */
  if (document_score_array_len < db->doc_table_allocated_entries)
    make_document_score_array (db->doc_table_allocated_entries);

  make_best_hits_array (max_hit_retrieved);
  sort_best_hits (db);
  syn_Free (db->syn_Table, &db->syn_Table_Size);

  return (0);
}
