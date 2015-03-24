/*                               -*- Mode: C -*- 

 * irsearch.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Brewster@think.com
 * Created On      : Tue May 23 14:08:30 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Dec  6 11:47:54 1995
 * Language        : C
 * Update Count    : 23
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * HISTORY
 * 
 * $$
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.    
   Brewster@think.com
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/* Looks up words in the inverted file index.

 * Important functions:
 * run_search
 * search_for_words
 *
 * to do:
 *    Handle searches on multiple databases
 */

/* Change Log:
 * $Log: irsearch.c,v $
 * Revision 2.0.1.4  1995/12/06 11:01:53  pfeifer
 * patch53: Change float parameter of search_word() and
 * patch53: search_word_before_pairs() to double.  (Patch from Rodney
 * patch53: Barnett).
 *
 * Revision 2.0.1.3  1995/11/03  12:36:49  pfeifer
 * patch45: Removed prototype for search_word_before_pairs since it makes
 * patch45: the AIX compiler unhappy.
 *
 * Revision 2.0.1.2  1995/10/10  18:29:52  pfeifer
 * patch33: Handling for query term weighting with '<- 99.99' as prefix
 * patch33: operator.
 *
 * Revision 2.0.1.2  1995/10/10  18:29:52  pfeifer
 * patch33: Handling for query term weighting with '<- 99.99' as prefix
 * patch33: operator.
 *
 * Revision 2.0.1.1  1995/09/15  09:45:29  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:00:15  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.27  1995/06/23  12:21:26  pfeifer
 * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS | indent
 *
 * Revision 1.26  1995/05/23  12:09:43  pfeifer
 * Removed teh code for soundex and phonix searches. Interpretation of
 * prefix operators now is in query_y.y.
 *
 * Revision 1.25  1995/05/07  16:04:57  pfeifer
 * More fixes by Tom Snee. Literal serach now works again!
 *
 * Revision 1.24  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.23  1995/04/17  13:17:56  huynh1
 * Release 1.1.2
 *
 * Revision 1.22  1994/12/22  13:59:37  pfeifer
 * typo
 *
 * Revision 1.21  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.20  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.19  1994/08/23  12:33:12  pfeifer
 * moved the HEADLINE constants to Defaults.tmpl
 *
 * Revision 1.18  1994/08/05  07:12:19  pfeifer
 * Release beta 04
 *
 * Revision 1.17  1994/07/22  12:30:05  huynh1
 * left for holidays
 *
 * Revision 1.16  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.15  1994/06/22  07:29:19  pfeifer
 * Do not normalize weights
 *
 * Revision 1.14  1994/05/19  12:53:48  huynh1
 * relevance feedback updated for fields.
 *
 * Revision 1.13  1994/05/18  16:44:02  huynh1
 * patchlevel 09
 *
 * Revision 1.11  1994/04/06  23:52:04  huynh1
 * 08, autoconf, Uli
 *
 * Revision 1.10  1994/03/23  15:30:58  pfeifer
 * fixed iso code
 *
 * Revision 1.9  1994/03/23  12:59:57  huynh1
 * calling openDatabase modified.
 * insert open_global_Database.
 * patchlevel 07.
 *
 * Revision 1.8  1994/03/08  20:43:18  huynh1
 * Patchlevel 04
 *
 * Revision 1.7  1994/02/27  15:57:32  huynh1
 * *** empty log message ***
 *
 * Revision 1.6  1994/02/14  10:32:21  huynh1
 * new code for field concept added.
 *
 * Revision 1.5  1993/10/17  15:38:50  huynh1
 * new code added for soundex, phonix retrieval and
 * nested boolean queries.
 *
 * Revision 1.3  93/07/21  18:46:35  warnock
 * Added STELAR-specific patches
 * 
 * Revision 1.2  93/07/02  18:04:26  warnock
 * replace handle_next_and_previous for multi-type from francois
 * 
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.54  92/05/10  14:44:35  jonathan
 * Made a little safer on NULL docid's when parsing.
 * 
 * Revision 1.53  92/05/04  17:20:11  jonathan
 * Added test for parsing docids (if null, log error).
 * 
 * Revision 1.52  92/04/29  08:22:17  shen
 * declare global variable "_BE_normalized" to allow turning on/off FE score
 * normalization.
 * 
 * Revision 1.51  92/04/28  16:56:30  morris
 * added boolean to serial engine
 * 
 * Revision 1.50  92/04/01  17:10:21  jonathan
 * ?
 * 
 * Revision 1.49  92/03/23  13:26:27  shen
 * add timing for query. Compile with GET_QUERY_TIMING. print timing every 200 queries.
 * 
 * Revision 1.48  92/03/18  08:56:00  jonathan
 * Removed databaseName argument to getDocumentText and getData.
 * 
 * Revision 1.47  92/02/17  16:22:42  jonathan
 * Added WCAT to types that can be used for relevance feedback.
 * 
 * Revision 1.46  92/02/16  18:04:38  jonathan
 * Demoted more WLOG_ERROR's to WLOG_WARNING's
 * 
 * Revision 1.45  92/02/16  09:51:12  jonathan
 * Plugged some memory leaks.  I be there are more.
 * 
 * Revision 1.44  92/02/15  19:41:20  jonathan
 * Improved logging for invalid relevant documents.
 * 
 * Revision 1.43  92/02/14  16:06:48  jonathan
 * Added diagnostic record for invalid relevant document.
 * 
 * Revision 1.42  92/02/12  17:30:20  jonathan
 * Conditionalized inclusion of object code.
 * 
 * Revision 1.41  92/02/12  17:04:03  jonathan
 * Moved logging info around.
 * 
 * Revision 1.40  92/02/12  15:26:35  morris
 * only call fnished_search_word when the preceeding search was successful
 * 
 * Revision 1.39  92/02/12  13:30:39  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * changes 5.2.90 HWM
 - changed calls to perror() to calls to panic()
 - made print_best_hits() only print hits w/ non-zero weight
 - made random arrays static instead of reading them in.  
 removed getRandomArray.
 - removed unused variables
 Brewster 7/90 made look_up_word_in_dictionary safer.
 Brewster 7/90 elimiated trailing <lf> on filename and headline table accesses
 HWM 7.12.90 - replaced all calls to panic with error code returns and a log
 file  
 - added the routine initSearchEngine() which should be called 
 before any other search routine
 - added beFriendly() to give other processes time under 
 multifinder
 JG 5.31.91 - added relevance feedback for line fragments.
 JG 7.8.91  - added doc_id to search_for_words, removed scale_scores.
 */

