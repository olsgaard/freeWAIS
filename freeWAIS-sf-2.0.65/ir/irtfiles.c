/*                               -*- Mode: C -*- 
 * 
 * irtfiles.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Jonny Goldman
 * Created On      : Fri Jun 23 16:36:37 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Thu Oct 12 14:36:14 1995
 * Language        : C
 * Update Count    : 17
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/* Change log:
 * $Log: irtfiles.c,v $
 * Revision 2.0.1.3  1995/10/16 12:45:08  pfeifer
 * patch38: Passed dataops->type to how_index_line if dataops->indextype
 * patch38: is not given. this is obviously a hack.
 *
 * Revision 2.0.1.2  1995/09/15  09:46:04  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:50:32  pfeifer
 * patch4: no change
 *
 * Revision 2.0  1995/09/08  08:00:26  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.35  1995/06/23  14:50:48  pfeifer
 * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS
 *
 * Revision 1.34  1995/06/23  12:28:53  pfeifer
 * unifdef -DBOOLEANS -DNESTED_BOOLEANS -DFIELDS | indent
 *
 * Revision 1.33  1995/05/07  16:04:57  pfeifer
 * More fixes by Tom Snee. Literal serach now works again!
 *
 * Revision 1.32  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.31  1995/04/17  13:33:27  pfeifer
 * Marek Andricik: It seems to me that in file ir/irtfiles.c at lines
 * 1830 and 1842 there are typos. If expression (mesg = -1)...
 *
 * Revision 1.30  1995/04/17  13:18:11  huynh1
 * Release 1.1.2
 *
 * Revision 1.29  1994/12/13  17:03:58  pfeifer
 * *** empty log message ***
 *
 * Revision 1.26  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.25  1994/08/22  14:11:52  pfeifer
 * waisindex: files on the command line my now contaion extensions ".gz"
 *
 * Revision 1.24  1994/08/08  10:29:53  pfeifer
 * Fixed the MAX_OCCURANCES - STOP_WORD_FLAG bug
 *
 * Revision 1.23  1994/08/05  08:53:59  pfeifer
 * Fix for total_word_count bug
 *
 * Revision 1.22  1994/08/05  07:12:22  pfeifer
 * Release beta 04
 *
 * Revision 1.20  1994/07/13  09:55:23  pfeifer
 * Negative numerics.
 *
 * Revision 1.19  1994/07/13  07:53:26  pfeifer
 * beta 02
 *
 * Revision 1.18  1994/06/09  16:49:50  pfeifer
 * changed index_text_file to put the URL in the filename_id. So headline
 * is freely customizable.
 *
 * Revision 1.17  1994/05/27  11:06:20  huynh1
 * updated index_text_file, field_index_text_file, index_directory. beta
 *
 * Revision 1.16  1994/05/21  14:29:38  pfeifer
 * fixes: add_word and field_add_word are called with missing
 *        word_position parameter
 *
 * Revision 1.15  1994/05/20  12:52:24  huynh1
 * weights
 *
 * Revision 1.13  1994/03/23  13:09:35  pfeifer
 * removed the include iso.h
 *
 * Revision 1.12  1994/03/17  13:52:18  pfeifer
 * patchlevel 06
 *
 * Revision 1.11  1994/03/17  13:28:19  huynh1
 * index headlines.
 * Patchlevel 06.
 *
 * Revision 1.10  1994/03/16  16:48:47  pfeifer
 * fixed bug in cleanHeadline
 *
 * Revision 1.9  1994/03/11  15:16:15  huynh1
 * not index headlines.
 * Patchlevel 05.
 *
 * Revision 1.8  1994/03/10  17:33:56  huynh1
 * Patchlevel 05
 *
 * Revision 1.7  1994/03/08  20:45:07  huynh1
 * Patchlevel 04
 *
 * Revision 1.6  1994/02/14  10:33:48  huynh1
 * new code for field concept added.
 *
 * Revision 1.4  1993/09/22  16:07:52  pfeifer
 * Fixed word breaking for german ISO Umlaute and sz
 *
 * Revision 1.3  1993/06/04  10:23:15  pfeifer
 * Pachtlevel BIBDB
 *
 * Revision 1.2  1993/06/01  14:05:54  pfeifer
 * Added code for soundex/phonix indexing and retrieval
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.32  92/05/06  17:32:14  jonathan
 * Added new global for current_filename and current_filecount (from
 * riddle@rice.edu).
 * 
 * Revision 1.31  92/04/30  12:25:09  jonathan
 * changed a couple of s_free's to free's for ULTRIX CC.
 * 
 * Revision 1.30  92/04/29  08:09:55  shen
 * add global variable "_indexable_section", default is true
 * 
 * Revision 1.29  92/04/28  17:53:24  jonathan
 * Replaced directory routines with scandir.
 * 
 * Revision 1.28  92/03/20  11:02:55  jonathan
 * Added code to handle switches for word_pairs and word_postition info.
 * 
 * Revision 1.27  92/02/13  11:23:21  jonathan
 * Removed printable_time() from index logging, since it's done by waislog.
 * 
 * Revision 1.26  92/02/12  13:31:29  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 */

/* 
 * Indexes the words in a text file.
 * 
 * Port of irtfiles.lisp.
 *
 * -brewster 6/90
 */

/* the main functions are:
 *   index_text_file
 *   index_directory
 *
 * Some of the policy issues coded in this file are
 *   What extra weight should the headline get?
 *
 */

#define EXTERN_WORD_DELIMITER
#define EXTERN_MAKE_COMPILER_HAPPY
/* #include <ctype.h> */
/* #include <string.h> */
#include "panic.h"
#include "cdialect.h"
#include "irhash.h"
#include "cutil.h"
#include "futil.h"
#include "irfiles.h"
#include "irtfiles.h"

#include "ircfiles.h"		/* dgg, need for genbank_header_function test */

#ifdef STEM_WORDS
#include "stemmer.h"
#endif /* STEM_WORDS */

#include "field_index.h"

#ifdef NEW_WEIGHT
#include "weight.h"
#endif /* NEW_WEIGHT */

#ifdef SOUND
#include "soundex.h"
#endif /* SOUND */

#ifndef THINK_C
#include <sys/types.h>
#include <sys/stat.h>
#endif /* not THINK_C */

#define MAX_LINE_LENGTH 1000	/* characters */
#define extra_weight_for_header 10

#ifdef UNIX
#define PRINT_AS_INDEXING true	/* also defined in irfiles.c */
#else /* not UNIX */
#define PRINT_AS_INDEXING false
#endif /* not UNIX */

char           *header_flag_1;
char           *header_flag_2;
long            len_of_files_since_last_delete = 0;
long            len_of_files_since_last_flush = 0;
long            total_indexed_file_length = 0;

boolean         indexingForBeta = false;

long            _indexable_section = 1;

char           *current_filename = NULL;
int             current_filecount = 0;

boolean         index_contents = true;

#define keyword_weight 1

/* keywords from command line (set in irbuild.c), used in finish_document */
char           *keywords = NULL;

/* name of keyword file from command line, used in finish_document */
char           *keyword_filename = NULL;

/*  Handling Word Pairs */

/* makes a word_pair out of a two words:
   make_joint_word("abcdefghijklmnopqrstuvwxyz", "123456789012345678901");
   "abcdefghij1234567890"
   make_joint_word("abcdefghijkl", "123");
   "abcdefghij123"
   make_joint_word("abc", "123");
   "abc123" */

