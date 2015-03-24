/*                               -*- Mode: C -*- 
 * 
 * irinv.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Wed Jun 28 14:29:45 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Oct 11 19:31:02 1995
 * Language        : C
 * Update Count    : 6
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
 * $Log: irinv.c,v $
 * Revision 2.0.1.2  1995/10/11 20:12:34  pfeifer
 * patch37: Repassed through GNU indet. No change.
 *
 * Revision 2.0.1.1  1995/09/15  09:44:58  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  07:59:58  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.17  1995/06/28  12:29:14  pfeifer
 * fixifdef | unifdef -DFIELDS | indent
 *
 * Revision 1.16  1995/05/23  12:15:22  pfeifer
 * Fix by Tom Snee
 *
 * Revision 1.15  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.14  1995/04/17  13:26:55  pfeifer
 * fixes by Tom Snee
 *
 * Revision 1.13  1995/04/17  13:17:18  huynh1
 * Release 1.1.2
 *
 * Revision 1.12  1995/02/07  15:59:32  huynh1
 * conditions: number_of_words_1 (_2) < 0 change to
 *             number_of_words_1 (_2) < 1
 *
 * Revision 1.11  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.10  1994/07/13  09:55:23  pfeifer
 * Negative numerics. Syntable freed to early
 *
 * Revision 1.9  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.8  1994/06/03  10:50:01  huynh1
 * bug in (field_)merge_index_file corrected.
 *
 * Revision 1.7  1994/05/26  15:09:57  huynh1
 * no change. beta
 *
 * Revision 1.6  1994/05/20  12:56:18  pfeifer
 * beta
 *
 * Revision 1.5  1994/04/06  23:50:22  pfeifer
 * 08
 *
 * Revision 1.4  1994/03/23  12:58:14  huynh1
 * init_add_word modified.
 * patchlevel 07.
 *
 * Revision 1.3  1994/03/08  20:43:00  huynh1
 * Patchlevel 04
 *
 * Revision 1.2  1994/02/14  10:42:51  huynh1
 * new code for field concept added.
 *
 * Revision 1.3  1993/09/23  03:00:07  pfeifer
 * undid swap.
 * Fixed bug with string compare (strcmp) for ISO-Chars
 *
 * Revision 1.2  1993/09/23  02:12:20  pfeifer
 * swaped from_version and into_version merge_index_file
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.28  92/04/28  16:55:50  morris
 * added boolean to serial engine
 * 
 * Revision 1.27  92/03/19  09:34:26  morris
 * fixed the dictionary header to accurately indicate the number of blocks
 * 
 * Revision 1.26  92/03/01  16:11:16  brewster
 * took out the analyze_hashtable_distribution
 * 
 * Revision 1.25  92/02/12  13:27:31  jonathan
 * Added "$Log: irinv.c,v $
 * Added "Revision 2.0.1.2  1995/10/11 20:12:34  pfeifer
 * Added "patch37: Repassed through GNU indet. No change.
 * Added "
 * Revision 2.0.1.1  1995/09/15  09:44:58  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  07:59:58  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.17  1995/06/28  12:29:14  pfeifer
 * fixifdef | unifdef -DFIELDS | indent
 *
 * Revision 1.16  1995/05/23  12:15:22  pfeifer
 * Fix by Tom Snee
 *
 * Revision 1.15  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.14  1995/04/17  13:26:55  pfeifer
 * fixes by Tom Snee
 *
 * Revision 1.13  1995/04/17  13:17:18  huynh1
 * Release 1.1.2
 *
 * Revision 1.12  1995/02/07  15:59:32  huynh1
 * conditions: number_of_words_1 (_2) < 0 change to
 *             number_of_words_1 (_2) < 1
 *
 * Revision 1.11  1994/09/02  14:34:21  pfeifer
 * fixed overlapping memory copies
 *
 * Revision 1.10  1994/07/13  09:55:23  pfeifer
 * Negative numerics. Syntable freed to early
 *
 * Revision 1.9  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.8  1994/06/03  10:50:01  huynh1
 * bug in (field_)merge_index_file corrected.
 *
 * Revision 1.7  1994/05/26  15:09:57  huynh1
 * no change. beta
 *
 * Revision 1.6  1994/05/20  12:56:18  pfeifer
 * beta
 *
 * Revision 1.5  1994/04/06  23:50:22  pfeifer
 * 08
 *
 * Revision 1.4  1994/03/23  12:58:14  huynh1
 * init_add_word modified.
 * patchlevel 07.
 *
 * Revision 1.3  1994/03/08  20:43:00  huynh1
 * Patchlevel 04
 *
 * Revision 1.2  1994/02/14  10:42:51  huynh1
 * new code for field concept added.
 *
 * Revision 1.3  1993/09/23  03:00:07  pfeifer
 * undid swap.
 * Fixed bug with string compare (strcmp) for ISO-Chars
 *
 * Revision 1.2  1993/09/23  02:12:20  pfeifer
 * swaped from_version and into_version merge_index_file
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.28  92/04/28  16:55:50  morris
 * added boolean to serial engine
 * 
 * Revision 1.27  92/03/19  09:34:26  morris
 * fixed the dictionary header to accurately indicate the number of blocks
 * 
 * Revision 1.26  92/03/01  16:11:16  brewster
 * took out the analyze_hashtable_distribution
 * " so RCS will put the log message in the header
 * 
 */

/* Inverted file accessor functions. 

   Main functions:
   finished_add_word

 */

/* #include <string.h> /* for memset() */

#include "cutil.h"
#include "futil.h"
#include "irhash.h"
#include "hash.h"
#include "panic.h"
#include "irfiles.h"
#include "irext.h"

/* ================== */
/* === Index file === */
/* ================== */

/*This is an implementation of the inverted file itself.
 * An index_block_number is the position in the file that the
 * entry starts.
 * Index block 0 is the null pointer.
 * The header contains the number of different words in the file.
 *
 */

/* the format of an index block is:
 *  in the first byte:
 *  INDEX_BLOCK_FULL_FLAG 
 *    || INDEX_BLOCK_NOT_FULL_FLAG
 *    || INDEX_BLOCK_DICTIONARY_FLAG
 * if index_block_full_flag
 *  next_index_block
 *  index_block_size
 *  stuff
 *  (the number of valid entries is index_block_size - index_block_header_size)
 * if index_block_not_full_flag
 *  number_of_valid_entries
 *  index_block_size
 *  stuff
 * if index_block_dictionary_flag
 *  last_index_block      NEXT_INDEX_BLOCK_SIZE (0 when this is the last)
 *  index_block_size      INDEX_BLOCK_SIZE_SIZE
 *  number_of_occurances  NUMBER_OF_OCCURANCES_SIZE
 *  word (followed by \n)
 *
 *
 * "Stuff" is a list of word occurances of the format:
 * (this is written in irhash.c and read in sersrch.c)
 *      doc_id
 *      character_position
 *      weight
 *      doc_id
 *      character_position
 *      weight 
 *      etc.
 *
 *   It should be (probably in release 9):
 *      doc_id (with high bit set)
 *      weight
 *      character_position
 *      ...
 *      character_position
 *      doc_id
 *      weight
 *      character_position
 *      etc.
 *      
 */

long
write_dictionary_index_block (number_of_occurances, word, stream)
     long            number_of_occurances;
     char           *word;
     FILE           *stream;

/* returns a pointer to the index block allocated */
{
  /* this assumes that the stream is positioned at the end */
  long            before_length = ftell (stream);	/* file_length(stream); */

  long            index_block_size =
  INDEX_BLOCK_FLAG_SIZE +
  NEXT_INDEX_BLOCK_SIZE +
  INDEX_BLOCK_SIZE_SIZE +
  NUMBER_OF_OCCURANCES_SIZE +
  strlen (word) +
  strlen ("\n");			    /* this is done this way for PC's (necessary?) */

  /* printf("writing dict entry to position %ld\n", before_length); */
  /* grow the file */
  /* in this implementation, the file is always filled by the fwrite,
   * so it will grow itself.
   * grow_file(stream, before_length + how_large);
   * file length leaves the stream at the end, so we are all set.
   * s_fseek(stream, before_length, SEEK_SET);
   */
  write_bytes (INDEX_BLOCK_DICTIONARY_FLAG, INDEX_BLOCK_FLAG_SIZE,
	       stream);
  write_bytes (0L, NEXT_INDEX_BLOCK_SIZE, stream);
  write_bytes (index_block_size, INDEX_BLOCK_SIZE_SIZE, stream);

  write_bytes (number_of_occurances, NUMBER_OF_OCCURANCES_SIZE,
	       stream);
  if (strlen (word) > MAX_WORD_LENGTH)	/* for debugging */
    waislog (WLOG_HIGH, WLOG_ERROR,
	     "Word: '%s' is too long (%ld characters) for some reason.",
	     word, strlen (word));

  fprintf (stream, "%s\n", word);

  /* just to check
     { long after_length = ftell(stream);
     if(after_length - before_length != index_block_size){
     waislog(WLOG_HIGH, WLOG_ERROR, 
     "dictionary entry size is %ld, and we thought %ld",
     after_length - before_length, index_block_size);
     }
     }
   */

  return (before_length);
}

#ifdef everneeded
static long     modify_dictionary_index_block
                _AP ((long index_block, long last_index_block, long number_of_occurances,
		      FILE * stream));