#if 0
#define GET_QUERY_TIMING
#endif /* 0 */

#define _search_c

/* #include <string.h>  /* for strlen() */
#ifdef THINK_C
#include <unix.h>		/* for sleep() */
#endif /* THINK_C */

#include "cutil.h"
#include "irfiles.h"
#include "irtfiles.h"		/* for map_over_words */
#include "irext.h"
#include "irsearch.h"
#include "docid.h"
#include <math.h>
#include "irretrvl.h"
#ifdef BOOL
#include "irparse.h"
#endif /* BOOL */

#include "analyse_str.h"

#include "field_search.h"

#include "trie.h"

#define TEST_SEARCH 	false	/* set to TRUE to allow printing to console */

#ifdef STELAR
/* File of associations between STELAR abstracts and bitmaps */
FILE           *BITMAPS;

#endif /* STELAR */

char           *server_name = NULL;
long            tcp_port = 0;

long            _BE_normalized = 1;

#ifdef GET_QUERY_TIMING
/*
   #include <sys/timeb.h>
   static struct timeb  s_time, e_time;
   static float t_time = 0;
   static long n_query = 0;
 */
#include <sys/types.h>
#include <sys/times.h>
static struct tms s_time, e_time;
static float    t_time = 0;
static long     n_query = 0;

#endif /* GET_QUERY_TIMING */

/*----------------------------------------------------------------------*/

static Boolean calcDocLength _AP ((hit * theHit, long *lines, long *bytes));

static          Boolean
calcDocLength (theHit, lines, bytes)
     hit            *theHit;
     long           *lines;
     long           *bytes;

/* Given a hit, open the file and figure out how many bytes and lines
   it contains.  This is not needed by the serial search engine (it
   stores these values in its dictionary.  It is used by the dynamic
   help facility).
 */
{
  *lines = theHit->number_of_lines;

  /* find the length of the document */
  if (theHit->end_character != 0) {
    /* document is not whole file, so size is stored */
    *bytes = theHit->end_character - theHit->start_character;
    return (true);
  } else {
    /* whole file, find file length from the file */
    FILE           *file = NULL;

    if (((file = s_fopen (theHit->filename, "r")) != NULL) &&
	(s_fseek (file, 0L, SEEK_END) == 0) &&
	((*bytes = ftell (file)) != -1)) {
      s_fclose (file);
      return (true);		/* we are done, bytes is set */
    } else {
      s_fclose (file);
      return (false);		/* something went wrong with the file */
    }
  }
}

/* Some compilers (AIX @#$!) dont like it declared */

static long search_word_before_pairs _AP ((char *word,
					   char *field_name,
					   long char_pos,
					   long line_pos, long weight, double query_weight,
					   long doc_id, time_t doc_date,
                                           boolean capitalized, database * db));
/* returns 0 is successful, non-0 if error.  A copy of add_word_before_pairs */
static long 
search_word_before_pairs (word,
			  field_name,
			  char_pos, line_pos,
			  weight, query_weight, doc_id, doc_date, capitalized, db)
     char           *word;		    /* the word to be indexed, this could be a

					       word pair. If NULL there are no more words
					       to be indexed */
     char           *field_name;
     long            char_pos;		    /* the position of the start of the

					       word */
     long            line_pos;		    /* this is passed for the best

					       section calculation */
     long            weight;		    /* Tung missused this for different flags */
     double          query_weight;	    /* how important the word looks
					       syntactically (such as is it bold)
					       NOT used by signature system */
     long            doc_id;		    /* current document, this will never be 0 */
     time_t          doc_date;		    /* display day of this document, 0 if not known */
     boolean         capitalized;	    /* if the word started with a cap */
     database       *db;		    /* database to insert the document */
{
  static char     last_word[MAX_WORD_LENGTH + 1];
  static long     last_doc_id = -1;

  /* The way it works is it remembers if the last word if it was
     capitalized (if not it clears the saved word).  
     If another capitalized word comes along next
     (and it is in the same document), then it makes a joint word and calls 
     add_word with it. */
  if (capitalized) {
    if (last_word[0] != '\0' && last_doc_id == doc_id) {
      search_word (make_joint_word (last_word, word),
		   field_name,
		   char_pos, line_pos, weight, query_weight, doc_id, 1L, db);
    } else {
      last_word[0] = '\0';
    }
    strncpy (last_word, word, MAX_WORD_LENGTH);
    last_doc_id = doc_id;
  } else {			/* not capitalized */
    last_word[0] = '\0';
  }
  return (search_word (word,
		       field_name,
		       char_pos,
		       line_pos, weight, query_weight, doc_id, 0L, db));
}

long            count_trie_words;
long            count_uniq;

boolean 
prepare_word_list (words, set, alloc)
     char           *words;
     trie           *set;
     trie_allocator *alloc;
{
  char           *word = NULL;
  int            *datum;

  count_trie_words = count_uniq = 0;
  /* printf("words: %s\n", words); */
#ifdef BIO
  word = (char *) strtokf (words, wordDelimiter);
#else /* not BIO */
  word = (char *) strtokf_isalnum (words);
#endif /* not BIO */
  while (word != NULL) {
    long            dictionary_value;

    /* trim the string if necessary */
    if (strlen (word) > MAX_WORD_LENGTH) {
      word[MAX_WORD_LENGTH] = '\0';
    }
    if (!encode ((unsigned char *) word)) {
      panic ("can't encode word %s", word);
    }
    datum = (int *) trie_lookup (word, set, alloc);
    if (!datum) {
      panic ("trie_lookup failed !!!");
    }
    count_trie_words++;

    *datum += 1;

    if (*datum == 1) {
      count_uniq++;
    }
#ifdef BIO
    word = (char *) strtokf (NULL, wordDelimiter);
#else /* not BIO */
    word = (char *) strtokf_isalnum (NULL);
#endif /* not BIO */
    beFriendly ();
  }

  waislog (WLOG_LOW, WLOG_INFO,
	   "after preparing word list, %d search items were presented.",
	   count_trie_words);
  waislog (WLOG_LOW, WLOG_INFO,
	   "There are %d words to search for.",
	   count_uniq);

  return (true);
}