char           *
make_joint_word (word1, word2)
     char           *word1;
     char           *word2;
{
  static char     new_word[MAX_WORD_LENGTH + 1];

  bzero (new_word, MAX_WORD_LENGTH + 1);
  strncpy (new_word, word1, MAX_WORD_LENGTH / 2);
  strncpy (new_word + MINIMUM (MAX_WORD_LENGTH / 2, strlen (word1)),
	   word2, MAX_WORD_LENGTH - (MAX_WORD_LENGTH / 2));
  return (new_word);
}

/* returns 0 is successful, non-0 if error */
static long add_word_before_pairs _AP ((char *word, long char_pos,
					long line_pos, long weight,
					long doc_id, time_t date,
					boolean capitalized, database * db,
				boolean word_position, boolean word_pairs));

static long
add_word_before_pairs (word, char_pos, line_pos,
		       weight, doc_id, date, capitalized, db,
		       word_position, word_pairs)
     char           *word;		    /* the word to be indexed, this could be a

					       word pair. If NULL there are no more words
					       to be indexed */
     long            char_pos;		    /* the position of the start of the

					       word */
     long            line_pos;		    /* this is passed for the best

					       section calculation */
     long            weight;		    /* how important the word looks

					       syntactically (such as is it bold)
					       NOT used by signature system */
     long            doc_id;		    /* current document, this will never be 0 */
     time_t          date;		    /* display day of this document, 0 if not known */
     boolean         capitalized;	    /* if the word started with a cap */
     database       *db;		    /* database to insert the document */
     boolean         word_position;	    /* if true, include word position in index. */
     boolean         word_pairs;	    /* if true, add pairs of capitalized words */
{
  static char     last_word[MAX_WORD_LENGTH + 1];
  static long     last_doc_id = -1;

  /* The way it works is it remembers if the last word if it was
     capitalized (if not it clears the saved word).  
     If another capitalized word comes along next
     (and it is in the same document), then it makes a joint word and calls 
     add_word with it. 

     This does not throw away stopwords before forming pairs, so it will 
     not be quite what CMDRS does.  This should only be used in seeker 
     and serial searching before proximity is used.

   */

  /* Tung, 2/95  
   * I set word_pairs = false and word_position = false 
   * because of   of memory place 
   * if word_pairs = true. The retrievaleffectivity is not better.
   */
  word_pairs = false;
  word_position = false;

  if (capitalized && word_pairs) {
    if (last_word[0] != '\0' && last_doc_id == doc_id) {
      add_word (make_joint_word (last_word, word),	/* added word_position (up) */
	   char_pos, line_pos, weight, doc_id, date, 1L, db, word_position);
    } else {
      last_word[0] = '\0';
    }
    strncpy (last_word, word, MAX_WORD_LENGTH);
    last_doc_id = doc_id;
  } else {			/* not capitalized or word_pairs is false */
    last_word[0] = '\0';
  }

  return (add_word (word, char_pos, line_pos, weight, doc_id, date, 0L, db, word_position));
}

static long field_add_word_before_pairs _AP ((char *word, long char_pos,
					      long line_pos, long weight,
					      long doc_id, time_t date,
					 boolean capitalized, database * db,
				boolean word_position, boolean word_pairs));

static long
field_add_word_before_pairs (word, char_pos, line_pos,
			     weight, doc_id, date, capitalized, db,
			     word_position, word_pairs)
     char           *word;		    /* the word to be indexed, this could be a

					       word pair. If NULL there are no more words
					       to be indexed */
     long            char_pos;		    /* the position of the start of the

					       word */
     long            line_pos;		    /* this is passed for the best

					       section calculation */
     long            weight;		    /* how important the word looks

					       syntactically (such as is it bold)
					       NOT used by signature system */
     long            doc_id;		    /* current document, this will never be 0 */
     time_t          date;		    /* display day of this document, 0 if not known */
     boolean         capitalized;	    /* if the word started with a cap */
     database       *db;		    /* database to insert the document */
     boolean         word_position;	    /* if true, include word position in index. */
     boolean         word_pairs;	    /* if true, add pairs of capitalized words */
{
  static char     field_last_word[MAX_WORD_LENGTH + 1];
  static long     field_last_doc_id = -1;
  static          last_current_field_id = -1;

  /* The way it works is it remembers if the last word if it was
     capitalized (if not it clears the saved word).  
     If another capitalized word comes along next
     (and it is in the same document), then it makes a joint word and calls 
     add_word with it. 

     This does not throw away stopwords before forming pairs, so it will 
     not be quite what CMDRS does.  This should only be used in seeker 
     and serial searching before proximity is used.

   */

  /* Tung, 2/95  
   * I set word_pairs = false and word_position = false 
   * because of   of memory place 
   * if word_pairs = true. The retrievaleffectivity is not better.
   */
  word_pairs = false;
  word_position = false;

  if (last_current_field_id != db->current_field_id) {
    last_current_field_id = db->current_field_id;	/* defined in field.c */
    field_last_word[0] = '\0';
  }
  if (capitalized && word_pairs) {
    if (field_last_word[0] != '\0' && field_last_doc_id == doc_id) {
      field_add_word (make_joint_word (field_last_word, word),
      /* Hey Tung: If you don't prototype, you *must* check */
      /* the parameters. You forgott word_position!  (up) */
	   char_pos, line_pos, weight, doc_id, date, 1L, db, word_position);
    } else
      field_last_word[0] = '\0';
    strncpy (field_last_word, word, MAX_WORD_LENGTH);
    field_last_doc_id = doc_id;
  } else
    field_last_word[0] = '\0';

  return (field_add_word (word, char_pos, line_pos, weight, doc_id, date, 0L, db, word_position));
}

#ifdef NOTUSED
#define WORD_LETTERS  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"

static char    *new_word _AP ((char *line, char *word));

static char    *
new_word (line, word)
     char           *line;
     char           *word;
{
  /* This copies the first word from line into word while downcasing it.
     It returns a pointer into line that is after the word,
     which can be used to call this function again.
     If there are no words left, then NULL is returned,
     and word is length 0.
     There has got to be a better way.
   */
  long            i = 0;
  char           *beginning_ptr = strpbrk (line, WORD_LETTERS);
  char           *next_word;
  long            length;

  if (NULL == beginning_ptr) {
    word[0] = '\0';
    return (NULL);
  }
  length = strspn (beginning_ptr, WORD_LETTERS);
  next_word = length + beginning_ptr;

  length = MINIMUM (MAX_WORD_LENGTH, length);
  for (i = 0; i < length; i++) {
    word[i] = char_downcase ((unsigned long) *beginning_ptr++);
  }
  word[i] = '\0';
  return (next_word);
}

static boolean reasonable_word _AP ((char *word));

static          boolean
reasonable_word (word)
     char           *word;

