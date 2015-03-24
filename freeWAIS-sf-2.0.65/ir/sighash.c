/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* The memory hashtables for building an index. */
/* -brewster 5/90 */

/* main functions:
 *   add_word
 *   finished_add_word
 *   look_up_word
 *
 * The idea is to store up a bunch of words before going to disk.
 * A word entry points to where it will go on disk, and
 * accumulates the entries before doing it.
 *
 * Some of the policy issues in this file are:
 *   How much weight should the first occurance of a word in a document get
 *   over the other occurances.  The first occurance should be worth more
 *   so that words with 3 occurances of "dog" and not "cat"'s should not 
 *   win out over 1 "dog" and 1 "cat" if the question is "Tell me about cats
 *   torture dogs"
 *   The extra weight is 5 at this point.
 *
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/sighash.c,v 2.0.1.4 1995/11/14 18:21:28 pfeifer Exp $";
#endif

/* Change log:
 * $Log: sighash.c,v $
 * Revision 2.0.1.4  1995/11/14 18:21:28  pfeifer
 * patch51:
 *
 * Revision 2.0.1.3  1995/11/14  16:15:26  pfeifer
 * patch50:
 *
 * Revision 2.0.1.2  1995/09/15  09:47:25  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/08  19:20:35  pfeifer
 * patch1: #ifdef-ed definition of add_stop_words with AUTO_STOPWORDS.
 * patch1: irhash.c contains another one!?  #ifdef-ed also a call to it.
 *
 * Revision 2.0  1995/09/08  08:01:18  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.5  1995/05/07  16:04:57  pfeifer
 * More fixes by Tom Snee. Literal serach now works again!
 *
 * Revision 1.4  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.3  1994/05/20  12:57:11  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:45  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.23  92/05/05  13:21:25  shen
 * undo the change make in previous revision 1.22
 * fixed the bug in db->total_word_ccount by checking the last word in t
 * table. If the last word is DICTIONARY_TOTAL_SIZE_WORD, decrement the number
 * of words to add to dictionary file by 1.
 * 
 * Revision 1.22  92/05/05  11:01:54  shen
 * fixed db->total_word_count for incremental update by eliminating adding
 * DICTIONARY_TOTAL_SIZE_WORD to hash table.
 * 
 * Revision 1.21  92/04/29  08:21:15  shen
 * redefine MAX_OCCURANCES to a very big number: 0x10000000.
 * 
 * Revision 1.20  92/03/20  11:04:18  jonathan
 * Added word_postition argument to add word.  See irext.h for explanation.
 * 
 * Revision 1.19  92/03/01  16:11:57  brewster
 * took out analyze_hashtable_distribution
 * 
 * Revision 1.18  92/02/24  19:58:04  jonathan
 * Added code to add the dictionary to the hastable on startup.
 * 
 * Revision 1.17  92/02/12  13:46:18  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

/* To Do:
 *  done: Improve the hashing functions.
 *  done: stop inserting into hash table after max number have been accumulated
 *  done: make flush not flush buffers that are too big.
 */
 
#include "panic.h"
#include "cutil.h"
#include "futil.h"
#include "irfiles.h"
#include "irhash.h"
#include "stoplist.h"
#include "irinv.h"
#include "sigindex.h"

#ifdef UNIX
#define PRINT_AS_INDEXING true /* also defined in irtfiles.c and irfiles.c */
#else 
#define PRINT_AS_INDEXING false
#endif

/* ---------------------------------------------------- */
static hash_entry* look_up_word _AP((char* word,hashtable*
				     the_word_memory_hashtable));
  
static hash_entry* 
look_up_word(word,the_word_memory_hashtable)
char* word;
hashtable* the_word_memory_hashtable;
{
  hash_entry * answer = get_hash(word, the_word_memory_hashtable);
  
  if(NULL != answer)
    return(answer);
  else{
    hash_entry wrd_entry;
    answer = put_hash(word, the_word_memory_hashtable, &wrd_entry);
    answer->number_of_occurances = 0;
    answer->memory_ptr = NULL;
    answer->memory_size = 0;
    answer->current_memory_ptr = answer->memory_ptr;
    answer->current_doc_id = 0;
    return(answer);
  }
}