static long 
modify_dictionary_index_block (index_block, last_index_block, number_of_occurances, stream)
     long            index_block;
     long            last_index_block;
     long            number_of_occurances;
     FILE           *stream;

     /* this does not allow one to change the word itself, only the entries 
        around it.  It will panic if the index_block is not a valid block.
        This returns the the stream to pointing at the end of the file.
      */
{
  s_fseek (stream, index_block, SEEK_SET);
  if (INDEX_BLOCK_DICTIONARY_FLAG != read_bytes (INDEX_BLOCK_FLAG_SIZE, stream))
    panic ("the index block %ld is not a legal dictionary block",
	   index_block);
  write_bytes (number_of_occurances, NUMBER_OF_OCCURANCES_SIZE,
	       stream);
  write_bytes (last_index_block, NEXT_INDEX_BLOCK_SIZE, stream);
  read_bytes (INDEX_BLOCK_SIZE_SIZE, stream);	/* ignore it */
  write_bytes (number_of_occurances, NUMBER_OF_OCCURANCES_SIZE,
	       stream);
  s_fseek (stream, 0L, SEEK_END);
}

#endif /* everneeded */

static long     next_dictionary_index_block
                _AP ((long *index_block_size, long *number_of_occurances, char *word,
		      FILE * stream));

static long
next_dictionary_index_block (index_block_size, number_of_occurances, word, stream)
     long           *index_block_size;
     long           *number_of_occurances;
     char           *word;
     FILE           *stream;
{
  /* this read the dictionary index block from the index stream.
     It assumes the stream is positioned at the start of a dictionary
     block, and will return non-0 if it is not.
     returns 0 if it succeeds.
     returns -1 if it is at the of a file.
     returns -2 if it read something strange.
     Always sets word length to 0 if it fails. */
  long            index_block_flag;
  char            temp[MAX_WORD_LENGTH + 2];

  word[0] = '\0';

  index_block_flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, stream);
  if (index_block_flag == EOF) {
    /* not an error, it is the way it knows it is done 
       waislog(WLOG_HIGH, WLOG_ERROR, "Hit the end of the inverted file too early");
     */
    return (-1);
  }
  if (index_block_flag != INDEX_BLOCK_DICTIONARY_FLAG) {
    waislog (WLOG_HIGH, WLOG_ERROR,
	"Did not find a dictionary entry, flag is %ld at file position %ld",
	     index_block_flag, ftell (stream) - INDEX_BLOCK_FLAG_SIZE);
    return (-2);
  }
  (void) read_bytes (NEXT_INDEX_BLOCK_SIZE, stream);
  *index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, stream);
  *number_of_occurances = read_bytes (NUMBER_OF_OCCURANCES_SIZE,
				      stream);
  fgets (temp, MAX_WORD_LENGTH + 2, stream);	/* 2 is for the \n and '\0' */

  /* trim the \n */
  if (temp[strlen (temp) - 1] == '\n') {
    temp[strlen (temp) - 1] = '\0';
  }
  strcpy (word, temp);
  /* printf("Merging word '%s'\n", word); */
  return (0);
}

long
read_dictionary_index_block (index_block,
			     last_index_block,
			     index_block_size,
			     number_of_occurances,
			     word,
			     stream)
     long            index_block;
     long           *last_index_block;
     long           *index_block_size;
     long           *number_of_occurances;
     char           *word;
     FILE           *stream;
{
  /* this read the dictionary index block from the index stream.
     returns 0 if it succeeds. */
  long            answer;

  s_fseek (stream, index_block, SEEK_SET);
  if (0 > (answer = next_dictionary_index_block (index_block_size,
						 number_of_occurances,
						 word,
						 stream)))
    panic ("read dictionary block %ld failed", index_block);

  *last_index_block = 0;
  return (answer);
}

#ifdef LITERAL
long 
allocate_index_block (how_large, docs_incl_term, stream)
     long            how_large;
     long            docs_incl_term;
     FILE           *stream;

#else /* not LITERAL */
long 
allocate_index_block (how_large, stream)
     long            how_large;
     FILE           *stream;

#endif /* not LITERAL */
{
  /* This returns a pointer for an index block.
     It creates the space and writes the header.
     how_large includes the header.
     Returns the block_number (the byte address of first byte in the block */
  /* this assumes that the stream is positioned at the end */
  long            before_length = ftell (stream);	/* file_length(stream); */

  /* grow the file */
  /* in this implementation, the file is always filled by the fwrite,
   * so it will grow itself.
   * grow_file(stream, before_length + how_large);
   * file length leaves the stream at the end, so we are all set.
   * s_fseek(stream, before_length, SEEK_SET);
   */
  write_bytes (INDEX_BLOCK_FULL_FLAG,	/* in this version all are full */
	       INDEX_BLOCK_FLAG_SIZE,
	       stream);
#ifdef LITERAL
  write_bytes (docs_incl_term, NEXT_INDEX_BLOCK_SIZE, stream);
#else /* not LITERAL */
  write_bytes (0L, NEXT_INDEX_BLOCK_SIZE, stream);
#endif /* not LITERAL */
  write_bytes (how_large, INDEX_BLOCK_SIZE_SIZE, stream);
  return (before_length);
}

#ifdef testing

static void scan_index_blocks _AP ((char *filename, boolean verbose));

static void 
scan_index_blocks (filename, verbose)
     char           *filename;
     boolean         verbose;
{
  /* this is a debugging routine for checking the inverted index file */
  long            current_index_block = INDEX_HEADER_SIZE;
  FILE           *stream = s_fopen (filename, "rb");
  long            length = file_length (stream);

  s_fseek (stream, current_index_block, SEEK_SET);
  printf ("File length %ld\n", length);

  while (current_index_block < length) {
    long            flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, stream);
    long            next_index_block = read_bytes (NEXT_INDEX_BLOCK_SIZE, stream);
    long            index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, stream);

    if (flag == INDEX_BLOCK_DICTIONARY_FLAG) {
      long            last_index_block;
      long            index_block_size;
      long            number_of_occurances;
      char            word[MAX_WORD_LENGTH + 1];

      if (0 > read_dictionary_index_block (current_index_block,
					   &last_index_block,
					   &index_block_size,
					   &number_of_occurances,
					   word,
					   stream))
	panic ("read dictionary index block failed");
      if (verbose)
	printf ("%5ld: size %3ld Dictionary entry '%s', number of occurances %ld last block %ld\n",
		current_index_block, index_block_size, word,
		number_of_occurances, next_index_block);
    } else if (flag == INDEX_BLOCK_NOT_FULL_FLAG) {
      if (verbose)
	printf ("%5ld: size %3ld Not full, valid entries %ld\n",
		current_index_block, index_block_size, next_index_block);
    } else if (flag == INDEX_BLOCK_FULL_FLAG) {
      if (verbose)
	printf ("%5ld: size %3ld full block, next block %ld\n",
		current_index_block, index_block_size, next_index_block);
    } else
      panic ("bad entry %ld (ftell %ld), flag was %ld",
	     current_index_block,
	     ftell (stream), flag);
    current_index_block += index_block_size;
    s_fseek (stream, current_index_block, SEEK_SET);
  }
  s_fclose (stream);
}

#endif /* testing */

#define COPY_BLOCK_BUFFER_LENGTH 1000

static long copy_stream _AP ((FILE * from_stream, FILE * to_stream, long n));

static long 
copy_stream (from_stream, to_stream, n)
     FILE           *from_stream;
     FILE           *to_stream;
     long            n;
{
  char            buffer[COPY_BLOCK_BUFFER_LENGTH];

  while (n > 0) {
    /* keep reading until we are done */
    long            amount_read = fread (buffer, sizeof (char),
				      MINIMUM (n, COPY_BLOCK_BUFFER_LENGTH),
					 from_stream);

    if (amount_read == 0 || amount_read == EOF)
      return (-1);
    if (amount_read != fwrite (buffer, sizeof (char), amount_read, to_stream))
                      return (-1);

    n -= amount_read;
  }
  return (0);
}

#if (INDEX_BLOCK_SIZE_SIZE < 4)
#define MAX_INDEX_BLOCK_SIZE ((unsigned long) (1 << (8 * INDEX_BLOCK_SIZE_SIZE)) - 1)
#else
#define MAX_INDEX_BLOCK_SIZE (0xFFFFFFFF - 1)
#endif

static void merge_blocks _AP ((char *word, FILE * from_stream_1,
			       FILE * from_stream_2, FILE * to_stream));

static void 
merge_blocks (word, from_stream_1, from_stream_2, to_stream)
     char           *word;
     FILE           *from_stream_1;
     FILE           *from_stream_2;
     FILE           *to_stream;

/* puts from_stream_1 first into to_stream then from_stream_2 */
{
  long            flag;
  long            index_block_size;
  long            other_block_size;

#ifdef LITERAL
  long            docs_incl_term_1;
  long            docs_incl_term_2;

#endif /* LITERAL */

  flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, from_stream_1);
#ifdef LITERAL
  docs_incl_term_1 = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_1);
#else /* not LITERAL */
  (void) read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_1);
#endif /* not LITERAL */
  index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, from_stream_1);

  if (flag == EOF)
    return;
  if (flag != INDEX_BLOCK_FULL_FLAG)
    panic ("the next index block is not a full block");

  flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, from_stream_2);
#ifdef LITERAL
  docs_incl_term_2 = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_2);
#else /* not LITERAL */
  (void) read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_2);
#endif /* not LITERAL */
  other_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, from_stream_2);

  if (flag != INDEX_BLOCK_FULL_FLAG)
    panic ("the next index block is not a full block");

  write_bytes (flag, INDEX_BLOCK_FLAG_SIZE, to_stream);
#ifdef LITERAL
  write_bytes (docs_incl_term_1 + docs_incl_term_2,
	       NEXT_INDEX_BLOCK_SIZE, to_stream);
#else /* not LITERAL */
  write_bytes (0L, NEXT_INDEX_BLOCK_SIZE, to_stream);