/* this should be more sophisticated */
{
  if (strlen (word) > 1) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

#endif /* NOTUSED */

/* MAPPING A FUNCTION OVER WORDS (QUICKLY) */

/* map_over_words("foo bar baz", 0L, 1L, 0L, &integer, false, db, dummy_wordfunction) */
static long
dummy_wordfunction (word, char_pos, line_pos,
		    weight, doc_id, date, capitalized, db)
     char           *word;		    /* the word to be indexed, this could be a

					       word pair. If NULL there are no more words
					       to be indexed */
     long            char_pos;		    /* the position of the start of the

					       word */
     long            line_pos;		    /* this is passed for the best

					       section calculation */
     long            weight;		    /* how important the word looks

					       syntactically (such as is it bold)
					       NOT used by signature system */
     long            doc_id;		    /* current document, this will never be 0 */
     time_t          date;		    /* display day of this document, 0 if not known */
     boolean         capitalized;	    /* if the word started with a cap */
     database       *db;		    /* database to insert the document */
{
  if (word != NULL)
    printf ("word: %s, char_pos: %ld\n", word, char_pos);
  return (0);
}

#ifdef STEM_WORDS
extern boolean  index_stemming;		    /* defined in field_index.c */

#endif /* STEM_WORDS */

#ifdef PROXIMITY
boolean         wais_proximity_unit_line = 0; /* waisindex.c command line option '-line' */
#endif 

/* returns the number of words added, or -1 if an error occurred */
long
map_over_words (line,
		document_id,
		weight,
		file_position_before_line,
		line_length,
		newline_terminated,
		db,
		wordfunction,
		word_position, word_pairs,
#ifdef SOUND
		minwordlen, type)
#else					    /* not SOUND */
		minwordlen)		    /* dgg */
#endif					    /* not SOUND */
     char           *line;
     long            document_id;
     long            weight;
     long            file_position_before_line;
     long           *line_length;
     boolean        *newline_terminated;
     database       *db;
     wordfunc       *wordfunction;
     boolean         word_position, word_pairs;
     int             minwordlen;

#ifdef SOUND
     char           *type;