boolean 
search_for_trie_words (dict, db, prefix, docid, result, field_name)
     trie           *dict;
     database       *db;
     char           *prefix;
     long            docid;
     boolean         result;
     char           *field_name;
{
  char            buffer[MAX_WORD_LENGTH + 1];
  char            tmp_word[MAX_WORD_LENGTH + 1];
  char           *word;
  long            dictionary_value;
  int             weight;
  char           *tmp = word;

  if (dict == NULL) {
    return result;
  }
  if (*dict->string) {
    strcpy (buffer, prefix);
    strcat (buffer, dict->string);
    word = buffer;
  } else {
    word = prefix;
  }

  if (dict->datum) {
    long            number_of_occurrences;

    /* this node has data */
    strcpy (tmp_word, word);
    decode (tmp_word);
    if (db->number_of_fields > 0)
      result |= search_word (tmp_word, field_name, 0L, 0L, FIELD_FLAG, 0.0, docid, 0L, db);
    else
      result |= search_word (tmp_word, field_name, 0L, 0L, 1L, 0.0, docid, 0L, db);
  }
  if (dict->table) {
    int             i;
    int             len;

    len = strlen (word);
    for (i = 0; i < ALPHA_SIZE; i++) {
      if (dict->table[i]) {
	word[len] = (char) i;
	word[len + 1] = '\0';
	result = search_for_trie_words (dict->table[i], db, word, docid, result, field_name);
      }
    }
  }
  return result;
}

/* dgg -- pulled this from irtfiles.c:map_over_words */
/* returns the number of words added, or -1 if an error occurred */
long            search_over_words
                _AP ((char *line, long document_id, database * db));

long 
search_over_words (line, document_id, db)
     char           *line;
     long            document_id;
     database       *db;
{
  long            weight = 1L;
  long            file_position_before_line = 0;
  boolean         word_position = false;
  boolean         word_pairs = false;
#ifdef BIO
  int             minwordlen = 1;	    /* only if symbols are active ? */
#else /* not BIO */
  int             minwordlen = 2;
#endif /* not BIO */
  long            position_in_word = 0;
  long            word_count = 0;
  unsigned long   ch;
  long            char_count = 0;
  boolean         capitalized = false;	    /* if the word starts with a cap */
  char            word[MAX_FIELD_LENGTH + 1];
#ifdef LITERAL
  char            key;
#endif /* LITERAL */
#define MAX_LINE_LENGTH 1000
  boolean         nextIsNot = false;
  char            notwords[MAX_LINE_LENGTH + 1];
  char            field_name[MAX_WORD_LENGTH + 1];
  double          query_weight = 0;
  field_name[0] = '\0';
  notwords[0] = '\0';

  for (ch = (unsigned char) line[char_count++];
       ch != '\0';
       ch = (unsigned char) line[char_count++]) { /* Purify (abr):  (up) */
    boolean         alnum;

    if ((ch == '<') && (line[char_count] == '-')) {
        /* assignment operator for term weights */
        char *sweight;

        char_count++;
        while (line[char_count] == ' ') char_count++;
        sweight = line+char_count;
        while (isnumchar(line[char_count])) char_count++;
        /* line[char_count] = '\0';  */
        sscanf(sweight, "%lf", &query_weight);
        ch = line[char_count++]; /* reenter loop */
    }

#ifdef BIO
    alnum = (wordDelimiter (ch) == NOT_DELIMITER);
#else /* not BIO */
    alnum = isalnum (ch);
#endif /* not BIO */
#ifdef LITERAL
#ifndef PROXIMITY
    if (ch == LITERAL_KEY1)
      key = LITERAL_KEY1;
    else if (ch == LITERAL_KEY2)
      key = LITERAL_KEY2;
    else {
      key = 0;
      weight = 1L;
    }				/* by only key=0 is it not possible
				   combine literal search and boolean search
				 */
    if (key != 0) {
      char           *cp, *match;

      cp = line + char_count;
      match = strchr (cp, key);
      if (match != NULL && cp < match) {
	for (position_in_word = 0; cp < match; cp++, char_count++)
	  if (position_in_word < MAX_PHRASE_LENGTH) {
	    word[position_in_word++] = char_downcase ((unsigned long) *cp);
	  }
	char_count++;		/* skip closing key */
	alnum = false;
	capitalized = false;

	weight = LITERAL_FLAG;	/* is this a safe flag parameter? --
				   unused but passed on to search_word
				   is what we need */
	/* !! need to break literal "word" into 1st dictionary word
	   and search on that... */
      }
    }
#else /* PROXIMITY */
    weight = 1L;
#endif /* PROXIMITY */
#endif /* LITERAL */
#ifdef PARTIALWORD
    if (ch == PARTWORD_WILDCARD) {
      word[position_in_word++] = ch;
      ++char_count;
      alnum = false;
      minwordlen = MAXIMUM (2, minwordlen);
      weight = PARTIAL_FLAG;
    } else
#endif /* PARTIALWORD */
      if ((ch == EQ) ||
	  (ch == GREATER) ||
	  (ch == LESS)) {	/* it's field ? */
      if (position_in_word >= minwordlen) {	/* is it reasonable ? */
	long            field_id;

	word[position_in_word] = '\0';
	strncpy (field_name, word, MAX_WORD_LENGTH);
	field_name[position_in_word] = '\0';
	field_id = pick_up_field_id (field_name, db);
	if ((ch == EQ && line[char_count] == EQ) ||
	    ch == GREATER || ch == LESS) {
	  char           *cp;

	  if (ch == EQ && line[char_count] == EQ)
	    char_count += 2;
	  else
	    ++char_count;
	  cp = line + char_count;
	  position_in_word = 0;
	  word[position_in_word++] = (unsigned long) ch;
	  for (position_in_word = 1; (*cp != '\0') && !isspace (*cp); cp++, char_count++) {
	    word[position_in_word++] = char_downcase ((unsigned long) *cp);
	  }
	  if (*cp != '\0')
	    char_count++;
	  alnum = false;
	  capitalized = false;
	  weight = NUMERIC_FLAG;
	} else {
	  position_in_word = 0;
	  char_count++;
	  alnum = false;
	  capitalized = false;
	  weight = FIELD_FLAG;
	}
      }
    }
    if (alnum) {
      /* put the character in the word if not too long */
      if (position_in_word == 0)
	capitalized = isupper ((unsigned long) ch) ? true : false;
      if (position_in_word < MAX_WORD_LENGTH) {
	word[position_in_word++] = char_downcase ((unsigned long) ch);
      }
    } else {			/* not an in a word */
      if (position_in_word != 0) {
/* note on BOOLEANS -- we really need to check for NOT words here,
   and move them to back of line so that (wordfunction)== search_word is
   called for NOT words after other words (excluding NOT inside a literal)
 */
	if (nextIsNot) {
	  word[position_in_word] = '\0';
	  strcat (notwords, word);
	  strcat (notwords, " ");
	  nextIsNot = false;
	  word_count++;
	} else if ((strncmp (word, BOOLEAN_NOT, position_in_word) == 0)) {
	  word[position_in_word] = '\0';
	  if (0 != search_word_before_pairs
	      (word,
	       field_name,
	       file_position_before_line + char_count,
	  /*^^  this param is supposed to be start-of-word, but char_count is now at end-of-word ! */
	       0L,		/* line_pos */
	       weight,
               query_weight,
	       document_id,
	       (time_t) 0L,
	       capitalized,
	       db)) 
	    return (-1);	/* error */
          query_weight = 0;
	  nextIsNot = false;
	  word_count++;
	} else
	  /* then we have collected a word */
	if (position_in_word >= minwordlen) {	/* is it reasonable ? */
	  word[position_in_word] = '\0';
	  if (0 != search_word_before_pairs (word,
					     field_name,
				     file_position_before_line + char_count,
	  /*^^  this param is supposed to be start-of-word, but char_count is now at end-of-word ! */
					     0L,	/* line_pos */
					     weight,
                                             query_weight,
					     document_id,
					     (time_t) 0L,
					     capitalized,
					     db))
	    return (-1);	/* error */
	  nextIsNot = false;
          query_weight = 0;
	  word_count++;
	}
	position_in_word = 0;
      }
    }
  }

  /* finish last word */
  if (position_in_word >= minwordlen) {		/* is it reasonable ? */
    word[position_in_word] = '\0';
    if (0 != search_word_before_pairs (word,
				       field_name,
				     file_position_before_line + char_count,
				       0L,	/* line_pos */
				       weight,
                                       query_weight,
				       document_id,
				       (time_t) 0L,
				       capitalized,
				       db))
      return (-1);
    word_count++;
  }
  return (word_count);
}