#endif /* not LITERAL */
  if ((unsigned long) (index_block_size + other_block_size) >
      MAX_INDEX_BLOCK_SIZE) {
    /* then the block is too large to be represented in the 
       index_block_size_size.  This routine takes the radical step to
       eliminate it completely.  The "right" thing to do is to
       chain blocks, but I dont think it is worth it since this should not
       happen very often.  If it does happen often then bump up
       INDEX_BLOCK_SIZE_SIZE. */

#ifdef AUTO_STOPWORDS
    waislog (WLOG_LOW, WLOG_INFO,
	     "Can not merge the index block for %s, since it would create one that is too big. Adding it into the stoplist external.", word);
    add_auto_stopword (word);
#else /* not AUTO_STOPWORDS */
    waislog (WLOG_LOW, WLOG_INFO,
	     "Can not merge the index block for %s, since it would create one that is too big.  Deleting it.", word);
#endif /* not AUTO_STOPWORDS */

    write_bytes (INDEX_BLOCK_HEADER_SIZE, INDEX_BLOCK_SIZE_SIZE, to_stream);
    s_fseek (from_stream_1, index_block_size - INDEX_BLOCK_HEADER_SIZE, SEEK_CUR);
    s_fseek (from_stream_2, other_block_size - INDEX_BLOCK_HEADER_SIZE, SEEK_CUR);
  } else {			/* copy away */
    write_bytes (index_block_size + other_block_size - INDEX_BLOCK_HEADER_SIZE,
		 INDEX_BLOCK_SIZE_SIZE, to_stream);
    copy_stream (from_stream_1, to_stream, index_block_size - INDEX_BLOCK_HEADER_SIZE);
    copy_stream (from_stream_2, to_stream, other_block_size - INDEX_BLOCK_HEADER_SIZE);
  }
}

static void make_dummy_block _AP ((FILE * from_stream_1, FILE * from_stream_2,
				   FILE * to_stream));

static void 
make_dummy_block (from_stream_1, from_stream_2, to_stream)
     FILE           *from_stream_1;
     FILE           *from_stream_2;
     FILE           *to_stream;

/* deletes block from_stream_1 and from_stream_2 and makes a 0 length
   block on to_stream */
{
  long            flag;
  long            index_block_size;
  long            other_block_size;

#ifdef LITERAL
  long            docs_incl_term_1;
  long            docs_incl_term_2;

#endif /* LITERAL */

  flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, from_stream_1);
#ifdef LITERAL
  docs_incl_term_1 = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_1);
#else /* not LITERAL */
  (void) read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_1);
#endif /* not LITERAL */
  index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, from_stream_1);

  if (flag == EOF)
    return;
  if (flag != INDEX_BLOCK_FULL_FLAG)
    panic ("the next index block is not a full block");

  flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, from_stream_2);
#ifdef LITERAL
  docs_incl_term_2 = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_2);
#else /* not LITERAL */
  (void) read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream_2);
#endif /* not LITERAL */
  other_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, from_stream_2);
  if (flag != INDEX_BLOCK_FULL_FLAG)
    panic ("the next index block is not a full block");

  write_bytes (flag, INDEX_BLOCK_FLAG_SIZE, to_stream);
#ifdef LITERAL
  write_bytes (docs_incl_term_1 + docs_incl_term_2,
	       NEXT_INDEX_BLOCK_SIZE, to_stream);
#else /* not LITERAL */
  write_bytes (0L, NEXT_INDEX_BLOCK_SIZE, to_stream);
#endif /* not LITERAL */
  write_bytes (INDEX_BLOCK_HEADER_SIZE, INDEX_BLOCK_SIZE_SIZE, to_stream);
  s_fseek (from_stream_1, index_block_size - INDEX_BLOCK_HEADER_SIZE, SEEK_CUR);
  s_fseek (from_stream_2, other_block_size - INDEX_BLOCK_HEADER_SIZE, SEEK_CUR);
}

static void copy_block _AP ((FILE * from_stream, FILE * to_stream));

static void 
copy_block (from_stream, to_stream)
     FILE           *from_stream;
     FILE           *to_stream;

/* copies an index block from one stream to another */
{				/* copies an index block from from_stream to to_stream */
  long            flag;

#ifdef LITERAL
  long            docs_incl_term;

#else /* not LITERAL */
  long            next_index_block;

#endif /* not LITERAL */
  long            index_block_size;

  flag = read_bytes (INDEX_BLOCK_FLAG_SIZE, from_stream);
#ifdef LITERAL
  docs_incl_term = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream);
#else /* not LITERAL */
  next_index_block = read_bytes (NEXT_INDEX_BLOCK_SIZE, from_stream);
#endif /* not LITERAL */
  index_block_size = read_bytes (INDEX_BLOCK_SIZE_SIZE, from_stream);

  if (flag == EOF)
    return;
  write_bytes (flag, INDEX_BLOCK_FLAG_SIZE, to_stream);
#ifdef LITERAL
  write_bytes (docs_incl_term, NEXT_INDEX_BLOCK_SIZE, to_stream);
#else /* not LITERAL */
  write_bytes (next_index_block, NEXT_INDEX_BLOCK_SIZE, to_stream);
#endif /* not LITERAL */
  write_bytes (index_block_size, INDEX_BLOCK_SIZE_SIZE, to_stream);
  /* copy the real stuff */
  copy_stream (from_stream, to_stream,
	       index_block_size - INDEX_BLOCK_HEADER_SIZE);
}

/* ================== */
/* === Merge File === */
/* ================== */

static boolean merge_index_file _AP ((long from_version, long into_version,
				      database * db,
				      boolean generate_dictionary));

/* Merges a index file (from_version) into another (into_version).
   Returns true if successful */

static boolean 
merge_index_file (into_version, from_version,
		  db, generate_dictionary)
     long            into_version;
     long            from_version;
     database       *db;
     boolean         generate_dictionary;