#endif /* SOUND */
{
  /* Add words to the index if it should be done. 
   * Returns the number of words added.
   * Should it return the amount of weight added?
   * The line length is side effected with the length of the line.
   * Newline_terminated is set based on whether the last character
   * in the string was a newline.  If it was not, then it fgets probably
   * did not retrieve the whole line.
   */

  long            position_in_word = 0;
  long            word_count = 0;
  unsigned long   ch;
  long            char_count = 0;
  boolean         capitalized = false;	    /* if the word starts with a cap */
  char            word[MAX_WORD_LENGTH + 1];
  static long     last_document_id = -1;
  static long     file_word_count = 0;
  static long     line_pos = 0;
  if (document_id != last_document_id) {
    file_word_count = 0;
    last_document_id = document_id;
    line_pos = 0;
  }
  line_pos++;
  for (ch = (unsigned char) line[char_count++];
       ch != '\0'; ch = (unsigned char) line[char_count++]) {
#ifdef BIO
    boolean         alnum = (wordDelimiter (ch) == NOT_DELIMITER);

#else /* not BIO */
    boolean         alnum = isalnum (ch);

#endif /* not BIO */

    if (db->number_of_fields > 0) {
      if (db->fields[db->current_field_position].numeric) {
	minwordlen = 1;
	if (ch == ',') {
	  ch = '.';
	  alnum = true;
	} else if (ch == '-') {	/* negative number */
	  /* printf("map_over_words: found negative number\n"); */
	  /* ch='a'; */
	  alnum = true;
	} else if (ch == '.') {
	  /* printf("map_over_words: found '.'\n"); */
	  alnum = true;
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
	/* then we have collected a word */
	if (position_in_word >= minwordlen) {	/* is it reasonable ? */
	  word[position_in_word] = '\0';

	  /* call the stemmer */
	  if (db->number_of_fields > 0) {
#ifdef STEM_WORDS
	    if (index_stemming)	/* defined in field_index.c */
	      stemmer (word);
	  } else {
	    if (db->stemming)
	      stemmer (word);
	  }
#endif /* STEM_WORDS */

#ifdef SOUND
	  if (type == NULL) {
#endif /* SOUND */
	    if (0 !=
		(*wordfunction) (word,
#ifndef PROXIMITY
				 file_position_before_line + char_count,
#else /* PROXIMITY */
				 (wais_proximity_unit_line)?line_pos:file_word_count,
#endif /* PROXIMITY */
	    /*^^ dgg, this param is supposed to be start-of-word, but char_count is now at end-of-word ! */
				 line_pos,
				 weight,
				 document_id,
				 (time_t) 0L,
				 capitalized,
				 db,
				 word_position,
				 word_pairs))
	      return (-1);	/* error */
	    word_count++;
	    file_word_count++;
#ifdef SOUND
	  } else {
#endif /* SOUND */

#ifdef SOUND
/*=========================== SOUNDEX / PHONIX ========================================*/
	    if (type != NULL)	/* use only the first word (i.e. the surname) for SOUNDEX/PHONIX! */
	      if ((!strcmp (type, "SOUNDEX")) || (!strcmp (type, "PHONIX"))) {
		char            code[20];
		int             i;

		if (!strcmp (type, "SOUNDEX"))
		  Soundex (word, code);
		else if (!strcmp (type, "PHONIX"))
		  Phonix (word, code);

		code[0] = tolower (code[0]);
#ifdef DEBUG
		fprintf (stderr, "%5d, %4s, %s\n", word_count, code, word);
#endif /* DEBUG */
		if (0 != (*wordfunction) (code,
#ifndef PROXIMITY
				     file_position_before_line + char_count,
#else /* PROXIMITY */
					  file_word_count,
#endif /* PROXIMITY */
					  0L,	/* line_pos */
					  weight,
					  document_id,
					  (time_t) 0L,
					  capitalized,
					  db,
					  word_position,
					  word_pairs))
		  return (-1);	/* error */

		word_count++;
		file_word_count++;
	      }
/*=====================================================================================*/
#endif /* SOUND */
#ifdef SOUND
	  }			/* else { */
#endif /* SOUND */
	}
	position_in_word = 0;
      }
    }
  }
  /* finish last word */
  if (position_in_word >= minwordlen) {		/* is it reasonable ? */
    word[position_in_word] = '\0';

    /* call the stemmer */
    if (db->number_of_fields > 0) {
#ifdef STEM_WORDS
      if (index_stemming)	/* defined in field_index.c */
	stemmer (word);
    } else {
      if (db->stemming)
	stemmer (word);
    }
#endif /* STEM_WORDS */

#ifdef SOUND
    if (type == NULL) {
#endif /* SOUND */
      if (0 != (*wordfunction) (word,
#ifndef PROXIMITY
				file_position_before_line + char_count,
#else /* PROXIMITY */
				file_word_count,
#endif /* PROXIMITY */
				0L,	/* line_pos */
				weight,
				document_id,
				(time_t) 0L,
				capitalized,
				db,
				word_position, word_pairs))
	return (-1);
      word_count++;
      file_word_count++;
#ifdef SOUND
    } else {
#ifdef SOUND
/*=========================== SOUNDEX / PHONIX ========================================*/
      if (type != NULL)		/* use only the word which contains alpha character for SOUNDEX/PHONIX! */
	if ((!strcmp (type, "SOUNDEX")) || (!strcmp (type, "PHONIX"))) {
	  char            code[20];
	  int             i;

	  if (!strcmp (type, "SOUNDEX"))
	    Soundex (word, code);
	  else if (!strcmp (type, "PHONIX"))
	    Phonix (word, code);

	  code[0] = tolower (code[0]);
#ifdef DEBUG
	  fprintf (stderr, "%5d, %4s, %s\n", word_count, code, word);
#endif /* DEBUG */
	  if (0 != (*wordfunction) (code,
				    file_position_before_line + char_count,
				    0L,		/* line_pos */
				    weight,
				    document_id,
				    (time_t) 0L,
				    capitalized,
				    db,
				    word_position,
				    word_pairs))
	    return (-1);	/* error */

	  word_count++;
	}
/*=====================================================================================*/
#endif /* SOUND */
    }
#endif /* SOUND */
  }				/* if(position_in_word >= minwordlen){ */
  /* for debugging
     if(char_count - 1 != strlen(line)) {
     waislog(WLOG_HIGH, WLOG_ERROR, 
     "char_count: %ld, strlen: %ld", char_count, strlen(line));
     }
   */
  if (newline_terminated != NULL) {
    if ('\n' != line[char_count - 2])
      *newline_terminated = false;
    else
      *newline_terminated = true;
  }
  if (line_length != NULL)
    *line_length = char_count - 1;
  return (word_count);
}

static long     add_words_if_appropriate
                _AP ((char *line, long document_id, long weight, long file_position_before_line,
	     long *line_length, boolean * newline_terminated, database * db,
		      boolean word_position, boolean word_pairs,
		      int minwordlen));

static long
add_words_if_appropriate (line,
			  document_id,
			  weight,
			  file_position_before_line,
			  line_length,
			  newline_terminated,
			  db,
			  word_position, word_pairs,
			  minwordlen)	    /* dgg */
     char           *line;
     long            document_id;
     long            weight;
     long            file_position_before_line;
     long           *line_length;
     boolean        *newline_terminated;
     database       *db;
     boolean         word_position, word_pairs;
     int             minwordlen;
{
  /* Add words to the index if it should be done. 
   * Returns the number of words added.
   * Should it return the amount of weight added?
   * The line length is side effected with the length of the line.
   * Newline_terminated is set based on whether the last character
   * in the string was a newline.  If it was not, then it fgets probably
   * did not retrieve the whole line.
   */

  long            position_in_word = 0;
  long            word_count = 0;
  char            word[MAX_WORD_LENGTH + 1];
  unsigned long   ch;
  long            char_count = 0;
  boolean         capitalized = false;	    /* if the word starts with a cap */
  static long     last_document_id = -1;
  static long     file_word_count = 0;

  if (document_id != last_document_id) {
    file_word_count = 0;
    last_document_id = document_id;
  }
  for (ch = (unsigned char) line[char_count++];
       ch != '\0'; ch = (unsigned char) line[char_count++]) {
#ifdef BIO
    ,boolean alnum = (wordDelimiter (ch) == NOT_DELIMITER);
#else /* not BIO */
    boolean         alnum = isalnum (ch);

#endif /* not BIO */

#ifdef FIELDS___
    if (db->number_of_fields > 0) {
      if (db->fields[db->current_field_position].numeric) {
	minwordlen = 1;
	if (ch == ',') {
	  ch = '.';
	  alnum = true;
	} else if (ch == '-') {	/* negative number */
	  /* printf("add_words_if_appropriate: found negative number\n"); */
	  /* ch='a'; */
	  alnum = true;
	} else if (ch == '.') {
	  /* printf("add_words_if_appropriate: found '.'\n"); */
	  alnum = true;
	}
      }
    }
#endif /* FIELDS___ */

    if (alnum) {
      /* put the character in the word if not too long */
      if (position_in_word == 0)
	capitalized = isupper ((unsigned long) ch) ? true : false;
      if (position_in_word < MAX_WORD_LENGTH) {
	word[position_in_word++] = char_downcase ((unsigned long) ch);
      }
    } else {			/* not an in a word */

      /* call the stemmer */
      if (db->number_of_fields > 0) {
#ifdef STEM_WORDS
	if (index_stemming)	/* defined in field_index.c */
	  stemmer (word);
      } else {
	if (db->stemming)
	  stemmer (word);
      }
#endif /* STEM_WORDS */

      if (position_in_word != 0) {
	/* then we have collected a word */
	if (position_in_word >= minwordlen) {	/* is it reasonable ? */
	  word[position_in_word] = '\0';
	  add_word_before_pairs (word,
#ifndef PROXIMITY
				 file_position_before_line + char_count,
#else /* PROXIMITY */
				 file_word_count,
#endif /* PROXIMITY */
				 0L,	/* line_pos */
				 weight,
				 document_id,
				 (time_t) 0L,
				 capitalized,
				 db,
				 word_position, word_pairs);
	  word_count++;
	  file_word_count++;
	}
	position_in_word = 0;
      }
    }
  }
  /* finish last word */
  if (position_in_word >= minwordlen) {		/* is it reasonable ? */
    word[position_in_word] = '\0';

    /* call the stemmer */
    if (db->number_of_fields > 0) {
#ifdef STEM_WORDS
      if (index_stemming)	/* defined in field_index.c */
	stemmer (word);
    } else {
      if (db->stemming)
	stemmer (word);
    }
#endif /* STEM_WORDS */

    add_word (word,
#ifndef PROXIMITY
	      file_position_before_line + char_count,
#else /* PROXIMITY */
	      file_word_count,
#endif /* PROXIMITY */
	      0L,		/* line_pos */
	      weight,
	      document_id,
	      (time_t) 0L,
	      0L,
	      db);
    word_count++;
    file_word_count++;
  }
  /* for debugging
     if(char_count - 1 != strlen(line)) {
     waislog(WLOG_HIGH, WLOG_ERROR, 
     "char_count: %ld, strlen: %ld", char_count, strlen(line));
     }
   */
  if ('\n' != line[char_count - 2])
    *newline_terminated = false;
  else
    *newline_terminated = true;

  *line_length = char_count - 1;
  return (word_count);
}

static int nodecompare _AP ((unsigned long *i, unsigned long *j));

static int
nodecompare (i, j)
     unsigned long  *i, *j;
{
  if (i[0] < j[0])
    return (-1);
  else if (i[0] > j[0])
    return (1);
  else
    return (0);
}

#define nodeRange 256		/* 2048 sprint nodes on a full sized machine - should
				   be passed in */
#define iterations_to_reorder 50	/* 1 is best but slow */

static void     finish_document
#ifndef SOUND
                _AP ((boolean recountHeader, char *header, char *line, long document_id,
		      document_table_entry * the_document_table_entry,
		      long file_position_before_line,
		      long file_position_before_document, database * db,
		      boolean word_position, boolean word_pairs,
		      int minwordlen));

#else					    /* SOUND */
                _AP ((boolean recountHeader, char *header, char *line, long document_id,
		      document_table_entry * the_document_table_entry,
		      long file_position_before_line,
		      long file_position_before_document, database * db,
		      boolean word_position, boolean word_pairs,
		      int minwordlen,
		      char *type));

#endif /* SOUND */

static void
finish_document (recountHeader, header, line, document_id, the_document_table_entry,
		 file_position_before_line, file_position_before_document,
		 db, word_position, word_pairs,
#ifndef SOUND
		 minwordlen)
#else					    /* SOUND */
		 minwordlen, type)
#endif					    /* SOUND */
     boolean         recountHeader;
     char           *header;
     char           *line;
     long            document_id;
     document_table_entry *the_document_table_entry;
     long            file_position_before_line;
     long            file_position_before_document;
     database       *db;
     boolean         word_position, word_pairs;

#ifdef SOUND
     char           *type;