boolean 
search_for_words (words, db, doc_id)
     char           *words;

     /* break the string into words (using map_over_words)
        and repeatedly call search_word_before_pairs(). 
        Returns true if successful.
      */
     database       *db;
     long            doc_id;		    /* = 1 for words == document in relevance feedback search */
{

#ifdef BOOL
  /* LISP QUERY */
  if (words[0] == '(') {	/* then it is a lisp query */
    /* this is a temporary stub for the real work */
    char            error_string[ERROR_STRING_LEN];
    object         *query = (object *) parseQuery (words, QUERY_SYNTAX_LISP, error_string);

    if (query == NULL) {
      waislog (WLOG_HIGH, WLOG_ERROR, "Unparsable query %s", error_string);
      return (false);
    } else {
      query = (object *) send (query, Evaluate, db);
      return (true);
    }
  }
#endif /* BOOL */

/* dgg mods really need new version of map_over_words for searching only
   (not for adding == indexing), and this way we can keep main search
   routines here (irsearch.c) & search_word in sersrch.c
 */

  /* NORMAL QUERY */
  if (-1 == search_over_words (words, doc_id, db))
    return (false);
  else
    return (true);
}

/* gets the next best hit from the search engine and fills in all the slots.
   If the document does not exist, then it gets another, etc.
   It returns 0 if successful */
long 
next_best_hit (the_best_hit, db)
     hit            *the_best_hit;
     database       *db;
{
  document_table_entry doc_entry;
  long            ret_value, start, end, length, date, nlines = 0;
  char            headline[MAX_HEADLINE_LEN];
  char            filename[200], type[20];

  while (1) {			/* keep going until we get a good document */
    if (0 != (ret_value = best_hit (db, &(the_best_hit->document_id),
				    &(the_best_hit->best_character),
				    &(the_best_hit->best_line),
				    &(the_best_hit->weight),
				    &start, &end, &date, &length, &nlines,
				    headline, filename, type))) {
      /* if we add start,end,length,date,headline here, we can remove lots of
         disk reads */

      return (ret_value);
    }
    if (the_best_hit->weight <= 0.0)	/* if we are out of good stuff, return */
      return (1);
    /* fill in the rest of the hit */
    the_best_hit->start_character = start;
    the_best_hit->end_character = end;
    the_best_hit->document_length = length;
    the_best_hit->number_of_lines = nlines;
    strcpy (the_best_hit->filename, filename);
    strcpy (the_best_hit->type, type);
    strcpy (the_best_hit->headline, headline);
    sprintf (the_best_hit->date, "%d", date);
    /* do we need this step??  JMF */
    if ( /*read_document_table_entry(&doc_entry, the_best_hit->document_id, db) */ true == true) {
      /*
         the_best_hit->start_character = doc_entry.start_character;
         the_best_hit->end_character = doc_entry.end_character;
         the_best_hit->document_length = doc_entry.document_length;
         the_best_hit->number_of_lines = doc_entry.number_of_lines;
         sprintf(the_best_hit->date, "%d", doc_entry.date);
         read_filename_table_entry(doc_entry.filename_id, 
         the_best_hit->filename,
         the_best_hit->type,
         NULL,
         db),
         strncpy(the_best_hit->headline, 
         read_headline_table_entry(doc_entry.headline_id,db),
         MAX_HEADLINE_LEN);
       */
      if (probe_file_possibly_compressed (the_best_hit->filename))
	return (0);
      else {
	waislog (WLOG_HIGH, WLOG_WARNING,
		 "Dangling File %s in database %s.",
		 the_best_hit->filename,
		 db->database_file);
      }
    } else {
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "Error reading doc_table_entry for database %s, docid: %ld",
	       db->database_file,
	       the_best_hit->document_id);
    }
    beFriendly ();
  }
}