/* 
   This is done by merging both into version -1 and then, 
   renames it to into_version, then deletes from_version. */
{
  char            input_filename_1[MAX_FILENAME_LEN];	/* into_version file */
  char            input_filename_2[MAX_FILENAME_LEN];	/* from_version file */
  char            output_filename[MAX_FILENAME_LEN];	/* version -1 */

  FILE           *input_stream_1;
  FILE           *input_stream_2;
  FILE           *output_stream;
  char            input_word_1[MAX_WORD_LENGTH + 1];
  char            input_word_2[MAX_WORD_LENGTH + 1];

  if (generate_dictionary) {
    if (into_version == -2)
      waislog (WLOG_MEDIUM, WLOG_INDEX,
       "Merging version %ld into master version and generating dictionary.",
	       from_version);
    else
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "Merging version %ld into %ld and generating dictionary.",
	       from_version, into_version);
  } else {
    waislog (WLOG_MEDIUM, WLOG_INDEX, "Merging version %ld and %ld",
	     into_version, from_version);
  }

  index_filename_with_version (into_version, input_filename_1, db);
  index_filename_with_version (from_version, input_filename_2, db);
  index_filename_with_version (-1L, output_filename, db);

  if (NULL == (input_stream_1 = s_fopen (input_filename_1, "rb")))
    return (false);

  if (NULL == (input_stream_2 = s_fopen (input_filename_2, "rb")))
    return (false);

  if (NULL == (output_stream = s_fopen (output_filename, "wb")))
    return (false);

  {
    long            index_block_size_1;
    long            number_of_occurances_1;
    long            number_of_words_1;
    long            index_block_size_2;
    long            number_of_occurances_2;
    long            number_of_words_2;

    /* leave room for the number_of_words in the output */
    s_fseek (output_stream, INDEX_HEADER_SIZE, SEEK_SET);

    /* read the number of words in the 2 input files, this is the maximum 
       number of words that can be in the dictionary,  It is used by 
       init_dict_file to set asside space for the dictionary_header_block.
       It is very likley that some of the words are duplicates, so the actual
       size of the header_block will be smaller.  But we never know until we
       do the merge, so we have to give enough space just in case.
       NOTE - in the case where generate_dictioary is false, we still need
       to do this because we need to skip the header_size in each of the
       input streams 
     */
    number_of_words_1 = read_bytes (INDEX_HEADER_SIZE, input_stream_1);
    number_of_words_2 = read_bytes (INDEX_HEADER_SIZE, input_stream_2);
    if ((number_of_words_1 > 0) && (number_of_words_2 < 1))
      db->number_of_words = number_of_words_1 + 1;
    else if ((number_of_words_1 < 1) && (number_of_words_2 > 0))
      db->number_of_words = number_of_words_2 + 1;
    else if ((number_of_words_1 > 0) && (number_of_words_2 > 0))
      db->number_of_words = number_of_words_1 + number_of_words_2 + 1;

    if (db->number_of_words == 0) {
      if ((number_of_words_1 == 1 && number_of_words_2 < 1) ||
	  (number_of_words_1 < 1 && number_of_words_2 == 1))
	db->number_of_words = 1;
    }
    /* db->number_of_words = read_bytes(INDEX_HEADER_SIZE,input_stream_1) +
     *                       read_bytes(INDEX_HEADER_SIZE,input_stream_2);
     */

    if (generate_dictionary) {	/* allocate the header block if necessary */
      init_dict_file_for_writing (db);
    }
    /* now that the dictioanry_header_block is allocated, we can start counting
       for real */
    db->number_of_words = 0;

    if (-1 > next_dictionary_index_block (&index_block_size_1,
					  &number_of_occurances_1,
					  input_word_1,
					  input_stream_1))
      panic ("Read of dictionary block failed in file %s",
	     input_filename_1);
    if (-1 > next_dictionary_index_block (&index_block_size_2,
					  &number_of_occurances_2,
					  input_word_2,
					  input_stream_2))
      panic ("Read of dictionary block failed in file %s",
	     input_filename_2);

    while (true) {
      long            strlen_1 = strlen (input_word_1);
      long            strlen_2 = strlen (input_word_2);
      long            compare = strcmp (input_word_1, input_word_2);	/* +1 if 1 is bigger */

      if (!strlen_1) {		/* Make it work for iso chars */
	if (!strlen_2)
	  compare = 0;
	else
	  compare = -1;
      }
      if ((0 == strlen_1) &&
	  (0 == strlen_2)) {
	/* printf("Done with merge\n"); */
	/* then we are done. */
	break;
      } else if ((0 != strlen_1) && (0 != strlen_2) && (0 == compare)) {
	/* they are equal */
	/* printf("Merging word %s and %s\n", input_word_1, input_word_2); */
	write_dictionary_index_block (number_of_occurances_1 +
				      number_of_occurances_2,
				      input_word_1,
				      output_stream);
	if (generate_dictionary) {
	  add_word_to_dictionary (input_word_1, ftell (output_stream),
				  number_of_occurances_1 +
				  number_of_occurances_2,
				  db);
	}
	db->number_of_words++;
	/* copy file 1 first.  this assumes that file 1 was indexed before
	   file 2 */
	if ((number_of_occurances_1 + number_of_occurances_2) > MAX_OCCURANCES) {
	  /* too many already, just make a dummy (0 length) */
	  if ((number_of_occurances_1 < MAX_OCCURANCES) &&
	      (number_of_occurances_2 < MAX_OCCURANCES)) {
	    /* only print the first time */
#ifdef AUTO_STOPWORDS
	    waislog (WLOG_MEDIUM, WLOG_INDEX,
		     "adding word '%s' into the stoplist %s.stop since it has %ld occurences (limit %ld).",
		     input_word_1, db->database_file, number_of_occurances_1 + number_of_occurances_2, MAX_OCCURANCES);
	    add_auto_stopword (input_word_1);
#else /* not AUTO_STOPWORDS */
	    waislog (WLOG_MEDIUM, WLOG_INDEX,
	      "Deleting word '%s' since it has %ld occurences (limit %ld).",
	      input_word_1, number_of_occurances_1 + number_of_occurances_2,
		     MAX_OCCURANCES);
#endif /* not AUTO_STOPWORDS */
	  }
	  make_dummy_block (input_stream_1, input_stream_2, output_stream);
	} else {
	  merge_blocks (input_word_1, input_stream_1, input_stream_2,
			output_stream);
	}
	if (-1 > next_dictionary_index_block (&index_block_size_1,
					      &number_of_occurances_1,
					      input_word_1,
					      input_stream_1))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_1);

	if (-1 > next_dictionary_index_block (&index_block_size_2,
					      &number_of_occurances_2,
					      input_word_2,
					      input_stream_2))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_2);
      } else if (((0 == strlen_1) && (0 != strlen_2) && (0 > compare)) ||
		 ((0 != strlen_1) && (0 != strlen_2) && (0 < compare))) {
	/* write from block 2 */
	/* printf("From block 2: writing word '%s' not '%s'\n", 
	   input_word_2, input_word_1);  */
	write_dictionary_index_block (number_of_occurances_2, input_word_2,
				      output_stream);
	if (generate_dictionary) {
	  add_word_to_dictionary (input_word_2, ftell (output_stream),
				  number_of_occurances_2, db);
	}
	db->number_of_words++;
	copy_block (input_stream_2, output_stream);
	if (-1 > next_dictionary_index_block (&index_block_size_2,
					      &number_of_occurances_2,
					      input_word_2,
					      input_stream_2))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_2);
      } else {
	/* write from block 1 */
	/* printf("From block 1: writing word '%s' not '%s'\n", 
	   input_word_1, input_word_2); */
	write_dictionary_index_block (number_of_occurances_1, input_word_1,
				      output_stream);
	if (generate_dictionary) {
	  add_word_to_dictionary (input_word_1, ftell (output_stream),
				  number_of_occurances_1, db);
	}
	db->number_of_words++;
	copy_block (input_stream_1, output_stream);
	if (-1 > next_dictionary_index_block (&index_block_size_1,
					      &number_of_occurances_1,
					      input_word_1,
					      input_stream_1))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_1);
      }
    }
  }

  /* write out the number of words */
  s_fseek (output_stream, 0L, SEEK_SET);
  write_bytes (db->number_of_words, INDEX_HEADER_SIZE, output_stream);

  s_fclose (input_stream_1);
  s_fclose (input_stream_2);
  s_fclose (output_stream);
  /* check resulting file */
  /* check_index_file(output_filename); */
  /* scan_index_blocks(output_filename); */

  /* delete the input files */
  remove (input_filename_1);
  remove (input_filename_2);
  /* These next two calls result in the renaming of the new files ontop of 
     the old
     files. This is the only critical time in which if an online update is 
     happening (the index files are being changed while the 
     queries are being answered).  If a query comes along in another
     process and opens the .inv file after the rename is done, but before
     the finished_add_word_to_dictionary has the change to rename the .dct
     file and therefore opens the old version of the .dct file, then the
     files will be out of synch. */
  rename (output_filename, input_filename_1);
  if (generate_dictionary)
    finished_add_word_to_dictionary (db);

#ifdef THINK_C
  /* set the mac file type to INDX */
  setFileType (input_filename_1, WAIS_INDEX_FILE_TYPE, CREATOR);
#endif /* THINK_C */

  return (true);
}

static boolean field_merge_index_file _AP ((long from_version,
					    long into_version,
					    database * db,
					    boolean generate_dictionary,
					    long field_id));

/* Merges a index file (from_version) into another (into_version).
   Returns true if successful */

static boolean 
field_merge_index_file (into_version, from_version,
			db, generate_dictionary, field_id)
     long            into_version;
     long            from_version;
     database       *db;
     boolean         generate_dictionary;
     long            field_id;