#endif /* SOUND */
{
  long            line_length;
  boolean         newline_terminated;
  long            number_of_words = 0;	    /* initialize, to make shure (up) */

  /* It is very dangerous to index headlines, because the healines could
   * contain words which the document do not contain. It means if one make
   * a literal search the coredump is procuced, because the text positions of
   * these words are not correct.
   */
  if (db->number_of_fields == 0) {
    if (0 != strlen (header) && recountHeader) {
      /* add weights for the header (if there was one) */
      number_of_words =		/* new variable number_of_words ?? (up) */
	map_over_words (header, document_id,
			extra_weight_for_header,
			file_position_before_line -
			file_position_before_document,
			&line_length,
			&newline_terminated,
			db,
			add_word_before_pairs,
#ifdef SOUND
			word_position, word_pairs, minwordlen, type);
#else /* not SOUND */
			word_position, word_pairs, minwordlen);
#endif /* not SOUND */
    }
    if (number_of_words == -1)
      waislog (WLOG_HIGH, WLOG_ERROR, "map_over_words failed");
    db->total_word_count += number_of_words;
    the_document_table_entry->document_length += number_of_words;
  }
  if (keyword_filename != NULL) {
    /* add keywords from keyword file (if specified on command line) */

    char           *tmpFileName = NULL;
    FILE           *keyword_stream = NULL;
    char            line[MAX_LINE_LENGTH];

    if (keyword_filename != NULL &&
	strlen (keyword_filename) > 1 &&
	!strcmp (keyword_filename + (strlen (keyword_filename) - 2), ".Z"))
      /* it's a .Z file.  First, remove the suffix or many things get confused. */
      keyword_filename[(strlen (keyword_filename) - 2)] = 0;

    if (probe_file (keyword_filename)) {
      keyword_stream = s_fopen (keyword_filename, "r");
    } else if (probe_file_possibly_compressed (keyword_filename)) {
      tmpFileName = s_fzcat (keyword_filename);
      if (tmpFileName) {
	keyword_stream = s_fopen (keyword_filename, "r");
	unlink (tmpFileName);
	free (tmpFileName);
      }
    }
    if (NULL == keyword_stream)
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "Unable to open keyword file %s", keyword_filename);
    else {			/* read keyword_file, index its contents */
      waislog (WLOG_HIGH, WLOG_INDEX,
	       "Indexing keyword file %s", keyword_filename);
      while (TRUE) {
	/* read a line */
	if (!fgets (line, MAX_LINE_LENGTH, keyword_stream))
	  break;		/* eof */
	number_of_words =
	  map_over_words (line, document_id,
			  keyword_weight,
			  0,
			  &line_length,
			  &newline_terminated,
			  db,
			  add_word_before_pairs,
#ifdef SOUND
			  word_position, word_pairs, minwordlen, type);
#else /* not SOUND */
			  word_position, word_pairs, minwordlen);
#endif /* not SOUND */
	if (number_of_words == -1)
	  waislog (WLOG_HIGH, WLOG_ERROR, "map_over_words failed");
	db->total_word_count += number_of_words;
	the_document_table_entry->document_length += number_of_words;
      }
      s_fclose (keyword_stream);
    }
  }
  /* store out the document header here */
  the_document_table_entry->headline_id =
    write_headline_table_entry (header, db);
  if (NULL == line) {		/* EOF */
    /* if it goes to the end of the file, then
     * set the end_character to 0 so that it is clear that
     * it goes to the end of the file.
     */
    the_document_table_entry->end_character = 0;
  } else			/* set the end_character */
    the_document_table_entry->end_character = file_position_before_line;

  /* waislog("start char: %ld, end char: %ld", 
   * the_document_table_entry->start_character,
   * the_document_table_entry->end_character);
   */

  if (indexingForBeta) {	/* we need to decide which sprint node this doc will go in.
				 * for now we will store the sn in the date field, but that
				 * is temporary
				 * NOTE that we must subract 1 from document_id, since we want
				 * a 0 based number
				 */
    static unsigned long *nodes = NULL;	    /* size/node# inited to 0 to 2047 */
    static long     minPos;
    unsigned long   size;

    if (nodes == NULL) {
      long            i;
      long            startPos;
      time_t          temp_time;

      nodes = (unsigned long *) s_malloc (sizeof (unsigned long) * nodeRange * 2);

      srand ((int) time (&temp_time));	/* try to distribute the entries */
      startPos = rand () % nodeRange;	/* for indexes with < nodeRng docs */
      for (i = 0; i < nodeRange; i++) {
	nodes[(i * 2) + 1] = (i + startPos) % nodeRange;
	nodes[i * 2] = 0;
      }
      minPos = 0;
      /*printf("init: ");
         for (i = 0; i < nodeRange; i++)
         printf("<%lu,%lu> ",nodes[i*2],nodes[(i*2)+1]);
         NL(); */
    }
    /* place the document in the emptiest node (at minPos) */
    the_document_table_entry->date = (time_t) nodes[(minPos * 2) + 1];

    /* increment the size to account for document */
    size = nodes[minPos * 2];
    size += (the_document_table_entry->end_character -
	     the_document_table_entry->start_character);
    nodes[minPos * 2] = size;

    if ((the_document_table_entry->end_character -
	 the_document_table_entry->start_character) > 100000)
      printf ("big doc %lu %s\n", the_document_table_entry->end_character - the_document_table_entry->start_character, header);

    minPos++;

    /* possibly reorder it */
    if (minPos > iterations_to_reorder) {
      long            i;

      minPos = 0;
      /*printf("before: ");
         for (i = 0; i < nodeRange; i++)
         printf("<%lu,%lu> ",nodes[i*2],nodes[(i*2)+1]);
         NL(); */
      qsort ((char *) nodes, nodeRange, sizeof (unsigned long) * 2, nodecompare);

      /*printf("after: ");
         for (i = 0; i < nodeRange; i++)
         printf("<%lu,%lu> ",nodes[i*2],nodes[(i*2)+1]);
         NL(); */
      printf ("just sorted nodes, min: ");
      for (i = 0; i < 10; i++)
	printf ("%lu ", nodes[i * 2]);
      printf (", max: %lu/%lu\n", nodes[(nodeRange * 2) - 2], nodes[(nodeRange * 2) - 1]);
    }
#ifdef old
    sn = (document_id - 1) % 2048;	/* 2048 = sn's in a full machine */

    /* should also take into account the "fullness" of any particular
       node */
    the_document_table_entry->date = (time_t) sn;
    /* waislog(WLOG_LOW, WLOG_INFO, 
       "put %s in sprint node %ld",header,sn); */
#endif /* old */
  }
  write_document_table_entry (the_document_table_entry, db);
  cprintf (PRINT_AS_INDEXING, ".");
  total_indexed_file_length =	/* set this so the speed looks right */
    total_indexed_file_length + file_position_before_line;
  total_indexed_file_length =	/* set it back */
    total_indexed_file_length - file_position_before_line;
}

#define LENGTH_OF_NEWLINE 1	/* this will be 2 on a PC, I think */

#ifdef NEW_WEIGHT
long            number_of_bucket_ids = 0;   /* used in hash.c, irhash.c */

#endif /* NEW_WEIGHT */