/*-New one---------------------------------------------------------------------*/

/* this function figures out if the request is for a NEXT or Previous document.
   If it is, then it makes a header for it and returns it.  If not, then it 
   returns NULL. */

WAISDocumentHeader *
handle_next_and_previous (docs, db, waisProtocolVersion, server)
     DocObj        **docs;
     database       *db;
     long            waisProtocolVersion;
     char           *server;
{
  char           *dbName = db->database_file;
  WAISDocumentHeader *header;
  DocID          *theDocID = NULL;
  char           *local_id;

  long            count, i;
  char           *tmp_type = NULL;	    /* temporary type */
  char           *tmp_type_pointer = NULL;  /* temporary type pointer */

  if (docs != NULL) {		/* All of this is for WAIS_Prev and WAIS_next */
    if (docs[0] != NULL && docs[0]->Type != NULL) {
      long            id = -1;

      if ((theDocID = docIDFromAny (docs[0]->DocumentID)) == NULL) {
	waislog (WLOG_HIGH, WLOG_WARNING, "can't parse docid");
	return (NULL);
      }
      local_id = anyToString (GetLocalID (theDocID));

      if (strcmp (docs[0]->Type, "WAIS_NEXT") == 0)	/* next page */
	id = next_docid (local_id, db);
      else if (strcmp (docs[0]->Type, "WAIS_PREV") == 0)	/* prev page */
	id = previous_docid (local_id, db);

      freeDocID (theDocID);
      s_free (local_id);

      if (id > -1) {
	document_table_entry doc_entry;
	hit             foo;
	long            lines, length;
	char            local_id[MAX_FILENAME_LEN + 60];	/* filename, start, end */

	local_id[0] = '\0';

	if (read_document_table_entry (&doc_entry, id, db) == true) {
	  foo.start_character = doc_entry.start_character;
	  foo.end_character = doc_entry.end_character;
	  foo.document_length = doc_entry.document_length;
	  foo.number_of_lines = doc_entry.number_of_lines;

	  read_filename_table_entry (doc_entry.filename_id,
				     foo.filename,
				     foo.type,
				     NULL,
				     db),
	    strncpy (foo.headline,
		     read_headline_table_entry (doc_entry.headline_id, db),
		     MAX_HEADLINE_LEN);
	  sprintf (foo.date, "%d", doc_entry.date);
	  sprintf (local_id, "%ld %ld %s",
		   doc_entry.start_character,
		   doc_entry.end_character,
		   foo.filename);

	  if (calcDocLength (&(foo), &lines, &length)) {
	    /* this document is good, return it */
	    char          **type = NULL;

/* multitype extensions */
/* 
   Need to parse out the document types and add them to the 
   document type list.
 */
	    if (waisProtocolVersion >= '2') {

	      /* I left this conditional here (it is not really needed, the 
	         'else' part could take care of both) on the assumption that
	         it would be faster for single type documents.
	       */
	      if (strstr (foo.type, ",") == NULL) {
		type = (char **) s_malloc ((size_t) (sizeof (char *) * 2));

		type[0] = s_strdup (foo.type);
		type[1] = NULL;
	      } else {
		/* count up the number of document types */
		count = 1L;
		for (i = 0L; i < strlen (foo.type); i++) {
		  if (foo.type[i] == ',')
		    count++;
		}

		/* allocate space for types */
		type = (char **) s_malloc ((size_t) (sizeof (char *) * (count + 1L)));

		/* duplicate the type and save the pointer */
		tmp_type = s_strdup (foo.type);
		tmp_type_pointer = tmp_type;

		/* add types - NULL out the pointer so that strtok can grab the subsequent entries */
		for (i = 0L; i < count; i++) {
		  type[i] = s_strdup (strtok (tmp_type_pointer, ","));
		  tmp_type_pointer = NULL;
		}

		/* add the terminating null */
		type[count] = NULL;

		/* release the tmp_type allocations */
		s_free (tmp_type);

	      }

	    }
	    theDocID = makeDocID ();

	    theDocID->distributorServer = stringToAny (server);
	    theDocID->originalServer = stringToAny (server);
	    theDocID->distributorDatabase = stringToAny (dbName);
	    theDocID->originalDatabase = stringToAny (dbName);
	    theDocID->distributorLocalID = stringToAny (local_id);
	    theDocID->originalLocalID = stringToAny (local_id);

	    header =
	      makeWAISDocumentHeader (anyFromDocID (theDocID),
				      UNUSED,
				      -1L,
				      UNUSED,
				      length, lines,
				      type,
				      s_strdup (dbName),
				      s_strdup (foo.date),
				      s_strdup (foo.headline),
				      NULL);
	    freeDocID (theDocID);
	    return (header);
	  } else {
	    waislog (WLOG_HIGH, WLOG_WARNING,
		     "document <%ld %ld %s> skipped.",
		     doc_entry.start_character,
		     doc_entry.end_character,
		     foo.filename);
	    return (NULL);
	  }
	}
      }
    }
  }
  return (NULL);
}

/*----------------------------------------------------------------------*/
/* search for each of the words in a document, up to a limit.
   this is for relevance feedback. */

#define MAX_TEXT_SIZE 100000	/* Maximume size of relevant text */

/* returns true if it added the words, false otherwise (not necessarily 
   an error) */