/* 
   This is done by merging both into version -1 and then, 
   renames it to into_version, then deletes from_version. */
{
  char            input_filename_1[MAX_FILENAME_LEN];	/* into_version file */
  char            input_filename_2[MAX_FILENAME_LEN];	/* from_version file */
  char            output_filename[MAX_FILENAME_LEN];	/* version -1 */

  FILE           *input_stream_1;
  FILE           *input_stream_2;
  FILE           *output_stream;
  char            input_word_1[MAX_WORD_LENGTH + 1];
  char            input_word_2[MAX_WORD_LENGTH + 1];

  if (generate_dictionary) {
    if (into_version == -2)
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "Merging version %ld into master version and generating dictionary of field %s.",
	       from_version, db->fields[field_id].field_name);
    else
      waislog (WLOG_MEDIUM, WLOG_INDEX,
      "Merging version %ld into %ld and generating dictionary of field %s.",
	       from_version, into_version, db->fields[field_id].field_name);
  } else {
    waislog (WLOG_MEDIUM, WLOG_INDEX, "Merging version %ld and %ld",
	     into_version, from_version);
  }

  field_index_filename_with_version (into_version, input_filename_1,
				     field_id, db);
  field_index_filename_with_version (from_version, input_filename_2,
				     field_id, db);
  field_index_filename_with_version (-1L, output_filename, field_id, db);

  if (NULL == (input_stream_1 = s_fopen (input_filename_1, "rb")))
    return (false);

  if (NULL == (input_stream_2 = s_fopen (input_filename_2, "rb")))
    return (false);

  if (NULL == (output_stream = s_fopen (output_filename, "wb")))
    return (false);

  {
    long            index_block_size_1;
    long            number_of_occurances_1;
    long            number_of_words_1;
    long            index_block_size_2;
    long            number_of_occurances_2;
    long            number_of_words_2;

    /* leave room for the number_of_words in the output */
    s_fseek (output_stream, INDEX_HEADER_SIZE, SEEK_SET);

    /* read the number of words in the 2 input files, this is the maximum 
       number of words that can be in the dictionary,  It is used by 
       init_dict_file to set asside space for the dictionary_header_block.
       It is very likley that some of the words are duplicates, so the actual
       size of the header_block will be smaller.  But we never know until we
       do the merge, so we have to give enough space just in case.
       NOTE - in the case where generate_dictioary is false, we still need
       to do this because we need to skip the header_size in each of the
       input streams 
     */

    number_of_words_1 = read_bytes (INDEX_HEADER_SIZE, input_stream_1);
    number_of_words_2 = read_bytes (INDEX_HEADER_SIZE, input_stream_2);
    if ((number_of_words_1 > 0) && (number_of_words_2 < 1))
      db->fields[field_id].number_of_words = number_of_words_1 + 1;
    else if ((number_of_words_1 < 1) && (number_of_words_2 > 0))
      db->fields[field_id].number_of_words = number_of_words_2 + 1;
    else if ((number_of_words_1 > 0) && (number_of_words_2 > 0))
      db->fields[field_id].number_of_words = number_of_words_1 + number_of_words_2 + 1;
    else
      db->fields[field_id].number_of_words = 0;

    if (db->fields[field_id].number_of_words == 0) {
      if ((number_of_words_1 == 1 && number_of_words_2 < 1) ||
	  (number_of_words_1 < 1 && number_of_words_2 == 1))
	db->fields[field_id].number_of_words = 1;
    }
    if (generate_dictionary) {	/* allocate the header block if necessary */
      field_init_dict_file_for_writing (field_id, db);
    }
    /* now that the dictioanry_header_block is allocated, we can start counting
       for real */
    db->fields[field_id].number_of_words = 0;

    if (-1 > next_dictionary_index_block (&index_block_size_1,
					  &number_of_occurances_1,
					  input_word_1,
					  input_stream_1))
      panic ("Read of dictionary block failed in file %s",
	     input_filename_1);
    if (-1 > next_dictionary_index_block (&index_block_size_2,
					  &number_of_occurances_2,
					  input_word_2,
					  input_stream_2))
      panic ("Read of dictionary block failed in file %s",
	     input_filename_2);
    while (true) {
      long            strlen_1 = strlen (input_word_1);
      long            strlen_2 = strlen (input_word_2);
      long            compare = strcmp (input_word_1, input_word_2);	/* +1 if 1 is bigger */

      if (!strlen_1) {		/* Make it work for iso chars */
	if (!strlen_2)
	  compare = 0;
	else
	  compare = -1;
      }
      if ((0 == strlen_1) &&
	  (0 == strlen_2)) {
	/* printf("Done with merge\n"); */
	/* then we are done. */
	break;
      } else if ((0 != strlen_1) && (0 != strlen_2) && (0 == compare)) {
	/* they are equal */
	/* printf("Merging word %s and %s\n", input_word_1, input_word_2); */
	write_dictionary_index_block (number_of_occurances_1 +
				      number_of_occurances_2,
				      input_word_1,
				      output_stream);
	if (generate_dictionary) {
	  field_add_word_to_dictionary (input_word_1, ftell (output_stream),
					number_of_occurances_1 +
					number_of_occurances_2,
					field_id, db);
	}
	db->fields[field_id].number_of_words++;
	/* copy file 1 first.  this assumes that file 1 was indexed before
	   file 2 */
	if ((number_of_occurances_1 + number_of_occurances_2) > MAX_OCCURANCES) {
	  /* too many already, just make a dummy (0 length) */
	  if ((number_of_occurances_1 < MAX_OCCURANCES) &&
	      (number_of_occurances_2 < MAX_OCCURANCES)) {
	    /* only print the first time */
#ifdef AUTO_STOPWORDS
	    waislog (WLOG_MEDIUM, WLOG_INDEX,
		     "adding word '%s' into the stoplist %s.stop since it has %ld occurences (limit %ld).",
		     input_word_1, db->database_file, number_of_occurances_1 + number_of_occurances_2, MAX_OCCURANCES);
	    add_auto_stopword (input_word_1);
#else /* not AUTO_STOPWORDS */
	    waislog (WLOG_MEDIUM, WLOG_INDEX,
	      "Deleting word '%s' since it has %ld occurences (limit %ld).",
	      input_word_1, number_of_occurances_1 + number_of_occurances_2,
		     MAX_OCCURANCES);
#endif /* not AUTO_STOPWORDS */
	  }
	  make_dummy_block (input_stream_1, input_stream_2, output_stream);
	} else {
	  merge_blocks (input_word_1, input_stream_1, input_stream_2,
			output_stream);
	}
	if (-1 > next_dictionary_index_block (&index_block_size_1,
					      &number_of_occurances_1,
					      input_word_1,
					      input_stream_1))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_1);

	if (-1 > next_dictionary_index_block (&index_block_size_2,
					      &number_of_occurances_2,
					      input_word_2,
					      input_stream_2))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_2);
      } else if (((0 == strlen_1) && (0 != strlen_2) && (0 > compare)) ||
		 ((0 != strlen_1) && (0 != strlen_2) && (0 < compare))) {
	/* write from block 2 */
	/* printf("From block 2: writing word '%s' not '%s'\n", 
	   input_word_2, input_word_1);  */
	write_dictionary_index_block (number_of_occurances_2, input_word_2,
				      output_stream);
	if (generate_dictionary) {
	  field_add_word_to_dictionary (input_word_2, ftell (output_stream),
				      number_of_occurances_2, field_id, db);
	}
	db->fields[field_id].number_of_words++;
	copy_block (input_stream_2, output_stream);
	if (-1 > next_dictionary_index_block (&index_block_size_2,
					      &number_of_occurances_2,
					      input_word_2,
					      input_stream_2))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_2);
      } else {
	/* write from block 1 */
	/* printf("From block 1: writing word '%s' not '%s'\n", 
	   input_word_1, input_word_2); */
	write_dictionary_index_block (number_of_occurances_1, input_word_1,
				      output_stream);
	if (generate_dictionary) {
	  field_add_word_to_dictionary (input_word_1, ftell (output_stream),
				      number_of_occurances_1, field_id, db);
	}
	db->fields[field_id].number_of_words++;
	copy_block (input_stream_1, output_stream);
	if (-1 > next_dictionary_index_block (&index_block_size_1,
					      &number_of_occurances_1,
					      input_word_1,
					      input_stream_1))
	  panic ("Read of dictionary block failed in file %s",
		 input_filename_1);
      }
    }
  }

  /* write out the number of words */
  s_fseek (output_stream, 0L, SEEK_SET);
  write_bytes (db->fields[field_id].number_of_words,
	       INDEX_HEADER_SIZE, output_stream);

  s_fclose (input_stream_1);
  s_fclose (input_stream_2);
  s_fclose (output_stream);
  /* check resulting file */
  /* check_index_file(output_filename); */
  /* scan_index_blocks(output_filename); */

  /* delete the input files */
  remove (input_filename_1);
  remove (input_filename_2);
  /* These next two calls result in the renaming of the new files ontop of 
     the old
     files. This is the only critical time in which if an online update is 
     happening (the index files are being changed while the 
     queries are being answered).  If a query comes along in another
     process and opens the .inv file after the rename is done, but before
     the finished_add_word_to_dictionary has the change to rename the .dct
     file and therefore opens the old version of the .dct file, then the
     files will be out of synch. */
  rename (output_filename, input_filename_1);
  if (generate_dictionary)
    field_finished_add_word_to_dictionary (field_id, db);

#ifdef THINK_C
  /* set the mac file type to INDX */
  setFileType (input_filename_1, WAIS_INDEX_FILE_TYPE, CREATOR);
#endif /* THINK_C */

  return (true);
}

/* only works on positive, non-zero numbers, and is slow to boot, yippie */
static long logcount _AP ((long number));
static long 
logcount (number)
     long            number;
{
  long            answer = 0;

  for (; number > 0; number = number >> 1)
    answer++;
  return (answer);
}

/*
 * Originally, the index files produced while building an index were
 * merged together using a normal binary merge sort.  For example, if
 * there were 8 files (0 through 7) to be merged, they were merged
 * in the following order, after all the indexing was finished:
 *
 * 0+1 -> 0  2+3 -> 2  4+5 -> 4  6+7 -> 6
 * 0+2 -> 0  4+6 -> 4
 * 0+4 -> 0
 *
 * The only problem with this is that it means that all of the index
 * files have to stay around until the entire database has been
 * indexed.  This means that the amount of disk space needs
 * temporarily during the indexing grows linearly with the amount of
 * data being indexed.
 *
 * The idea of this routine is to reduce the amount of temporary disk
 * space used by doing whatever merging it can, DURING the indexing
 * rather than after it.  The routine gets called whenever an index
 * file has just been flushed, and it figures out what merging needs
 * to be performed and does it.
 *
 * The merging done by this routine ends up being the same as the
 * merging done by the old routine at the end of indexing.  However,
 * it happens in a different order.  For example, once again, if the
 * total number of index files were 8, this is what would happen (a
 * number by itself means that it was flushed; number+number->number
 * refers to a merge):
 * 
 * 0 1 0+1->0
 * 2 3 2+3->2 0+2->0
 * 4 5 4+5->4
 * 6 7 6+7->6 4+6->4 0+4->0
 *
 * You should be able to convince yourself relatively quickly that the
 * result of this is that the amount of space needed temporarily
 * during indexing grows logarithmically with the amount of data being
 * indexed, rather than linearly.  What's more, no extra time is required.
 *
 * There is one complication.  When the total number of files being
 * indexed is an even power of two, the pattern of merging is regular,
 * as demonstrated above.  However, if the total number of files is
 * not an even power of two, things are a bit complicated.  For
 * example, if there were 11 files instead of 8, we'd have to add this
 * to the end of the list given above:
 *
 * 8 9 8+9->8
 * 10 8+10->8 0+8->0
 * 
 * The algorithm for determining what merges to perform after every
 * second index file, when the index file just flushed is not the last
 * one, is simple.  However, the algorithm for determining what to do
 * on the last file flushed, whether or not it's a power of two, is a
 * bit more complicated.
 *
 * The first algorithm:
 *
 * splitter := the number of the file just indexed (starting from 1)
 * if splitter is odd
 *   then return
 * low = splitter - 2  // counting index files from 0
 * high = splitter - 1 // ditto
 * twotothe = 1
 * while splitter is even do
 *   merge files low and high into low
 *   high := high - twotothe
 *   twotothe := twotothe * 2
 *   low := low - twotothe
 *   splitter := splitter / 2
 * done
 *
 * The translation from this into the code below should be somewhat
 * obvious.  What is NOT obvious, however, is all the stuff with
 * "target", "realhigh" and "where_to_stop", and with the extra loop
 * wrapped around the code above.  The point of all that is to handle
 * the second case mentioned above, when we're done indexing and need
 * to merge everything together.  It works by pretending doing a
 * series of merges of the first type described above, from the actual
 * last index file until the next power of two higher than it.  At
 * each pretended series, only those merges for which the low and high
 * index files actually still exist are performed.
 *
 * In the degenerate case of the second algorithm, when the last index
 * file being flushed is an even power of two, the outer loop only
 * runs once and the behavior is the same as in the first algorithm
 * described above.
 *
 * I hope that explains things adequately :-).
 */

#ifndef END_MERGE

static void do_intermediate_merging _AP ((database * db, boolean completely));