long
index_text_file (filename, dataops, db,
		 check_for_text_file,
		 check_for_file_already_indexed,
		 word_position, word_pairs, field_id)
     char           *filename;
     dataopsrec     *dataops;
     database       *db;
     boolean         check_for_text_file;
     boolean         check_for_file_already_indexed;
     boolean         word_position, word_pairs;
     long            field_id;
{
  /* Addes words to the index for a given file.  
   * The function arguments can be NULL which means it would 
   *  always answer NULL.  
   * separator_function is called on every line to see if it 
   *  separates documents.  
   * header_function is called on every line so that a headline 
   *  can be accumulated.  This assumes that it will side effect global 
   *  variables.
   * finish_header_function is called when the document is finished 
   *  (by separator function responding TRUE or EOF) this will return 
   *  the headline string or NULL. 
   *  Presumably finish_header_function will use the
   *  effects of header_function.  finish_header_function 
   *  will only be called once, so it should clear whatever state 
   *  header_function has set.
   * if check_for_text_file then it looks to see if first character
   *  in the file is a printable character.
   * if check_for_file_already_indexed then it looks through the filename 
   *  file to see if the file has not been indexed.  If it has,
   *  then it is checked to see if it is up-to-date. (it does not 
   *  kill the old entry (maybe it should)).
   */

  long            filename_id;
  document_table_entry the_document_table_entry;
  long            document_id = next_document_id (db);

/*  FILE* input_stream = s_fopen(filename, "r"); */
  FILE           *input_stream;
  char           *tmpFileName = NULL;

  long            file_position_before_line = 0;
  long            file_position_before_document = 0;
  long            date;
  long            number_of_not_ended_section = 0;
  int             use_url = 0;		    /* use URL for filename ? */

  if (filename != NULL &&
      strlen (filename) > 1 &&
      !strcmp (filename + (strlen (filename) - 2), ".Z"))
    /* it's a .Z file.  First, remove the suffix or many things get confused. */
    filename[(strlen (filename) - 2)] = 0;

  if (filename != NULL &&
      strlen (filename) > 2 &&
      !strcmp (filename + (strlen (filename) - 3), ".gz"))
    /* it's a .gz file.  First, remove the suffix or many things get confused. */
    filename[(strlen (filename) - 3)] = 0;

/* multitype extensions */
/* 
   If dataops->multitype (primary and secondary type) is defined, then
   we need to index filenames that have an extension with the
   dataops->type (primary type) and skip all other files.

   The only problem with this approach is that we may loose files which 
   dont have an instance of the primary file type 
 */
  if ((dataops->multitype != NULL) &&
      strcmp (filename + (strlen (filename) - strlen (dataops->type)), dataops->type)) {
    waislog (WLOG_HIGH, WLOG_INFO, "Skipping file: %s",
	     filename);
    return (-1);
  }
  if (probe_file (filename)) {
    input_stream = s_fopen (filename, "r");
  } else {
    if (probe_file_possibly_compressed (filename)) {
      tmpFileName = s_fzcat (filename);
      if (tmpFileName) {
	input_stream = s_fopen (tmpFileName, "r");
	unlink (tmpFileName);
	free (tmpFileName);
      }
    } else {
      waislog (WLOG_HIGH, WLOG_INDEX,
	       "File %s not readable", filename);
      return (0);		/* silently skip it */
    }
  }
/* end of this long one */

  if (NULL == input_stream) {
    waislog (WLOG_HIGH, WLOG_ERROR,
	     "File %s does not exist", filename);
    /* then the is not a valid file to be indexed */
    return (0);
  }
  if (check_for_file_already_indexed) {
    time_t          time;
    char            full_path[MAX_FILENAME_LEN];

    truename (filename, full_path);
    if (true == filename_in_database (full_path, dataops->type, &time, db)) {
      /* check that it is the same time as this file */
      if (time == file_write_date (filename)) {
	waislog (WLOG_HIGH, WLOG_INDEX,
		 "File %s already indexed", filename);
	s_fclose (input_stream);
	return (0);		/* silently skip it */
      }
    }
  }
  /* Make the current filename accessible via global variables.
   * Increment current_filecount so routines can efficiently detect
   * changes in the current file.
   * -- Prentiss Riddle, Rice ONCS, riddle@rice.edu, 5/6/92
   */

  if (current_filename == NULL)
    current_filename = s_malloc (MAX_FILENAME_LEN + 1);

  if (URL_prefix && !strncmp (filename, URL_trim, MINIMUM (strlen (URL_trim), strlen (filename)))) {
    /* trim capable */
    strcpy (current_filename, URL_prefix);
    strcat (current_filename, filename + strlen (URL_trim));
    use_url = 1;
  } else {
    strncpy (current_filename, filename, MAX_FILENAME_LEN);
  }
  current_filecount++;

  if (check_for_text_file) {
    /* if we need this to be a text file, check the first character
       for a printable character */
    long            ch = fgetc (input_stream);

    /* printf("First character is '%c'\n", ch); */
    if (EOF == ch || (!isprint (ch) && !isspace (ch))) {
      s_fclose (input_stream);
      return (0);
    }
    ungetc (ch, input_stream);
  }
/* multitype extensions */

  /* write out the filename */
  if (dataops->multitype != NULL) {
#ifdef URLDOCID
    if (use_url) {		/* make the URL the filename */
      filename_id =
	write_filename_table_entry (current_filename, dataops->multitype, db);
    } else {
#endif /* URLDOCID */
      filename_id =
	write_filename_table_entry (filename, dataops->multitype, db);
#ifdef URLDOCID
    }
#endif /* URLDOCID */
  } else {
#ifdef URLDOCID
    if (use_url) {		/* make the URL the filename  */
      filename_id =
	write_filename_table_entry (current_filename, dataops->type, db);
    } else {
#endif /* URLDOCID */
      filename_id =
	write_filename_table_entry (filename, dataops->type, db);
#ifdef URLDOCID
    }
#endif /* URLDOCID */
  }

  /*  (if (not *drop_table*) (make_drop_table)) maybe put in later */

  header_flag_1 = NULL;
  the_document_table_entry.filename_id = filename_id;
  the_document_table_entry.start_character = 0;
  the_document_table_entry.document_length = 0;
  the_document_table_entry.number_of_lines = 0;
  the_document_table_entry.date = 0;

#ifdef NEW_WEIGHT
  number_of_bucket_ids = 0;
#endif /* NEW_WEIGHT */

  while (TRUE) {
    long            line_length;
    boolean         newline_terminated;
    char            line[MAX_LINE_LENGTH];
    char            header[MAX_LINE_LENGTH];
    char           *read_line_result;
    boolean         eof;
    long            current_id;

    /* read a line */
    read_line_result = fgets (line, MAX_LINE_LENGTH, input_stream);
    beFriendly ();

    /* eof = feof(input_stream); *//* zero means not eof */
    eof = !read_line_result;

    the_document_table_entry.number_of_lines++;

    header[0] = '\0';		/* set it to the empty string */

    if (eof 
        || ((NULL != dataops->separator_function) && dataops->separator_function (line)) 
        || (keyword_filename != NULL)) {

      /* tell this function that there is not more to process */
      if (keyword_filename != NULL) {
	eof = true;
      }
      /* we are processing a separator, therefore we should
       * finish off the last document, and start a new one
       */
      if (NULL != dataops->finish_header_function) {
	dataops->finish_header_function (header);

/* call Victor Nettoyage :-( */
	(void) cleanHeadline (header);

      }
      if (0 == strlen (header)) {
	char            full_path[1000];
	char            directory[1000];

	if (!URL_prefix) {
	  truename (filename, full_path);
	  sprintf (header, "%s   %s", pathname_name (full_path),
		   pathname_directory (full_path, directory));
	} else
	  strncpy (header, current_filename, MAX_FILENAME_LEN);
      }
      the_document_table_entry.number_of_lines--;	/* dont count separator */
      /* finish off the last */

      finish_document (dataops->extraheaderweight,
		       header, line, document_id,
		       &the_document_table_entry,
		       eof ?	/* if EOF, use file length */
		     file_length (input_stream) : file_position_before_line,
		       file_position_before_document,
		       db, word_position, word_pairs,
#ifndef SOUND
		       dataops->minwordlen);
#else /* SOUND */
		       dataops->minwordlen, dataops->indextype);
#endif /* SOUND */
      /* initialize the next one */
      the_document_table_entry.filename_id = filename_id;
      the_document_table_entry.start_character = file_position_before_line;
      the_document_table_entry.number_of_lines = 1;	/* count separator */
      the_document_table_entry.date = 0;
      the_document_table_entry.document_length = 0;
      file_position_before_document = file_position_before_line;

      document_id = next_document_id (db);

      if (!eof) {		/* not EOF */
	if (NULL != dataops->header_function) {
	  dataops->header_function (line);
	}
	if (dataops->date_function != NULL &&
	    (date = dataops->date_function (line)) > 0)
	  the_document_table_entry.date = date;
	/* dgg -- don't know where this goes. */

	if (dataops->addseparatorwords) {	/* dgg */
	  long            number_of_words;

	  number_of_words = map_over_words (line, document_id, dataops->repeat_weight,
					    file_position_before_line -
					    file_position_before_document,
					    &line_length,
					    &newline_terminated,
					    db,
					    add_word_before_pairs,
					    word_position, word_pairs,
#ifdef SOUND
					    dataops->minwordlen,
					    dataops->indextype);
#else /* not SOUND */
					    dataops->minwordlen);
#endif /* not SOUND */
	  the_document_table_entry.document_length += number_of_words;
	  len_of_files_since_last_delete += number_of_words;
	  len_of_files_since_last_flush += number_of_words;
	} else {
	  line_length = strlen (line);
	  newline_terminated = true;
	}
      }
#ifdef NEW_WEIGHT
      assign_term_weight_for_doc (&number_of_bucket_ids, db);
      if (eof) {
	s_fclose (input_stream);
	return (1);
      }
#else /* not NEW_WEIGHT */
      else {			/* EOF */
	/* printf("closing the file\n"); */
	s_fclose (input_stream);
	return (1);
      }
#endif /* not NEW_WEIGHT */
      number_of_not_ended_section = 0;
    } else {
      /* not a separator or EOF so process the line */
      long            number_of_words;

      if (dataops->date_function != NULL &&
	  the_document_table_entry.date == 0 &&
	  (date = dataops->date_function (line)) > 0)
	the_document_table_entry.date = date;

      if (NULL != dataops->header_function)
	dataops->header_function (line);

      if (index_contents) {
	if (_indexable_section) {
	  if ((field_id >= 0) && (db->number_of_fields > 0)) {
	    current_id = how_index_line (field_id, line,
					 &number_of_not_ended_section,
					 document_id,
					 dataops->repeat_weight,
					 file_position_before_line -
					 file_position_before_document,
					 &line_length,
					 &newline_terminated,
					 db,
					 add_word_before_pairs,
					 field_add_word_before_pairs,
					 word_position, word_pairs,
					 dataops->minwordlen,
					 (dataops->indextype)?dataops->indextype
                                                             :dataops->type);

	    number_of_words = count_words (line, &line_length,
					   &newline_terminated);
	  } else {
	    dataops->indextype = dataops->prev_indextype;
	    number_of_words = map_over_words (line, document_id, dataops->repeat_weight,
					      file_position_before_line -
					      file_position_before_document,
					      &line_length,
					      &newline_terminated,
					      db,
					      add_word_before_pairs,
					      word_position, word_pairs,
#ifdef SOUND
					      dataops->minwordlen,
					      dataops->indextype);
#else /* not SOUND */
					      dataops->minwordlen);
#endif /* not SOUND */
	  }			/* else */
	  if (number_of_words == -1)
	    waislog (WLOG_HIGH, WLOG_ERROR, "map_over_words failed");
	  the_document_table_entry.document_length += number_of_words;
	  len_of_files_since_last_delete += number_of_words;
	  len_of_files_since_last_flush += number_of_words;
	  db->total_word_count += number_of_words;
	} else
	  newline_terminated = 0;
      }
    }
    if (newline_terminated)
      file_position_before_line += (line_length +
				    LENGTH_OF_NEWLINE	/* in case of crlf */
				    - 1		/* fgets gets one newline */
	);
    else
      file_position_before_line = ftell (input_stream);

    /* for debugging
       if(file_position_before_line != ftell(input_stream)) {
       waislog(WLOG_LOW, WLOG_INFO, "ftell: %ld, computed ftell: %ld", 
       ftell(input_stream),
       file_position_before_line);
       }
     */

  }
}