boolean 
search_for_words_in_document (doc, docid, db, diags, num_diags)
     DocObj         *doc;
     long            docid;
     database       *db;
     diagnosticRecord ***diags;		    /* list of diagnostics */
     long           *num_diags;
{
  char           *dbName = db->database_file;
  long            errorCode;
  WAISDocumentText *doctext;

  char            prefix[MAX_WORD_LENGTH + 1];
  trie           *the_dict;
  trie_allocator *alloc;

  count_trie_words = 0;
  count_uniq = 0;

  alloc = make_trie_allocator ();
  the_dict = new_trie ("", alloc);
  *prefix = 0;

  if (doc->Type == NULL ||
      substrcmp (doc->Type, "TEXT") ||
      strcmp (doc->Type, "WSRC") == 0 ||
      strcmp (doc->Type, "WCAT") == 0 ||
      doc->Type[0] == 0) {

    doctext = NULL;
    if (doc->ChunkCode == CT_line)
      doctext = getDocumentText (doc, &errorCode, NULL);
    else if ((doc->ChunkCode == CT_byte) ||
	     (doc->ChunkCode == CT_document))
      doctext = getData (doc, &errorCode, NULL);
    if (doctext != NULL) {

      boolean         search_result;

      if (doctext->DocumentText->size > MAX_TEXT_SIZE)
	doctext->DocumentText->bytes[MAX_TEXT_SIZE] = 0;
      search_result = prepare_word_list (doctext->DocumentText->bytes, the_dict, alloc);
      if (db->number_of_fields > 0) {
	long            fcount;

	for (fcount = 0; fcount < db->number_of_fields; fcount++) {
	  if (db->fields[fcount].numeric == false)
	    search_result |= search_for_trie_words (the_dict, db, prefix, docid, search_result, db->fields[fcount].field_name);
	}
	if (db->dictionary_stream != NULL) {
	  search_result |= search_for_trie_words (the_dict, db, prefix, docid, search_result, FREE_TEXT_FIELD);
	}
      } else
	search_result |= search_for_trie_words (the_dict, db, prefix, docid, search_result, "\0");
      dispose_trie_allocator (alloc);

      freeWAISDocumentText (doctext);
      return (search_result);
    } else {			/* bad docid? */
      DocID          *theDocID = NULL;
      char           *local_id = NULL;
      diagnosticRecord *diag = NULL;
      char            msg[MAX_FILENAME_LEN * 2];

      theDocID = docIDFromAny (doc->DocumentID);

      if (theDocID == NULL) {
	local_id = s_strdup ("can't parse docid");
      } else {
	local_id = anyToString (GetLocalID (theDocID));

	freeDocID (theDocID);
      }
      waislog (WLOG_HIGH, WLOG_WARNING,
	       "Relevance Feedback with invalid doc-id: '%s'",
	       local_id);
      strncpy (msg, "Relevant Document not available: ",
	       MAX_FILENAME_LEN);
      s_strncat (msg, local_id, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
      s_free (local_id);
      (*num_diags)++;
      diag = makeDiag (true, D_TemporarySystemError, msg);
      *diags = (diagnosticRecord **) s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * *num_diags));
      (*diags)[(*num_diags) - 1] = diag;
    }

  }
  return (false);
}

/*----------------------------------------------------------------------*/
#ifdef FIXDATE
FixDate (date)
     char           *date;
{
  long            ndate;
  int             year, month, day;

  ndate = atoi (date);
  year = ndate / 10000;
  month = (ndate - 10000 * year) / 100;
  day = (ndate - 10000 * year - 100 * month);
#ifdef DEBUG
  fprintf (stderr, "FixDate: %ld (%d/%d/%d)\n", ndate, year, month, day);
  fprintf (stderr, "FixDate: %s\n", date);
#endif /* DEBUG */
  if (year > 100)
    strcpy (date, "0");
  else if (month > 12)
    strcpy (date, "0");
  else if (day > 31)
    strcpy (date, "0");
}
#endif /* FIXDATE */

#ifdef ONELINELENGTH
FixLength (length)
     int            *length;
{
  if (*length < MAX_HEADLINE_LEN)
    *length = 0;
}
#endif /* ONELINELENGTH */