static void 
do_intermediate_merging (db, completely)
     database       *db;
     boolean         completely;
{
  int             last = db->index_file_number - 1;
  int             twotothe, low, high, where_to_stop, target, realhigh = last,
                  splitter;
  char            filename[MAX_FILENAME_LEN];
  char            filename2[MAX_FILENAME_LEN];

  if (last % 2 == 0) {
    if (!completely) {
      return;
    } else {
      for (where_to_stop = 1; where_to_stop < last + 1;
	   where_to_stop *= 2)	/* empty */
	;
    }
  } else {
    if (completely) {
      for (where_to_stop = 1; where_to_stop < last + 1;
	   where_to_stop *= 2)	/* empty */
	;
    } else {
      where_to_stop = last + 1;
    }
  }

  for (target = ((last % 2) == 0) ? last + 2 : last + 1;
       target <= where_to_stop; target += 2) {

    low = target - 2;
    high = target - 1;
    twotothe = 1;
    splitter = target;

    while (((splitter % 2) == 0) && (low >= 0)) {
      if (low < realhigh) {
	if (!merge_index_file (low, (high > realhigh) ? realhigh : high, db,
			       completely && (!low) &&
			     !probe_file (index_filename (filename, db)))) {
	  panic ("Error in merging file %ld into %ld", low,
		 (high > last) ? last : high);
	}
	realhigh = low;
      }
      high -= twotothe;
      twotothe *= 2;
      low -= twotothe;
      splitter /= 2;
    }
  }

  if (completely) {
    if (probe_file (index_filename (filename, db))) {
      merge_index_file (-2L, 0L, db, true);
    } else if (last == 0) {
      touch_file (index_filename_with_version (1, filename, db));
      merge_index_file (0L, 1, db, true);
      /* rename 0 into the final name */
      rename (index_filename_with_version (0L, filename2, db),
	      index_filename (filename, db));
    }
  }
}
#endif /* not END_MERGE */

#ifndef END_MERGE
static void field_do_intermediate_merging _AP ((database * db, boolean completely, long field_id));

static void 
field_do_intermediate_merging (db, completely, field_id)
     database       *db;
     boolean         completely;
     long            field_id;
{
  long            i = field_id;
  int             last = db->fields[i].index_file_number - 1;
  int             twotothe, low, high, where_to_stop, target, realhigh = last,
                  splitter;
  char            filename[MAX_FILENAME_LEN];
  char            filename2[MAX_FILENAME_LEN];

  twotothe = low = high = where_to_stop = target = splitter = 0;

  if (last % 2 == 0) {
    if (!completely) {
      return;
    } else {
      for (where_to_stop = 1; where_to_stop < last + 1;
	   where_to_stop *= 2)	/* empty */
	;
    }
  } else {
    if (completely) {
      for (where_to_stop = 1; where_to_stop < last + 1;
	   where_to_stop *= 2)	/* empty */
	;
    } else {
      where_to_stop = last + 1;
    }
  }

  for (target = ((last % 2) == 0) ? last + 2 : last + 1;
       target <= where_to_stop; target += 2) {

    low = target - 2;
    high = target - 1;
    twotothe = 1;
    splitter = target;

    while (((splitter % 2) == 0) && (low >= 0)) {
      if (low < realhigh) {
	if (!field_merge_index_file (low, (high > realhigh) ? realhigh : high,
				     db,
				     completely && (!low) &&
				     !probe_file (field_index_filename (filename, db->fields[i].field_name,
								 db)), i)) {
	  panic ("Error in merging file %ld into %ld", low,
		 (high > last) ? last : high);
	}
	realhigh = low;
      }
      high -= twotothe;
      twotothe *= 2;
      low -= twotothe;
      splitter /= 2;
    }
  }

  if (completely) {
    if (probe_file (field_index_filename (filename, db->fields[i].field_name,
					  db))) {
      field_merge_index_file (-2L, 0L, db, true, i);
    } else if (last == 0) {
      touch_file (field_index_filename_with_version (1, filename, i, db));
      field_merge_index_file (0L, 1, db, true, i);
      /* rename 0 into the final name */
      rename (field_index_filename_with_version (0L, filename2, i, db),
	      field_index_filename (filename, db->fields[i].field_name, db));
    }
  }
}
#endif /* not END_MERGE */

static void merge_index_files _AP ((database * db));

static void 
merge_index_files (db)
     database       *db;

/* this merges all the temporary inverted files into a large on 
 * and creates a dictionary.
 * This is done in a logrithmic merge of the files.
 * An n-ary merge would be faster of course, but what the heck... 
 */
{
  /* this version does it two at a time */
  char            filename[MAX_FILENAME_LEN];
  char            filename2[MAX_FILENAME_LEN];
  long            level;
  long            final_level;
  long            number_of_files_to_merge = db->index_file_number;

  /* at level 0, merge 0,1->0; 2,3->2; 4,5->4; 6,7->6; 
     at level 1, merge 0,2->0; 4,6->4;
     at level 2, merge 0,4->0;
     stop.
     If there is only 1 file, then dont merge (final_level will be -1),
     the dictionary will be generated by merge after the for loop.
   */
  final_level = logcount (number_of_files_to_merge - 1) - 1;
  for (level = 0; level <= final_level; level++) {
    long            into_version;

    for (into_version = 0;
	 into_version < number_of_files_to_merge;
	 into_version += (2 << level)) {
      long            from_version = into_version + (1 << level);

      if (from_version < number_of_files_to_merge) {
	boolean         generate_dictionary;	/* if this is the final level and 

						   there is no .inv file then yes */
	if (level == final_level) {
	  if (probe_file (index_filename (filename, db)))
	    generate_dictionary = false;
	  else
	    generate_dictionary = true;
	} else {
	  generate_dictionary = false;
	}

	/* printf("Level %d (out of %d) merging into %d from %d\n", 
	   level, final_level, into_version, from_version); */
	if (false == merge_index_file (into_version, from_version, db,
				       generate_dictionary))
	  panic ("Error in merging file %ld into %ld",
		 from_version, into_version);
      }
    }
  }				/* done merging */
  if (probe_file (index_filename (filename, db))) {
    /* if there is a .inv file, then we are adding to an existing db. merge */
    /* do this by merging straight into the final using the special -2 version
       for the .inv file */
    merge_index_file (-2L, 0L, db, true);
  } else if (number_of_files_to_merge == 1) {
    /* then we have to generate a dictionary for this one file. 
       create a dummy file in 1, then merge that while making a dictionary
     */
    touch_file (index_filename_with_version (1, filename, db));
    merge_index_file (0L, 1, db, true);
    /* rename 0 into the final name */
    rename (index_filename_with_version (0L, filename2, db),
	    index_filename (filename, db));
  }
}

static void field_merge_index_files _AP ((database * db, long field_id));

static void 
field_merge_index_files (db, field_id)
     database       *db;
     long            field_id;

/* this merges all the temporary inverted files into a large on 
 * and creates a dictionary.
 * This is done in a logrithmic merge of the files.
 * An n-ary merge would be faster of course, but what the heck... 
 */
{
  /* this version does it two at a time */
  char            filename[MAX_FILENAME_LEN];
  char            filename2[MAX_FILENAME_LEN];
  long            level;
  long            final_level;
  long            number_of_files_to_merge;
  long            i = field_id;

  /* at level 0, merge 0,1->0; 2,3->2; 4,5->4; 6,7->6; 
     at level 1, merge 0,2->0; 4,6->4;
     at level 2, merge 0,4->0;
     stop.
     If there is only 1 file, then dont merge (final_level will be -1),
     the dictionary will be generated by merge after the for loop.
   */

  number_of_files_to_merge = db->fields[i].index_file_number;
  final_level = logcount (number_of_files_to_merge - 1) - 1;
  for (level = 0; level <= final_level; level++) {
    long            into_version;

    for (into_version = 0;
	 into_version < number_of_files_to_merge;
	 into_version += (2 << level)) {
      long            from_version = into_version + (1 << level);

      if (from_version < number_of_files_to_merge) {
	boolean         generate_dictionary;	/* if this is the final level and 

						   there is no .inv file then yes */
	if (level == final_level) {
	  if (probe_file (index_filename (filename, db)))
	    generate_dictionary = false;
	  else
	    generate_dictionary = true;
	} else {
	  generate_dictionary = false;
	}

	/* printf("Level %d (out of %d) merging into %d from %d\n", 
	   level, final_level, into_version, from_version); */
	if (false == field_merge_index_file (into_version, from_version, db,
					     generate_dictionary, i))
	  panic ("Error in merging file %ld into %ld",
		 from_version, into_version);
      }
    }
  }				/* done merging */
  if (probe_file (field_index_filename (filename, db->fields[i].field_name, db))) {
    /* if there is a .inv file, then we are adding to an existing db. merge */
    /* do this by merging straight into the final using the special -2 version
       for the .inv file */
    field_merge_index_file (-2L, 0L, db, true, i);
  } else if (number_of_files_to_merge == 1) {
    /* then we have to generate a dictionary for this one file. 
       create a dummy file in 1, then merge that while making a dictionary
     */
    touch_file (field_index_filename_with_version (1, filename, i, db));
    field_merge_index_file (0L, 1, db, true, i);
    /* rename 0 into the final name */
    rename (field_index_filename_with_version (0L, filename2, i, db),
	    field_index_filename (filename, db->fields[i].field_name, db));
  }
}

/* Tung, 2/95
 * memory place is full, write out all terms of the document
 * which is not ready indexed to take place for new entries 
 */

#define SAVEDSUFFIX ".savedtmp"

long 
save_terms_for_doc (db)
     database       *db;
{
  extern long     bucket_ids_doc_array[];   /* defined in weight.c */
  extern long     number_of_bucket_ids;	    /* defined in irtfiles.c */
  long            i, id, char_pos, nwrite;
  FILE           *stream = NULL;
  char            filename[1000];
  hashtable      *htable = db->the_word_memory_hashtable;

  strcpy (filename, db->database_file);
  strcat (filename, SAVEDSUFFIX);

  stream = s_fopen (filename, "w");
  if (stream == NULL)
    panic ("can't read file %s", filename);

  for (i = 0; i < number_of_bucket_ids; i++) {
    id = bucket_ids_doc_array[i];
    fprintf (stream, "%s\n", (htable->contents)[id].key);
    write_bytes ((htable->contents)[id].occurances_in_doc,
		 NUMBER_OF_OCCURANCES_SIZE, stream);

    if ((htable->contents)[id].occurances_in_doc > 0)
      (htable->contents)[id].docs_incl_term -= 1;

#ifdef LITERAL
    nwrite =
      (htable->contents)[id].current_memory_ptr -
      ((htable->contents)[id].memory_ptr +
       ((htable->contents)[id].index_element_current_pos +
	INDEX_ELEMENT_SIZE - CHARACTER_POSITION_SIZE));
    (htable->contents)[id].current_memory_ptr -= nwrite;
    write_bytes (nwrite, NUMBER_OF_OCCURANCES_SIZE, stream);
#else /* not LITERAL */
    (htable->contents)[id].current_memory_ptr -= CHARACTER_POSITION_SIZE;
    nwrite = CHARACTER_POSITION_SIZE;
#endif /* not LITERAL */
    if (nwrite != fwrite ((htable->contents)[id].current_memory_ptr, 1L, nwrite, stream))
      panic ("Write failed");

    (htable->contents)[id].current_memory_ptr -=
      INDEX_ELEMENT_SIZE - CHARACTER_POSITION_SIZE;

    fflush (stream);
  }

  if (stream != NULL)
    s_fclose (stream);

  return (i);
}