long
field_index_text_file (filename, dataops, db,
		       check_for_text_file,
		       check_for_file_already_indexed,
		       word_position, word_pairs, field_id)
     char           *filename;
     dataopsrec     *dataops;
     database       *db;
     boolean         check_for_text_file;
     boolean         check_for_file_already_indexed;
     boolean         word_position, word_pairs;
     long            field_id;
{
  long            document_id = next_document_id (db);
  long            file_position_before_line = 0;
  long            file_position_before_document = 0;
  long            number_of_not_ended_section = 0;
  long            i;
  FILE           *input_stream = NULL;
  char           *tmpFileName = NULL;
  char            file[256];

  if (filename != NULL &&
      strlen (filename) > 1 &&
      !strcmp (filename + (strlen (filename) - 2), ".Z"))
    /* 
     * it's a .Z file.  First, remove the suffix or many things get confused. 
     */
    filename[(strlen (filename) - 2)] = 0;
  if (probe_file (filename)) {
    input_stream = s_fopen (filename, "r");
  } else if (probe_file_possibly_compressed (filename)) {
    tmpFileName = s_fzcat (filename);
    if (tmpFileName) {
      input_stream = s_fopen (tmpFileName, "r");
      unlink (tmpFileName);
      free (tmpFileName);
    }
  }
  /* Make the current filename accessible via global variables.
   * Increment current_filecount so routines can efficiently detect
   * changes in the current file.
   * -- Prentiss Riddle, Rice ONCS, riddle@rice.edu, 5/6/92
   */

  if (current_filename == NULL)
    current_filename = s_malloc (MAX_FILENAME_LEN + 1);

  if (URL_prefix && !strncmp (filename, URL_trim, MINIMUM (strlen (URL_trim), strlen (filename)))) {
    /* trim capable */
    strcpy (current_filename, URL_prefix);
    strcat (current_filename, filename + strlen (URL_trim));
  } else
    strncpy (current_filename, filename, MAX_FILENAME_LEN);

  if (check_for_text_file) {
    /* if we need this to be a text file, check the first character
       for a printable character */
    long            ch = fgetc (input_stream);

    /* printf("First character is '%c'\n", ch); */
    if (EOF == ch || (!isprint (ch) && !isspace (ch))) {
      s_fclose (input_stream);
      return (0);
    }
    ungetc (ch, input_stream);
  }
  if (NULL == input_stream) {
    waislog (WLOG_HIGH, WLOG_ERROR,
	     "File %s does not exist", filename);
    /* then the is not a valid file to be indexed */
    return (0);
  }
#ifdef NEW_WEIGHT
  number_of_bucket_ids = 0;
#endif /* NEW_WEIGHT */

  while (TRUE) {
    long            line_length;
    boolean         newline_terminated;
    char            line[MAX_LINE_LENGTH];
    char            header[MAX_LINE_LENGTH];
    char           *read_line_result;
    boolean         eof;
    long            current_id;

    /* printf("ftell: %ld\n", ftell(input_stream)); */
    /* read a line */
    read_line_result = fgets (line, MAX_LINE_LENGTH, input_stream);
    beFriendly ();

    eof = !read_line_result;

    if (eof ||
	((NULL != dataops->separator_function)
	 && dataops->separator_function (line))
	|| (keyword_filename != NULL)) {

      file_position_before_document = file_position_before_line;

      document_id = ++(db->doc_table_allocated_entries);

      if (!eof) {		/* not EOF */
	if (dataops->addseparatorwords) {	/* dgg */
	  long            number_of_words;

	  number_of_words = map_over_words (line, document_id,
					    dataops->repeat_weight,
					    file_position_before_line -
					    file_position_before_document,
					    &line_length,
					    &newline_terminated,
					    db,
					    add_word_before_pairs,
					    word_position, word_pairs,
#ifdef SOUND
					    dataops->minwordlen,
					    dataops->indextype);
#else /* not SOUND */
					    dataops->minwordlen);
#endif /* not SOUND */

	} else {
	  line_length = strlen (line);
	  file_position_before_line += line_length;
	  newline_terminated = true;
	}
      }
#ifdef NEW_WEIGHT
      assign_term_weight_for_doc (&number_of_bucket_ids, db);
      if (eof) {
	s_fclose (input_stream);
	return (1);
      }
#else /* not NEW_WEIGHT */
      else {			/* EOF */
	/* printf("closing the file\n"); */
	s_fclose (input_stream);
	return (1);
      }
#endif /* not NEW_WEIGHT */
      number_of_not_ended_section = 0;
    } else {
      /* not a separator or EOF so process the line */
      long            number_of_words;

      if (index_contents) {
	if (_indexable_section) {

	  current_id = how_index_line (field_id, line,
				       &number_of_not_ended_section,
				       document_id,
				       dataops->repeat_weight,
				       file_position_before_line -
				       file_position_before_document,
				       &line_length,
				       &newline_terminated,
				       db,
				       add_word_before_pairs,
				       field_add_word_before_pairs,
				       word_position, word_pairs,
				       dataops->minwordlen,
				       (dataops->indextype)?dataops->indextype
                                                            :dataops->type);
	  number_of_words = count_words (line, &line_length,
					 &newline_terminated);
	  if (number_of_words == -1)
	    waislog (WLOG_HIGH, WLOG_ERROR, "map_over_words failed");
	} else
	  newline_terminated = 0;
      }
      if (newline_terminated)
	file_position_before_line += (line_length +
				      LENGTH_OF_NEWLINE		/* in case of crlf */
				      - 1	/* fgets gets one newline */
	  );
      else
	file_position_before_line = ftell (input_stream);
    }
  }
}