WAISDocumentHeader *
best_hit_to_header (best_hit, maxRawScore, waisProtocolVersion, server, db)
     hit            *best_hit;
     long            maxRawScore;
     long            waisProtocolVersion;
     char           *server;
     database       *db;
{
  long            lines, length, count, i;
  DocID          *theDocID = NULL;
  WAISDocumentHeader *header;
  char           *originName = db->database_file;
  char            local_id[MAX_FILENAME_LEN + 60];	/* filename, start, end */
  char           *tmp_type = NULL;	    /* temporary type */
  char           *tmp_type_pointer = NULL;  /* temporary type pointer */

  local_id[0] = '\0';

  if (true == calcDocLength (best_hit, &lines, &length)) {	/* this document is good, return it */
    char          **type = NULL;
    long            normalScore;

    if (_BE_normalized)
      normalScore = best_hit->weight / MAX_NORMAL_SCORE;
    else {
      normalScore = (long) floor (
				   (((double) best_hit->weight) /
				    ((double) maxRawScore)) *
				   (MAX_NORMAL_SCORE + 1));

      if (normalScore > MAX_NORMAL_SCORE)
	normalScore = MAX_NORMAL_SCORE;
    }

    sprintf (local_id, "%ld %ld %s",
	     best_hit->start_character,
	     best_hit->end_character,
	     best_hit->filename);

/* multitype extensions */
/* 
   Need to parse out the document types and add them to the 
   document type list.
 */
    if (waisProtocolVersion >= '2') {

      /* I left this conditional here (it is not really needed, the 
       * 'else' part could take care of both) on the assumption that
       * it would be faster for single type documents.
       */
      if (strstr (best_hit->type, ",") == NULL) {
	type = (char **) s_malloc ((size_t) (sizeof (char *) * 2));

	type[0] = s_strdup (best_hit->type);
	type[1] = NULL;
      } else {
/* Check for multiple types, including STELAR */
#ifdef STELAR
	/*
	 * Use originName to decide whether to look for bitmaps (only database
	 * 'stelar' makes associations between abstracts and bitmaps).
	 */
	char           *dbname;

	if ((dbname = strrchr (originName, '/')) != NULL)
	  dbname++;
	else
	  dbname = originName;
	if ((!strcasecmp (dbname, "STELAR")
	     || !strcasecmp (dbname, "STELAR-DB")) && BITMAPS) {
	  char           *absname, *bitname;
	  char            tmpname[80], inrec[80];
	  int             found = 0;

	  if ((absname = strrchr (best_hit->filename, '/')) != NULL)
	    strcpy (tmpname, ++absname);
	  else
	    strcpy (tmpname, best_hit->filename);
	  absname = strtok (tmpname, ".");
	  rewind (BITMAPS);
	  do {
	    fgets (inrec, 80, BITMAPS);
	    bitname = strtok (inrec, "\n");
	    if (!strcasecmp (absname, bitname)) {
	      found = 1;
	      break;
	    }
	  } while (!feof (BITMAPS));

	  if (found) {
	    waislog (WLOG_LOW, WLOG_RESULTS,
		     "Bitmap association: %s", absname);
	    type = (char **) s_malloc ((size_t) (sizeof (char *) * 4));

	    type[0] = s_strdup (best_hit->type);
	    type[1] = s_strdup ("TIFF");
	    type[2] = s_strdup ("ARTICLE_TIFF");
	    type[3] = NULL;
	    length = 1024L * 1024L;
	  } else {
	    type = (char **) s_malloc ((size_t) (sizeof (char *) * 2));

	    type[0] = s_strdup (best_hit->type);
	    type[1] = NULL;
	  }
	} else {		/* It's not the STELAR database */
#endif /* STELAR */
	  /* count up the number of document types */
	  count = 1L;
	  for (i = 0L; i < strlen (best_hit->type); i++) {
	    if (best_hit->type[i] == ',')
	      count++;
	  }

	  /* allocate space for types */
	  type = (char **) s_malloc ((size_t) (sizeof (char *) * (count + 1L)));

	  /* duplicate the type and save the pointer */
	  tmp_type = s_strdup (best_hit->type);
	  tmp_type_pointer = tmp_type;

	  /* add types - NULL out the pointer so that strtok can grab the subsequent entries */
	  for (i = 0L; i < count; i++) {
	    type[i] = s_strdup (strtok (tmp_type_pointer, ","));
	    tmp_type_pointer = NULL;
	  }

	  /* add the terminating null */
	  type[count] = NULL;

	  /* release the tmp_type allocations */
	  s_free (tmp_type);
#ifdef STELAR
	}
#endif /* STELAR */
      }
    } else
      type = NULL;
    /*
       printf("header %ld out of %ld\n", *headerNum, 
       wais_search->MaxDocumentsRetrieved); 
     */
    theDocID = makeDocID ();

    theDocID->distributorServer = stringToAny (server);
    theDocID->originalServer = stringToAny (server);

    theDocID->distributorDatabase = stringToAny (originName);
    theDocID->originalDatabase = stringToAny (originName);

    theDocID->distributorLocalID = stringToAny (local_id);
    theDocID->originalLocalID = stringToAny (local_id);
#ifdef FIXDATE
    FixDate (best_hit->date);
#endif /* FIXDATE */
#ifdef ONELINELENGTH
    FixLength (&length);
#endif /* ONELINELENGTH */
    header =
      makeWAISDocumentHeader (anyFromDocID (theDocID),
			      UNUSED,
			      (long) normalScore,
			      best_hit->best_line,
			      length, lines,
			      type,
			      s_strdup (originName),
			      s_strdup (best_hit->date),
			      s_strdup (best_hit->headline),
			      NULL);
    freeDocID (theDocID);
    return (header);
  } else {
    waislog (WLOG_HIGH, WLOG_WARNING,
	     "document <%ld %ld %s> skipped.",
	     best_hit->start_character,
	     best_hit->end_character,
	     best_hit->filename);
    return (NULL);
  }
}

/*----------------------------------------------------------------------*/

boolean 
run_search (aSearch, headers, diags, index_directory,
	    seed_words_used, waisProtocolVersion, headerNum)
     SearchAPDU     *aSearch;
     WAISDocumentHeader **headers;	    /* list of results */
     diagnosticRecord ***diags;		    /* list of diagnostics */
     char           *index_directory;
     char          **seed_words_used;	    /* called with enough space */
     long            waisProtocolVersion;
     long           *headerNum;