void
add_terms_saved (is_field, doc_id, db, number_of_saved_terms)
     boolean         is_field;
     long            doc_id;
     database       *db;
     long            number_of_saved_terms;
{
  extern long     number_of_bucket_ids;	    /* defined in irtfiles.c */
  extern long     max_term_frequency;	    /* defined in weight.c */
  long            i, tf;
  FILE           *stream = NULL;
  char            filename[1000];
  char            word[100];
  long            char_pos;

#ifdef LITERAL
  long            char_list_size;

#endif /* LITERAL */
  hash_entry     *wrd_entry;

  number_of_bucket_ids = 0;
  max_term_frequency = 0;

  strcpy (filename, db->database_file);
  strcat (filename, SAVEDSUFFIX);

  stream = s_fopen (filename, "r");
  if (stream == NULL)
    panic ("can't read file %s", filename);

  for (i = 0; i < number_of_saved_terms; i++) {
    fgets (word, 100, stream);
    if (word[strlen (word) - 1] == '\n')
      word[strlen (word) - 1] = '\0';

    tf = read_bytes (NUMBER_OF_OCCURANCES_SIZE, stream);
#ifdef LITERAL
    char_list_size = read_bytes (NUMBER_OF_OCCURANCES_SIZE, stream);
#endif /* LITERAL */

    wrd_entry = add_word_saved_into_hashtable (word,
					       doc_id,
					       tf,
#ifdef LITERAL
					       char_list_size,
#endif /* LITERAL */
					     db->the_word_memory_hashtable);

    db->number_of_words_in_hashtable += tf;

    if (wrd_entry != NULL) {
#ifdef LITERAL
      if (0 > fread_from_stream (stream, wrd_entry->current_memory_ptr, char_list_size)) {
	panic ("read failed");
      }
      wrd_entry->current_memory_ptr += char_list_size;
#else /* not LITERAL */
      wrd_entry->current_memory_ptr +=
	write_bytes_to_memory (char_pos,
			       CHARACTER_POSITION_SIZE,
			       wrd_entry->current_memory_ptr -
#ifdef NEW_WEIGHT
			       NEW_WEIGHT_SIZE
#else /* not NEW_WEIGHT */
			       WEIGHT_SIZE
#endif /* not NEW_WEIGHT */
			       + CHARACTER_POSITION_SIZE)
#endif /* not LITERAL */
    }
  }
  if (stream != NULL) {
    s_fclose (stream);
    remove (filename);
  }
}

/* ============================================ */
/* ===  Flushing the memory version of the  === */
/* ===  word hashtable to disk files        === */
/* ============================================ */

static void flush_word_entry_to_file _AP ((hash_entry * wrd_entry,
					   long not_flush_doc_id,
					   database * db));

static void 
flush_word_entry_to_file (wrd_entry, not_flush_doc_id, db)
     hash_entry     *wrd_entry;
     long            not_flush_doc_id;
     database       *db;

{
  /* In this version, each word_entry is made into an index block.
     This means that there may be many small index blocks if the memory
     can not hold many files worth of data.  This approach was taken 
     for simplicities sake and it might be the best approach if there
     is a small vocabulary.

     This assumes that the index stream is positioned at the end.
   */

  if (wrd_entry->number_of_occurances > MAX_OCCURANCES) {
    /* there are too many of this word, do nothing.
     * this may result in some having been written before, or 
     * no occurances of this word might be recorded.  Is this right?
     * if the first MAX_OCCURANCES want to be recored, then
     * add this clause to this condition:
     *     && wrd_entry->starting_block_number != 0
     */
#ifdef AUTO_STOPWORDS
    if (wrd_entry->stop_flag)
      return;
    else {
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "adding word '%s' into the stoplist %s.stop since it has %ld occurences (limit %ld).",
	       wrd_entry->key, db->database_file, wrd_entry->number_of_occurances, MAX_OCCURANCES);
      add_auto_stopword (wrd_entry->key);
    }
#endif /* AUTO_STOPWORDS */
    return;
  }
  if ((unsigned long) (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr +
		       INDEX_BLOCK_HEADER_SIZE) >= MAX_INDEX_BLOCK_SIZE) {
#ifdef AUTO_STOPWORDS
    if (wrd_entry->stop_flag)
      return;
    else {
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "adding word '%s' into the stoplist %s.stop since there are too many entries",
	       wrd_entry->key, db->database_file);
      add_auto_stopword (wrd_entry->key);
    }
#endif /* AUTO_STOPWORDS */
    return;			/* there are too many entries to store away, therefore throw it all away */
  }
  if (NULL == wrd_entry->memory_ptr) {
    /* not entries to write out */
    return;
  }
  if (0 == (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr)) {
    return;			/* there are no word entries to write */
  }
  if (not_flush_doc_id == wrd_entry->current_doc_id) {
    wrd_entry->number_of_occurances -= wrd_entry->occurances_in_doc;
  }
  if (((not_flush_doc_id == wrd_entry->current_doc_id) &&
       (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr > 0) &&
       wrd_entry->number_of_occurances > 0) ||
      (not_flush_doc_id != wrd_entry->current_doc_id)) {
    /* printf("Flushing word %s\n", wrd_entry->key); */
    /* if this is the entry for this word, the put in the dictionary
       entry in the index file */
    write_dictionary_index_block (wrd_entry->number_of_occurances,
				  wrd_entry->key,
				  db->index_stream);
    db->number_of_words++;

    /* allocate and write the new block */
    allocate_index_block (wrd_entry->current_memory_ptr -
			  wrd_entry->memory_ptr +
			  INDEX_BLOCK_HEADER_SIZE,
#ifdef LITERAL
			  wrd_entry->docs_incl_term,
#endif /* LITERAL */
			  db->index_stream);
    /* cprintf(PRINT_AS_INDEXING, "New block number: %ld\n", new_block); */

    if ((wrd_entry->current_memory_ptr - wrd_entry->memory_ptr) !=
	fwrite (wrd_entry->memory_ptr, 1L, (wrd_entry->current_memory_ptr -
					    wrd_entry->memory_ptr),
		db->index_stream))
      panic ("Write failed");
  }
  /* free the memory for the block written out */
  free_word_occurance_block (wrd_entry->memory_ptr);
  wrd_entry->memory_ptr = NULL;
  wrd_entry->current_memory_ptr = NULL;
  wrd_entry->memory_size = 0;
  wrd_entry->current_doc_id = 0;
}

static void field_flush_word_entry_to_file _AP ((hash_entry * wrd_entry,
						 long field_id,
						 long not_flush_doc_id,
						 database * db));

static void 
field_flush_word_entry_to_file (wrd_entry, field_id, not_flush_doc_id, db)
     hash_entry     *wrd_entry;
     long            field_id;
     long            not_flush_doc_id;
     database       *db;

{
  /* In this version, each word_entry is made into an index block.
     This means that there may be many small index blocks if the memory
     can not hold many files worth of data.  This approach was taken 
     for simplicities sake and it might be the best approach if there
     is a small vocabulary.

     This assumes that the index stream is positioned at the end.
   */

  if (db->fields[field_id].numeric) {	/* insert only number */
    if (!isdigit (wrd_entry->key[0]) &&
	!(wrd_entry->key[0] == '-') &&
	!(wrd_entry->key[0] == '.'))
      return;
  }
  if (wrd_entry->number_of_occurances > MAX_OCCURANCES) {
    /* there are too many of this word, do nothing.
     * this may result in some having been written before, or 
     * no occurances of this word might be recorded.  Is this right?
     * if the first MAX_OCCURANCES want to be recored, then
     * add this clause to this condition:
     *     && wrd_entry->starting_block_number != 0
     */
#ifdef AUTO_STOPWORDS
    if (wrd_entry->stop_flag)
      return;
    else {
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "adding word '%s' into the stoplist %s.stop since it has %ld occurences (limit %ld).",
	       wrd_entry->key, db->database_file, wrd_entry->number_of_occurances, MAX_OCCURANCES);
      add_auto_stopword (wrd_entry->key);
    }
#endif /* AUTO_STOPWORDS */
    return;
  }
  if ((unsigned long) (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr +
		       INDEX_BLOCK_HEADER_SIZE) >= MAX_INDEX_BLOCK_SIZE) {
#ifdef AUTO_STOPWORDS
    if (wrd_entry->stop_flag)
      return;
    else {
      waislog (WLOG_MEDIUM, WLOG_INDEX,
	       "adding word '%s' into the stoplist %s.stop since there are too many entries",
	       wrd_entry->key, db->database_file);
      add_auto_stopword (wrd_entry->key);
    }
#endif /* AUTO_STOPWORDS */
    return;			/* there are too many entries to store away, therefore throw it all away */
  }
  if (NULL == wrd_entry->memory_ptr) {
    /* not entries to write out */
    return;
  }
  if (0 == (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr)) {
    return;			/* there are no word entries to write */
  }
  if (not_flush_doc_id == wrd_entry->current_doc_id) {
    wrd_entry->number_of_occurances -= wrd_entry->occurances_in_doc;
  }
  if (((not_flush_doc_id == wrd_entry->current_doc_id) &&
       (wrd_entry->current_memory_ptr - wrd_entry->memory_ptr > 0) &&
       wrd_entry->number_of_occurances > 0) ||
      (not_flush_doc_id != wrd_entry->current_doc_id)) {

    /* printf("Flushing word %s\n", wrd_entry->key); */
    /* if this is the entry for this word, the put in the dictionary
       entry in the index file */
    write_dictionary_index_block (wrd_entry->number_of_occurances,
				  wrd_entry->key,
				  *(db->field_index_streams));
    db->fields[field_id].number_of_words++;

    /* allocate and write the new block */
    allocate_index_block (wrd_entry->current_memory_ptr -
			  wrd_entry->memory_ptr +
			  INDEX_BLOCK_HEADER_SIZE,
#ifdef LITERAL
			  wrd_entry->docs_incl_term,
#endif /* LITERAL */
			  *(db->field_index_streams));

    /* cprintf(PRINT_AS_INDEXING, "New block number: %ld\n", new_block); */
    if ((wrd_entry->current_memory_ptr - wrd_entry->memory_ptr) !=
	fwrite (wrd_entry->memory_ptr, 1L, (wrd_entry->current_memory_ptr -
					    wrd_entry->memory_ptr),
		*(db->field_index_streams)))
      panic ("Write failed");
  }