/* ---------------------------------------------------------------- */

/* return TRUE if it is a directory, FALSE otherwise */
boolean
directoryp (file)
     char           *file;

{
#ifdef THINK_C
  return (false);
#else /* not THINK_C */
  struct stat     stbuf;

  if (stat (file, &stbuf) == -1)
    return (FALSE);
  if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
    return (true);
  return (FALSE);
#endif /* not THINK_C */
}

/* return true if it is a file, FALSE otherwise */
boolean
filep (file)
     char           *file;
{
#ifdef THINK_C
  return (probe_file (file));
#else /* not THINK_C */
  struct stat     stbuf;

  if (stat (file, &stbuf) == -1)
    return (FALSE);
  if (!((stbuf.st_mode & S_IFMT) == S_IFDIR))
    return (true);
  return (FALSE);
#endif /* not THINK_C */
}

/* recursively indexes the directory specified. 
 * If it is a file, then index it. 
 */
void
index_directory (file, dataops, db,
		 check_for_text_file,
		 check_for_file_already_indexed,
		 word_position, word_pairs
		 ,index_next_field, field_id
)
     char           *file;
     dataopsrec     *dataops;
     database       *db;
     boolean         check_for_text_file;
     boolean         check_for_file_already_indexed;
     boolean         word_position, word_pairs;
     boolean         index_next_field;
     long            field_id;
{
#ifndef THINK_C
  Direntry_t **list;
  long            i, j;
  long            mesg;

  if (filep (file)) {
    waislog (WLOG_MEDIUM, WLOG_INDEX, "File: %s", file);
    if (index_next_field && db->number_of_fields) {
      if ((mesg = field_index_text_file (file, dataops, db,
					 check_for_text_file,
					 check_for_file_already_indexed,
			       word_position, word_pairs, field_id)) <= 0) {
	waislog (WLOG_HIGH, WLOG_ERROR, "File not indexed: %s", file);
	if (mesg == -1) {
	  closeDatabase (db);
	  exit (0);
	}
      }
    } else {
      if ((mesg = index_text_file (file, dataops, db,
				   check_for_text_file,
				   check_for_file_already_indexed,
			       word_position, word_pairs, field_id)) <= 0) {
	waislog (WLOG_HIGH, WLOG_ERROR, "File not indexed: %s", file);
	if (mesg == -1) {
	  closeDatabase (db);
	  exit (0);
	}
      }
    }
  } else if (directoryp (file)) {
    if ((i = scandir (file, &list, NULL, NULL)) < 0) {
      return;
    }
    for (j = 0; j < i; j++) {
      char            name[1000];	    /* max filename size */

      if (strcmp (list[j]->d_name, ".") == 0
	  || strcmp (list[j]->d_name, "..") == 0
	)
	continue;

      strcpy (name, file);	/* copy the filename into the name variable */
      strcat (name, "/");
      strcat (name, list[j]->d_name);
      index_directory (name, dataops, db,
		       check_for_text_file,
		       check_for_file_already_indexed,
		       word_position, word_pairs
		       ,index_next_field, field_id
	);
    }
    if (list != NULL) {
      for (j = 0; j < i; j++)
	if (list[j] != NULL)
	  free ((char *) list[j]);
      free ((char *) list);
    }
#endif /* not THINK_C */
  }
}

/* returns a pointer to a string with good stuff */
char           *
cleanHeadline (headline)
     char           *headline;
{
  long            length = strlen (headline) + 1;	/* include the trailing null */
  long            i, j;
  Boolean         spaceFlag = false;

#ifdef do_delete_leading_spaces
  /* delete leading spaces */
  for (i = 0L; i < strlen (headline); i++) {
/*    if(isprint(headline[i])){ */
    if (isgraph (headline[i])) {
      break;
    }
  }
  /* and move it */
  memcpy (headline, headline + i, length);

  /* 
     ** - replace all the \n and \r with a space, avoid putting 
     **   two spaces one after the other
   */
  headline = headline + i;
#endif /* do_delete_leading_spaces */

  /* replace carriage returns and line feeds */
  for (i = 0L, j = 0L; i < strlen (headline) + 1; i++) {
    if ((headline[i] != '\r') && (headline[i] != '\n')) {
      headline[j++] = headline[i];
      spaceFlag = false;
    } else {

      if (spaceFlag == true) {
	j++;
      } else {
	headline[j++] = ' ';
      }

      spaceFlag = true;
    }
  }

  /* delete trailing stuff */
  for (i = strlen (headline) - 1L; i > 0; i--) {
/*    if(isprint(headline[i])){ */
    if (isgraph (headline[i])) {
      break;
    }
    headline[i] = '\0';
  }

  return (headline);
}