#ifdef NOTUSED
static unsigned char add_weight _AP((long current_weight,long new_weight));

static unsigned char 
add_weight(current_weight,new_weight)
long current_weight;
long new_weight;
/* add a new weight to the existing one */
{
  /* this should be smarter than this, like doing the log or something */
  if(127 < (current_weight + new_weight)){
    /* the max char.  should be 255, but does not work on all compilers */
    return(127);
  }
  else{
    return(current_weight + new_weight);
  }
}

long write_bytes_to_memory(value,size,ptr)
long value;
long size;
unsigned char* ptr;
{
  /* writes the number into memory lsb first.  
     returns the number of bytes written */
  long i;
  long original_value = value;

  if(size < 0) /* paranoia */
    panic("attempting to write a negative number of bytes");

  ptr += size; /* start at the end of the block and write backwards */
  for (i = 0; i < size; i++){
    ptr--;
    *ptr = (unsigned char)(value & 0xFF);
    value = value >> 8;
  }
  if(value != 0)
    panic("In a call to write_bytes_to_memory, the value %ld can not be represented in %ld bytes", original_value, size);

  return(size);
}
		
#endif /* def NOTUSED */

/* adds a word to the hashtable. 
 * Returns the 0 if successful. See irext.h for more documentation.
 */
long add_word(word, char_pos, line_pos,
	      weight, doc_id, date, word_pair, db, word_position)
     char *word;	/* the word to be indexed, this could be a
			   word pair. If NULL there are no more words
			   to be indexed */
     long char_pos;	/* the position of the start of the
			   word */
     long line_pos;	/* this is passed for the best
			   section calculation */
     long weight;	/* how important the word looks
			   syntactically (such as is it bold)
			   NOT used by signature system */
     long doc_id; 	/* current document, this will never be 0 */
     time_t date; /* display day of this document, 0 if not known */
     long word_pair;
     database* db; /* database to insert the document */
     boolean word_position; /* ignored here. */
{
  /* look up the word in the hashtable */
  /* creates it if necessary */	
  hash_entry* wrd_entry;
  hashtable * the_word_memory_hashtable = db->the_word_memory_hashtable;
  /* printf("Word: '%s' doc_id: %ld, pos: %ld, weight: %ld\n",
     word, doc_id, char_pos, weight); */
  
  if(NULL == db->the_word_memory_hashtable){
    panic("The memory word hashtable is not defined.");
  }

  /* if we have indexed enough words flush the memory copies to disk.
  if(db->number_of_words_in_hashtable == db->flush_after_n_words)
    flush_memory_hashtable_to_disk(db, false);
    ** not done on sig system **
 */
  
  wrd_entry = look_up_word(word, the_word_memory_hashtable);
  wrd_entry->number_of_occurances ++;

  /* check if we have too many of this word before we add it */
#undef MAX_OCCURANCES
#define MAX_OCCURANCES 0x10000000
  if(wrd_entry->number_of_occurances < MAX_OCCURANCES){
    db->number_of_words_in_hashtable ++;
    sig_add_word(word, char_pos, line_pos, weight, doc_id, date, word_pair);
  }
  return(0L);
}

#ifndef AUTO_STOPWORDS
void add_stop_words(the_word_memory_hashtable)
hashtable *the_word_memory_hashtable;
     /* add the stop words to the hashtable.  this must be done before
	adding other words */
{
  init_stop_list();
  while(true){
    char *word = next_stop_word();
    hash_entry* wrd_entry;

    if(NULL == word)
      break;
    wrd_entry = look_up_word(word, the_word_memory_hashtable);
    wrd_entry->number_of_occurances = STOP_WORD_FLAG;
  }
}
#endif