#if 0
  if (not_flush_doc_id == wrd_entry->current_doc_id)
    wrd_entry->current_memory_ptr += INDEX_ELEMENT_SIZE - CHARACTER_POSITION_SIZE + (wrd_entry->occurances_in_doc * CHARACTER_POSITION_SIZE);
#endif /* 0 */

  /* free the memory for the block written out */
  free_word_occurance_block (wrd_entry->memory_ptr);
  wrd_entry->memory_ptr = NULL;
  wrd_entry->current_memory_ptr = NULL;
  wrd_entry->memory_size = 0;
  wrd_entry->current_doc_id = 0;
}

/* This flushes all of the memory version of the word hashtable to disk.
 * this is called when the hashtable is filling up or we have 
 * accumulated enough words.  If completely is true, then it will merge 
 * all the intermediate files.
 */
void 
flush_memory_hashtable_to_disk (db, completely, not_flush_doc_id)
     database       *db;
     boolean         completely;
     long            not_flush_doc_id;

{
  /* map over the memory word hashtable and write the entries to disk */
  long            i;
  char            filename[1000];
  long            number_of_saved_terms;

  /* analyze the hash distribution 
     analyze_hashtable_distribution(db->the_word_memory_hashtable);
   */

  if (db->index_stream != NULL)
    s_fclose (db->index_stream);

  db->index_stream =
    s_fopen (index_filename_with_version (db->index_file_number++, filename, db),
	     "wb");
  if (NULL == db->index_stream)
    panic ("Could not open file %s to insert index",
	 index_filename_with_version (db->index_file_number, filename, db));

  waislog (WLOG_MEDIUM, WLOG_INDEX,
	   "Flushing %ld different words, %ld total words to disk...",
	   hashtable_count (db->the_word_memory_hashtable),
	   db->number_of_words_in_hashtable);

  db->number_of_words = 0;
  write_bytes (0l, INDEX_HEADER_SIZE, db->index_stream);

  if (not_flush_doc_id != -1) {
    number_of_saved_terms = save_terms_for_doc (db);
  }
  sort_hashtable (db->the_word_memory_hashtable);
  for (i = 0; i < hashtable_count (db->the_word_memory_hashtable); i++) {
    flush_word_entry_to_file (&db->the_word_memory_hashtable->contents[i], not_flush_doc_id, db);
  }

  /* write out the number of entries into the index_file header */
  s_fseek (db->index_stream, 0L, SEEK_SET);
  write_bytes (db->number_of_words, INDEX_HEADER_SIZE, db->index_stream);
  s_fclose (db->index_stream);

  /* since everything is written out, we can flush these. */
  flush_word_occurance_buffers ();
  clr_hashtable (db->the_word_memory_hashtable);
  db->number_of_words_in_hashtable = 0;

  /* add the stopwords to the index for the next session. */
#ifdef AUTO_STOPWORDS
  add_stop_words (db->the_word_memory_hashtable, db->database_file);
#else /* not AUTO_STOPWORDS */
  add_stop_words (db->the_word_memory_hashtable);
#endif /* not AUTO_STOPWORDS */

  if (not_flush_doc_id != -1) {
    add_terms_saved (false, not_flush_doc_id, db, number_of_saved_terms);
  }
  waislog (WLOG_MEDIUM, WLOG_INDEX,
	   "Done flushing version %ld", db->index_file_number - 1);
  /* scan_index_blocks(filename); */

#ifdef END_MERGE
  if (completely) {
    waislog (WLOG_MEDIUM, WLOG_INDEX,
	     "Merging %ld files",
	     db->index_file_number);
    merge_index_files (db);
    waislog (WLOG_MEDIUM, WLOG_INDEX,
	     "Done merging.");
  }
#else /* not END_MERGE */

  do_intermediate_merging (db, completely);

#endif /* not END_MERGE */
}

void 
field_flush_memory_hashtable_to_disk (db, completely, field_id, not_flush_doc_id)
     database       *db;
     boolean         completely;
     long            field_id;
     long            not_flush_doc_id;

{
  /* map over the memory word hashtable and write the entries to disk */
  long            i = field_id;
  long            j;
  char            filename[1000];
  long            number_of_saved_terms;

  if (*(db->field_index_streams) != NULL)
    s_fclose (*(db->field_index_streams));

  *(db->field_index_streams) =
    s_fopen (field_index_filename_with_version (db->fields[i].index_file_number++, filename, i, db), "wb");
  if (NULL == *(db->field_index_streams))
    panic ("Could not open file %s to insert index",
	 field_index_filename_with_version (db->fields[i].index_file_number,
					    filename, i, db));

  waislog (WLOG_MEDIUM, WLOG_INDEX,
     "Flushing %ld different words, %ld total words of field %s to disk...",
	   hashtable_count (db->the_word_memory_hashtable),
	   db->fields[i].number_of_words_in_hashtable,
	   db->fields[i].field_name);
  db->fields[i].number_of_words = 0;
  write_bytes (0l, INDEX_HEADER_SIZE, *(db->field_index_streams));

  if (not_flush_doc_id != -1) {
    number_of_saved_terms = save_terms_for_doc (db);
  }
  if (db->fields[i].numeric)
    sort_hashtable_numeric (db->the_word_memory_hashtable);
  else
    sort_hashtable (db->the_word_memory_hashtable);
  for (j = 0; j < hashtable_count (db->the_word_memory_hashtable); j++) {
    field_flush_word_entry_to_file (&db->the_word_memory_hashtable->contents[j], i, not_flush_doc_id, db);
  }
  s_fseek (*(db->field_index_streams), 0L, SEEK_SET);
  write_bytes (db->fields[i].number_of_words, INDEX_HEADER_SIZE,
	       *(db->field_index_streams));
  s_fclose (*(db->field_index_streams));

  /* since everything is written out, we can flush these. */

  flush_word_occurance_buffers ();
  clr_hashtable (db->the_word_memory_hashtable);
  db->fields[i].number_of_words_in_hashtable = 0;
  /* add the stopwords to the index for the next session. */
#ifdef AUTO_STOPWORDS
  add_stop_words (db->the_word_memory_hashtable, db->database_file);
#else /* not AUTO_STOPWORDS */
  add_stop_words (db->the_word_memory_hashtable);
#endif /* not AUTO_STOPWORDS */

  if (not_flush_doc_id != -1) {
    add_terms_saved (false, not_flush_doc_id, db, number_of_saved_terms);
  }
  waislog (WLOG_MEDIUM, WLOG_INDEX,
	   "Done flushing version %ld", db->fields[i].index_file_number - 1);
  /* scan_index_blocks(filename); */

#ifdef END_MERGE
  if (completely) {
    waislog (WLOG_MEDIUM, WLOG_INDEX,
	     "Merging %ld files",
	     db->index_file_number);
    field_merge_index_files (db, field_id);
    waislog (WLOG_MEDIUM, WLOG_INDEX,
	     "Done merging.");
  }
#else /* not END_MERGE */

  field_do_intermediate_merging (db, completely, field_id);

#endif /* not END_MERGE */
}

long 
init_add_word (db, hashtable_size, flush_after_n_words)
     database       *db;
     long            hashtable_size;
     long            flush_after_n_words;

{
  char            fn[256];

  if (NULL != db->the_word_memory_hashtable)
    free_hashtable (db->the_word_memory_hashtable);
  db->the_word_memory_hashtable =
    make_hashtable (0, hashtable_size, sizeof (hash_entry));
  db->flush_after_n_words = flush_after_n_words;
#ifdef AUTO_STOPWORDS
  add_stop_words (db->the_word_memory_hashtable, db->database_file);
#else /* not AUTO_STOPWORDS */
  add_stop_words (db->the_word_memory_hashtable);
#endif /* not AUTO_STOPWORDS */
  strcpy (fn, db->database_file);
  strcat (fn, synonym_ext);
  syn_ReadFile (fn, &db->syn_Table, &db->syn_Table_Size);

  return (0);
}

long 
field_finished_add_word (db, field_id)
     database       *db;
     long            field_id;

{
  field_flush_memory_hashtable_to_disk (db, true, field_id, -1);
  return (0);			/* successful */
}

long 
finished_add_word (db)
     database       *db;

{
  flush_memory_hashtable_to_disk (db, true, -1);
  /* syn_Free( db->syn_Table,&db->syn_Table_Size ); */
  /* very bad idea for fields !!! */
  return (0);			/* successful */
}