/* runs a search on the inverted file index and returns false if it errors 
   in such a way that it can not even make a diagnostic record 
   (should not happen).
   It changes headers with the replies or makes a diagnostic record
 */
{
  extern long     number_of_operands;
  extern char    *field_name_array[];
  char            field_name[MAX_FILENAME_LEN + 1];
  char           *SeedWords = NULL;
  long            local_number_of_operands = 1;
  long            number_of_fields = 0;
  int             global_dictionary_exists = 0;
  diagnosticRecord *diag = NULL;
  WAISSearch     *wais_search = (WAISSearch *) aSearch->Query;	/* for convenience */
  database       *db = NULL;
  long            maxRawScore;
  long            i;
  query_parameter_type parameters;
  boolean         search_result;
  char            server[255];
  WAISDocumentHeader *header;
  long            num_diags = 0;
  char            dbName[MAX_FILENAME_LEN * 2];

  if (aSearch->DatabaseNames == NULL)
    strcpy (dbName, merge_pathnames (INFO_DATABASE_NAME, index_directory));
  else
    strcpy (dbName, merge_pathnames (aSearch->DatabaseNames[0], index_directory));

#ifdef GET_QUERY_TIMING
/*
   ftime(&s_time);
 */
  times (&s_time);
#endif /* GET_QUERY_TIMING */

  /* strlip .src if it is on the name */
  if (strlen (dbName) > strlen (".src"))
    if (0 == strcmp (dbName + strlen (dbName) - strlen (".src"),
		     ".src"))
      dbName[strlen (dbName) - strlen (".src")] = '\0';

  if (server_name != NULL)
    sprintf (server, "%s:%d", server_name, tcp_port);
  else
    sprintf (server, "localhost:0");

  /* removed strdup (up) */
  if ((SeedWords = (char *) analyse_string (wais_search->SeedWords,
					    &number_of_fields,
				      &global_dictionary_exists)) == NULL) {
    char            msg[MAX_FILENAME_LEN * 2];

    strncpy (msg, "query syntax error: ", MAX_FILENAME_LEN);
    s_strncat (msg, wais_search->SeedWords, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
    diag = makeDiag (false, D_PermanentSystemError, msg);
    *diags = (diagnosticRecord **) s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * 2));
    (*diags)[0] = diag;
    (*diags)[1] = NULL;
    return (false);
  }
  db = openDatabase (dbName, false, true, (number_of_fields > 0) ? true : false);
  if (db == NULL) {
    char            msg[MAX_FILENAME_LEN * 2];

    strncpy (msg, "The following database is not available: ",
	     MAX_FILENAME_LEN);
    s_strncat (msg, dbName, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
    diag = makeDiag (false, D_PermanentSystemError, msg);
    *diags = (diagnosticRecord **) s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * 2));
    (*diags)[0] = diag;
    (*diags)[1] = NULL;
    return (false);
  }
  if (number_of_fields > 0) {
    insert_fields (field_name_array, number_of_fields, db);
    if (!open_field_streams_for_search (field_name, global_dictionary_exists, db)) {
      char            msg[MAX_FILENAME_LEN * 2];

      strncpy (msg, "field unexists: ", MAX_FILENAME_LEN);
      s_strncat (msg, field_name, MAX_FILENAME_LEN, MAX_FILENAME_LEN);
      s_strncat (msg, "\n", MAX_FILENAME_LEN, MAX_FILENAME_LEN);
      diag = makeDiag (false, D_PermanentSystemError, msg);
      *diags = (diagnosticRecord **) s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * 2));
      (*diags)[0] = diag;
      (*diags)[1] = NULL;
      disposeDatabase (db);
      return (false);
    }
  }
  local_number_of_operands = number_of_operands;
  number_of_operands = 1;

#ifdef BIO
  {
    char           *cp = read_delimiters (db);	/* use data-specific delimiters, if available */

    if (cp != NULL) {
      strcpy (gDelimiters, cp);
      wordDelimiter = wordbreak_user;
    } else
      wordDelimiter = wordbreak_notalnum;
  }
#else /* not BIO */
  wordDelimiter = wordbreak_notalnum;	/* actually, wordDelimeter is used only ifdef BIO ? */
#endif /* not BIO */

  /* figure out if it is a NEXT or PREVIOUS, if so, return it. */
  header = handle_next_and_previous (wais_search->Docs, db,
				     waisProtocolVersion, server);
  if (header != NULL) {
    headers[(*headerNum)++] = header;
    headers[*headerNum] = NULL;
    return (true);
  }
  /* until seed_words_used is supported */
  strcpy (*seed_words_used, wais_search->SeedWords);

  parameters.max_hit_retrieved = wais_search->MaxDocumentsRetrieved;
  set_query_parameter (SET_MAX_RETRIEVED_MASK, &parameters);

  search_result = false;
  init_search_word (db);

#ifdef RELEVANCE_FEEDBACK
  if (wais_search->Docs != NULL) {
    DocObj         *doc = NULL;
    boolean         res;

    /* assemble the elements and construct a response */
    for (i = 0, doc = wais_search->Docs[i];
	 doc != NULL;
	 doc = wais_search->Docs[++i]) {
      search_result |=
	search_for_words_in_document (doc, i + 1, db, diags, &num_diags);
    }
    if (*diags != NULL) {
      num_diags++;
      *diags = (diagnosticRecord **) s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * num_diags));
      (*diags)[num_diags - 1] = NULL;
    }
  }
#endif /* RELEVANCE_FEEDBACK */

  number_of_operands = local_number_of_operands;
  search_result |= search_for_words (SeedWords, db, 0);
  if (SeedWords != NULL)
    s_free (SeedWords);

  if (search_result == true) {	/* the search went ok */
    hit             best_hit;

    finished_search_word (db);
    init_best_hit (db);
    for (i = 0; i < wais_search->MaxDocumentsRetrieved; i++) {
      if (0 != next_best_hit (&best_hit, db))
	break;			/* out of hits */
      if (i == 0)
	maxRawScore = best_hit.weight;
      if (best_hit.weight > 0) {
	WAISDocumentHeader *header =
	best_hit_to_header (&best_hit, maxRawScore,
			    waisProtocolVersion, server, db);

	if (NULL != header) {
	  headers[(*headerNum)++] = header;
	  headers[*headerNum] = NULL;
	}
      }
    }
  } else {			/* something went awry in the search */
    num_diags++;
    diag = makeDiag (true, D_PermanentSystemError,
		     "Serious error in server");
    *diags = (diagnosticRecord **)
      s_realloc (*diags, (size_t) (sizeof (diagnosticRecord *) * num_diags));
    (*diags)[num_diags - 2] = diag;
    (*diags)[num_diags - 1] = NULL;
  }
  finished_best_hit (db);
  /* free everything */
  /*closeDatabase(db); */
  disposeDatabase (db);		/* (up) */
#ifdef GET_QUERY_TIMING
/*
   ftime(&e_time);
   t_time += (e_time.time + e_time.millitm/1000.0) - 
   (s_time.time + s_time.millitm/1000.0);
   n_query++;
   if ( n_query == 200 ) {
   waislog(WLOG_LOW, WLOG_INFO, "searching 200 queries takes %f seconds.",
   t_time);
   waislog(WLOG_LOW, WLOG_INFO, "average %f/query.", t_time/200.0);
   n_query = 0;
   t_time = 0;
   }
 */
  times (&e_time);
  t_time = ((e_time.tms_utime - s_time.tms_utime) + (e_time.tms_stime - s_time.tms_stime)) / 60.0;
  /*printf("time= %f\n", t_time); */
/*  waislog(WLOG_LOW, WLOG_INFO, "time= %f", t_time); */
#endif /* GET_QUERY_TIMING */

  return (true);
}