long finished_add_word(db)
database *db;
{
  /* write out the dictioanry */
  long i;
  long num_words;

  db->number_of_words = hashtable_count(db->the_word_memory_hashtable);
  init_dict_file_for_writing(db);
  /* analyze_hashtable_distribution(db->the_word_memory_hashtable); */
  sort_hashtable(db->the_word_memory_hashtable);
  /* exclude the last word which is DICTIONARY_TOTAL_SIZE_WORD */
  num_words = hashtable_count(db->the_word_memory_hashtable);
  if ( 0 == strcmp(db->the_word_memory_hashtable->contents[num_words-1].key,
                   DICTIONARY_TOTAL_SIZE_WORD) ) 
    num_words--;
   
  for(i = 0; i < num_words; i++){
    hash_entry * entry = &db->the_word_memory_hashtable->contents[i];
    if(0 == (STOP_WORD_FLAG & entry->number_of_occurances)){
      /* write out the dictionary entry */
      /* printf("Adding word: %s %ld entries\n", entry->word, entry->number_of_occurances); */
      /* the position in this world is a unique id for every word */
      add_word_to_dictionary(entry->key, i,
			     entry->number_of_occurances, db);
    }
  }
  finished_add_word_to_dictionary(db);
  return(sig_finished_add_word(db));
}

/* Add the dictionary to the hastable */

void add_dictionary_to_hashtable(db)
database *db;
{
  /* prints the contents of a dictionary */
  FILE *stream = db->dictionary_stream;
  long i, j, new_number_of_dictionary_blocks;
  extern unsigned char *dictionary_header_block, *dictionary_block;
  extern long number_of_dictionary_blocks;

  if(stream != NULL) {
    waislog(WLOG_LOW, WLOG_INFO, "Adding dictionary to hastable");

    s_fseek(stream, 0L, SEEK_SET);
    new_number_of_dictionary_blocks = read_bytes(DICTIONARY_HEADER_SIZE, stream);
    if(new_number_of_dictionary_blocks > number_of_dictionary_blocks)
      dictionary_header_block = NULL;
    number_of_dictionary_blocks = new_number_of_dictionary_blocks;
    if(NULL == (dictionary_header_block =
		read_dictionary_block(dictionary_header_block,
				      DICTIONARY_HEADER_SIZE,
				      number_of_dictionary_blocks,
				      stream))) {    
      waislog(WLOG_MEDIUM, WLOG_WARNING,
	      "Could not read dictionary header block");
      return;
    }
    for(i = 0; i < number_of_dictionary_blocks; i++){
      long pos = dictionary_block_position(i, dictionary_header_block);
      if(NULL == (dictionary_block =
		  read_dictionary_block(dictionary_block,
					pos, DICTIONARY_BLOCK_SIZE, stream))) {
	waislog(WLOG_MEDIUM, WLOG_WARNING,
		"Could not read dictionary block %ld", pos);
      }
      else
	/* iterate over words */
	for(j = 0; j < DICTIONARY_BLOCK_SIZE; j++) {
	  char *word = dictionary_block_word(j, dictionary_block);
	  hash_entry* wrd_entry;

	  if(word[0] == '\0' )
	    break;
	  wrd_entry = look_up_word(word, db->the_word_memory_hashtable);
	  wrd_entry->number_of_occurances = 
	    dictionary_block_word_occurances(j, dictionary_block);
	}
    }
  }
}

long init_add_word(db, hashtable_size, flush_after_n_words)
     database *db;
     long hashtable_size;
     long flush_after_n_words;
{
  if(NULL != db->the_word_memory_hashtable)
    free_hashtable(db->the_word_memory_hashtable);
  db->the_word_memory_hashtable =
    make_hashtable(0, hashtable_size, sizeof(hash_entry));
  db->flush_after_n_words = 0x7FFFFFFF;  /* a large number */
  sig_init_add_word(db, BATCH_UPDATE, ADD_UPDATE);
  add_dictionary_to_hashtable(db);
#ifndef AUTO_STOPWORDS
  add_stop_words(db->the_word_memory_hashtable);
#else
  add_stop_words(db->the_word_memory_hashtable,db->database_file);
#endif
  return(0);
}
