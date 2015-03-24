/*                               -*- Mode: C -*- 
 * 
 * irhash.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Tue Apr  4 11:11:50 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Wed Oct 11 20:58:04 1995
 * Language        : C
 * Update Count    : 8
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * (C) Copyright CNIDR (see ../doc/CNIDR/COPYRIGHT) 
 * $$
 * 
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* The memory hashtables for building an index. */
/* -brewster 5/90 */

/* Change log:
 * $Log: irhash.c,v $
 * Revision 2.0.1.3  1995/10/11 20:12:06  pfeifer
 * patch37: Bad memory bug. Tung copied add_word to field_add_word and
 * patch37: added one more than obsolete '+'.
 *
 * Revision 2.0.1.2  1995/09/15  09:44:43  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0.1.1  1995/09/11  12:50:10  pfeifer
 * patch4: Added typecasts for writeCompressedInteger() to make gcc
 * patch4: happy.
 *
 * Revision 2.0  1995/09/08  07:59:50  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.16  1995/08/10  11:40:19  pfeifer
 * patch by Tom Snee
 *
 * Revision 1.15  1995/05/07  16:04:57  pfeifer
 * More fixes by Tom Snee. Literal serach now works again!
 *
 * Revision 1.13  1995/04/30  15:32:21  pfeifer
 * Sicheung vor unifdef PROXIMITY
 *
 * Revision 1.12  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.11  1995/04/17  13:27:18  pfeifer
 * Fixes by Tom Snee
 *
 * Revision 1.10  1995/04/17  13:01:16  pfeifer
 * removed bad free in flush_word_occurance_buffers
 *
 * Revision 1.8  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.7  1994/06/20  15:41:35  huynh1
 * missing parameter field_id added by calling
 * field_flush_memory_hashtable_to_disk(db, false, field_id).
 *
 * Revision 1.6  1994/06/06  11:01:27  huynh1
 * add_word, look_up_word updated.
 *
 * Revision 1.5  1994/05/27  09:23:26  huynh1
 * add_word, field_add_word updated. beta
 *
 * Revision 1.4  1994/05/20  12:56:13  pfeifer
 * beta
 *
 * Revision 1.3  1994/03/08  20:42:41  huynh1
 * Patchlevel 04
 *
 * Revision 1.2  1994/02/14  10:46:51  huynh1
 * new code for field concept added.
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.16  92/03/20  11:04:33  jonathan
 * Added code to handle switches for word_pairs and word_postition info.
 * 
 * Revision 1.15  92/02/12  13:26:28  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

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

/* To Do:
 *  done: Improve the hashing functions.
 *  done: stop inserting into hash table after max number have been accumulated
 *  done: make flush not flush buffers that are too big.
 */
 
#include "panic.h"
#include "cutil.h"
#include "irfiles.h"
#include "irhash.h"
#include "stoplist.h"
#include "irinv.h"
#ifdef NEW_WEIGHT
#include "weight.h"
#endif /* NEW_WEIGHT */
#ifdef LITERAL
#include "zutil.h"
#endif /* LITERAL */

#ifdef UNIX
#define PRINT_AS_INDEXING true /* also defined in irtfiles.c and irfiles.c */
#else /* not UNIX */
#define PRINT_AS_INDEXING false
#endif /* not UNIX */

/* ================================
   ===  Word Occurance Buffers  ===
   ================================ */

/* Word occurance buffers
 * This is a simple memory allocator for use with the word memory hashtable.
 * Since the buffers are tiny, this is done as a copy-sweep GC scheme.
 * Oh, I long for the storage system of lisp.
 */
unsigned char *first_word_occurance_buffer = NULL;  /* allocate blocks out of this */
unsigned char *last_word_occurance_buffer = NULL;
long word_occurance_block_length = 256000;  /* maybe this should be larger? */
unsigned char * word_occurance_free_ptr = NULL;

unsigned char *make_word_occurrance_block(size)
long size;

{
  /* allocates a word_occurance_block out of the buffers */
  /* old way: s_malloc((size_t)size); */
  /* returns a pointer to a piece of memory */
  if(NULL == first_word_occurance_buffer){
    /* initialize it */
    first_word_occurance_buffer = 
      (unsigned char *)calloc((size_t)MAXIMUM(word_occurance_block_length,
					      sizeof(size_t)+ size),
			      (size_t)1);
#if 0
      (unsigned char *)s_malloc(MAXIMUM(word_occurance_block_length,
					sizeof(size_t)+ size));
#endif /* 0 */
    *(unsigned char **)first_word_occurance_buffer = NULL; /* set the end */
    last_word_occurance_buffer = first_word_occurance_buffer;
    word_occurance_free_ptr = first_word_occurance_buffer + sizeof(size_t);
  }
  if((long)word_occurance_free_ptr + size >= 
     word_occurance_block_length + (long)last_word_occurance_buffer){
    /* then allocate a new block */
    unsigned char * new_block = 
      (unsigned char *)calloc((size_t)MAXIMUM(word_occurance_block_length,
					      sizeof(size_t)+ size),
			      (size_t)1);
#if 0
    (unsigned char *)s_malloc(MAXIMUM(word_occurance_block_length, 
				      sizeof(size_t)+ size));
#endif /* 0 */
    if(new_block != NULL) {
      *(unsigned char **)new_block = NULL; /* set the end of the chain */
      *(unsigned char **)last_word_occurance_buffer = new_block;
      word_occurance_free_ptr = new_block + sizeof(size_t);
      last_word_occurance_buffer = new_block;
    } else return(NULL);
  }
  /* allocate away */	
  { unsigned char * answer = word_occurance_free_ptr;
    word_occurance_free_ptr += size;	
    return(answer);  
  }
}

void free_word_occurance_block(block)
unsigned char *block;
{
  /* this is not used with the new scheme, but is here in case
     malloc is a win on some systems */
  /* s_free(block);  */
}

static void flush_word_occur_bufs_internal
  _AP((unsigned char* head_of_list));

static void flush_word_occur_bufs_internal(head_of_list)
unsigned char* head_of_list;
/* frees all word occurance buffers.  This should be done with care */
{
  while(1){
    unsigned char * next_block;
    if(NULL == head_of_list)
      break;
    next_block = *(unsigned char **)head_of_list;
    s_free(head_of_list);
    head_of_list = next_block;
  }
}

void flush_word_occurance_buffers()
{
  /* frees all word occurance buffers.  This should be done with care */
  flush_word_occur_bufs_internal(first_word_occurance_buffer);
  /* this seems to be obsolete (up)
  if(first_word_occurance_buffer != NULL) {
    s_free(first_word_occurance_buffer);  
  }
  */
  first_word_occurance_buffer = NULL;
  word_occurance_free_ptr = NULL;
  last_word_occurance_buffer = NULL;
}

/* ---------------------------------------------------- */
#ifdef NEW_WEIGHT
extern long max_term_frequency; /* defined in weight.c */
#endif /* NEW_WEIGHT */

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
    answer->memory_ptr = 
      make_word_occurrance_block(WORD_MEMORY_INIT_BLOCK_SIZE);
    answer->current_memory_ptr = answer->memory_ptr;
    answer->memory_size = WORD_MEMORY_INIT_BLOCK_SIZE;
    answer->current_doc_id = 0;
#ifdef NEW_WEIGHT
    answer->occurances_in_doc = 0;
#endif /* NEW_WEIGHT */
#ifdef LITERAL
    answer->docs_incl_term = 0;
    answer->last_pos_in_doc = 0;
    answer->index_element_current_pos = 
	answer->current_memory_ptr - answer->memory_ptr;
#endif /* LITERAL */
    return(answer);
  }
}

#ifdef LITERAL
hash_entry* 
add_word_saved_into_hashtable(word,doc_id,occurances_in_doc,char_list_size,the_word_memory_hashtable)
     char* word;
     long doc_id;
     long occurances_in_doc;
     long char_list_size;
     hashtable* the_word_memory_hashtable;
#else /* not LITERAL */
hash_entry* 
add_word_saved_into_hashtable(word,doc_id,occurances_in_doc,the_word_memory_hashtable)
     char* word;
     long doc_id;
     long occurances_in_doc;
     hashtable* the_word_memory_hashtable;
#endif /* not LITERAL */
{
  hash_entry * answer = get_hash(word, the_word_memory_hashtable);
  
  if(NULL != answer)
    return(answer);
  else{
    hash_entry wrd_entry;
    answer = put_hash(word, the_word_memory_hashtable, &wrd_entry);
    answer->number_of_occurances = occurances_in_doc;
#ifdef LITERAL
    answer->memory_ptr = 
      make_word_occurrance_block(INDEX_ELEMENT_SIZE + char_list_size);
    answer->memory_size = INDEX_ELEMENT_SIZE + char_list_size;
#else /* not LITERAL */
    answer->memory_ptr = 
      make_word_occurrance_block(WORD_MEMORY_INIT_BLOCK_SIZE);
    answer->memory_size = WORD_MEMORY_INIT_BLOCK_SIZE;
#endif /* not LITERAL */
    answer->current_memory_ptr = answer->memory_ptr;
    answer->current_doc_id = doc_id;
#ifdef NEW_WEIGHT
    answer->occurances_in_doc = occurances_in_doc;
#endif /* NEW_WEIGHT */
#ifdef LITERAL
    answer->docs_incl_term = 1;
    answer->last_pos_in_doc = 0;
    answer->index_element_current_pos = 
	answer->current_memory_ptr - answer->memory_ptr;
#endif /* LITERAL */

#ifdef AUTO_STOPWORDS
    answer->stop_flag = 0;
#endif /* AUTO_STOPWORDS */
    
    /* add away */
    answer->current_memory_ptr +=
      write_bytes_to_memory(doc_id, DOCUMENT_ID_SIZE,
			    answer->current_memory_ptr);
#ifdef LITERAL
    answer->current_memory_ptr +=
      write_bytes_to_memory(0L, 
			    NUMBER_OF_OCCURANCES_SIZE,
			    answer->current_memory_ptr);
#else /* not LITERAL */
    answer->current_memory_ptr +=
      write_bytes_to_memory(0L, 
			    CHARACTER_POSITION_SIZE,
			    answer->current_memory_ptr);
#endif /* not LITERAL */
#ifdef NEW_WEIGHT
    answer->current_memory_ptr +=
      write_bytes_to_memory(0L,
			    NEW_WEIGHT_SIZE,
			    answer->current_memory_ptr);
    if(answer->occurances_in_doc > max_term_frequency)
      max_term_frequency = answer->occurances_in_doc;
#else /* not NEW_WEIGHT */
    answer->current_memory_ptr +=
      write_bytes_to_memory(occurances_in_doc + 5,
			    WEIGHT_SIZE,
			    answer->current_memory_ptr);
#endif /* not NEW_WEIGHT */
    
    return(answer);
  }
}

static unsigned char add_weight _AP((long current_weight,long new_weight));

static unsigned char 
add_weight(current_weight,new_weight)
long current_weight;
long new_weight;
/* add a new weight to the existing one */
{
#ifndef NEW_WEIGHT
  /* this should be smarter than this, like doing the log or something */
  if(127 < (current_weight + new_weight)){
    /* the max char.  should be 255, but does not work on all compilers */
    return(127);
  }
  else{
    return(current_weight + new_weight);
  }
#else /* NEW_WEIGHT */
  return(current_weight + new_weight);
#endif /* NEW_WEIGHT */
}

static unsigned char* more_memory _AP((unsigned char* current_memory_ptr,
				       long current_memory_size,
				       long* new_size));

static unsigned char* more_memory(current_memory_ptr,current_memory_size,new_size)
     unsigned char* current_memory_ptr;
     long current_memory_size;
     long *new_size;
     /* Allocates more memory for a hash_entry.  It transfers all the bytes 
      * from the old to the new and then returns the new.
      */
{
  unsigned char* new_memory = NULL;
  if(current_memory_size > *new_size){
    panic("trying to contract a hash_entry block.  This is not right\n");
  }
  new_memory = make_word_occurrance_block(*new_size);
  
  if(NULL == new_memory){
    *new_size = 0;
    return(current_memory_ptr);
#if 0
    panic("Out of memory.");
#endif /* 0 */
  }
  memset(new_memory, 0, *new_size);
  memmove(new_memory, current_memory_ptr, (size_t)current_memory_size); 
  return(new_memory);
}

static long more_memory_size _AP((long current_size,
				  long number_of_occurances));

static long more_memory_size(current_size,number_of_occurances)
long current_size;
long number_of_occurances;
/* This is pretty important to get right.  This is a place holder */
{
  return(MAXIMUM(2 * current_size, WORD_MEMORY_INIT_BLOCK_SIZE));
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
		
/* adds a word to the hashtable. Returns the 0 if successful. 
   See irext.h for more documentation.
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
     boolean word_position; /* whether to have multiple entries for words in a document */
{
  /* look up the word in the hashtable */
  /* creates it if necessary */	
  hash_entry* wrd_entry; 
  char* newword;
  t_Synonym* syn_Table = db->syn_Table;
  int syn_Table_Size = db->syn_Table_Size;
  hashtable * the_word_memory_hashtable = db->the_word_memory_hashtable;
#ifdef LITERAL
  unsigned char* old_memory_ptr;
  long tmplen = 4; /* 4 bytes */
#endif /* LITERAL */

  /* printf("Word: '%s' doc_id: %ld, pos: %ld, weight: %ld\n",
     word, doc_id, char_pos, weight); */
  
  if(NULL == db->the_word_memory_hashtable){
    panic("The memory word hashtable is not defined.");
  }

  /* if we have indexed enough words flush the memory copies to disk. */
  if(db->number_of_words_in_hashtable == db->flush_after_n_words) {
    flush_memory_hashtable_to_disk(db, false, doc_id);
  }
  
  /* perform synonym table lookup and if the word is changed skip WordRoot */
  newword = lookup_Synonym( word,syn_Table,syn_Table_Size );
  strncpy( word,newword,MAX_WORD_LENGTH );
  wrd_entry = look_up_word(word, the_word_memory_hashtable);
  wrd_entry->number_of_occurances ++;

  if(wrd_entry->number_of_occurances > MAX_OCCURANCES){
    /* do nothing. we have enough of that word */
  }
  else{
    /* we have a word to add */
    db->number_of_words_in_hashtable ++;

#ifdef AUTO_STOPWORDS
    wrd_entry->stop_flag = 0; /* no stopword */
#endif /* AUTO_STOPWORDS */

    if(word_position || doc_id != wrd_entry->current_doc_id){
      /* then we have a new doc_id to add to the memory block */
      
      /* check to see if we need more memory */
      if((wrd_entry->memory_size -
          (wrd_entry->current_memory_ptr - 
           wrd_entry->memory_ptr) 
          < 
          INDEX_ELEMENT_SIZE)){
        /* we need more memory. this makes more and frees the old*/
        long new_size = 
	  more_memory_size(wrd_entry->memory_size,
			   wrd_entry->number_of_occurances);
#ifdef LITERAL
	old_memory_ptr = wrd_entry->memory_ptr;
#else /* not LITERAL */
        unsigned char* old_memory_ptr = wrd_entry->memory_ptr;
#endif /* not LITERAL */
        /* cprintf(PRINT_AS_INDEXING, "Get more memory %ld bytes for %s\n", new_size, word); */
        wrd_entry->memory_ptr = 
          more_memory(wrd_entry->memory_ptr, wrd_entry->memory_size,
                      &new_size);
	if(new_size == 0) { /* take memory place for new entries */
#ifdef NEW_WEIGHT
	  extern long number_of_bucket_ids; /* defined in irtfiles.c */
	  number_of_bucket_ids -= 1;
#endif /* NEW_WEIGHT */
	  flush_memory_hashtable_to_disk(db, false, doc_id);
	  add_word(word, char_pos, line_pos,  weight, doc_id, date, 
		   word_pair, db, word_position);
	  return(0L);
	}
	wrd_entry->current_memory_ptr = 
	  wrd_entry->memory_ptr + /* new offset */
	    (wrd_entry->current_memory_ptr - old_memory_ptr);
	/* just being paranoid... no longer illegal
	   if(wrd_entry->current_memory_ptr == wrd_entry->memory_ptr)
	   panic("After allocating more memory, the size went to 0");
           */
        wrd_entry->memory_size = new_size;
      }				/* finished making more memory */
#ifdef LITERAL
      wrd_entry->index_element_current_pos = 
	wrd_entry->current_memory_ptr - wrd_entry->memory_ptr;
#endif /* LITERAL */
      /* add away */
      wrd_entry->current_memory_ptr +=
        write_bytes_to_memory(doc_id, DOCUMENT_ID_SIZE,
                              wrd_entry->current_memory_ptr);
#ifdef LITERAL
      ++(wrd_entry->docs_incl_term);
      wrd_entry->current_memory_ptr +=
        write_bytes_to_memory(0L, 
			      NUMBER_OF_OCCURANCES_SIZE,
                              wrd_entry->current_memory_ptr);
#else /* not LITERAL */
      wrd_entry->current_memory_ptr +=
        write_bytes_to_memory(char_pos, 
                              CHARACTER_POSITION_SIZE,
                              wrd_entry->current_memory_ptr);
#endif /* not LITERAL */
#ifdef NEW_WEIGHT
      wrd_entry->current_memory_ptr +=
	write_bytes_to_memory(0L,
			      NEW_WEIGHT_SIZE,
                              wrd_entry->current_memory_ptr);
      wrd_entry->occurances_in_doc += 1;
      if(wrd_entry->occurances_in_doc > max_term_frequency)
	max_term_frequency = wrd_entry->occurances_in_doc;
#else /* not NEW_WEIGHT */
      wrd_entry->current_memory_ptr +=
        write_bytes_to_memory(weight +
                              /* add 5 since for the first one */
                              (doc_id != wrd_entry->current_doc_id)?5:0,
                              WEIGHT_SIZE,
                              wrd_entry->current_memory_ptr);
#endif /* not NEW_WEIGHT */
#ifdef LITERAL
      wrd_entry->last_pos_in_doc = char_pos;
      wrd_entry->current_memory_ptr +=
        write_bytes_to_memory(char_pos, 
                              CHARACTER_POSITION_SIZE,
                              wrd_entry->current_memory_ptr);
#endif /* LITERAL */
      wrd_entry->current_doc_id = doc_id;
      
    }
    else{
      /* The word is already there,
       * just increment the weight in the record.
       * This will change when/if position information is kept (for proximity).
       */
      if(wrd_entry->current_memory_ptr == wrd_entry->memory_ptr){
        panic("Memory hashtable error. Recorded doc_id %ld, current doc_id %ld\n",
              wrd_entry->current_doc_id, doc_id);
      }
#ifdef LITERAL
      if((wrd_entry->memory_size -
          (wrd_entry->current_memory_ptr - 
           wrd_entry->memory_ptr) 
          < writtenCompressedIntSize(char_pos - wrd_entry->last_pos_in_doc))) {
	long new_size = 
	  more_memory_size(wrd_entry->memory_size,
			   wrd_entry->number_of_occurances);
	old_memory_ptr = wrd_entry->memory_ptr;
	wrd_entry->memory_ptr = 
	  more_memory(wrd_entry->memory_ptr,wrd_entry->memory_size, &new_size);
	if(new_size == 0) { /* take memory place for new entries */
	  flush_memory_hashtable_to_disk(db, false, doc_id);
	  add_word(word, char_pos, line_pos,  weight, doc_id, date, 
		   word_pair, db, word_position);
	  return(0L);
	}
	wrd_entry->current_memory_ptr = 
	  wrd_entry->memory_ptr + /* new offset */
	    (wrd_entry->current_memory_ptr - old_memory_ptr);
	wrd_entry->memory_size =  new_size;
      }
      wrd_entry->current_memory_ptr = (unsigned char *)
	writeCompressedInteger(char_pos - wrd_entry->last_pos_in_doc,
			       wrd_entry->current_memory_ptr,
			       &tmplen);
#if 0      
      wrd_entry->current_memory_ptr +=
	write_bytes_to_memory(char_pos, 
			      CHARACTER_POSITION_SIZE,
			      wrd_entry->current_memory_ptr);
#endif /* 0       */
      wrd_entry->last_pos_in_doc = char_pos;

#endif /* LITERAL */
#ifdef NEW_WEIGHT
      wrd_entry->occurances_in_doc += 1;
      if(wrd_entry->occurances_in_doc > max_term_frequency)
	max_term_frequency = wrd_entry->occurances_in_doc;
#else /* not NEW_WEIGHT */
      *(wrd_entry->current_memory_ptr - 1) =
        add_weight(*(wrd_entry->current_memory_ptr - 1), weight);
#endif /* not NEW_WEIGHT */
    }
  }

  return(0L);
}

#ifdef FIELDS
long field_add_word(word, char_pos, line_pos,
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
     boolean word_position; /* whether to have multiple entries for words in a document */
{
  /* look up the word in the hashtable */
  /* creates it if necessary */	

  char* newword;
  t_Synonym* syn_Table = db->syn_Table;
  int syn_Table_Size = db->syn_Table_Size;
  hashtable * the_word_field_memory_hashtable = db->the_word_memory_hashtable;
  hash_entry* wrd_field_entry;
  long field_id = db->current_field_position; 
#ifdef LITERAL
  unsigned char* old_memory_ptr;
  long tmplen = 4; /* 4 bytes */
#endif /* LITERAL */

  /* printf("Word: '%s' doc_id: %ld, pos: %ld, weight: %ld\n",
     word, doc_id, char_pos, weight); */
  
  if(NULL == db->the_word_memory_hashtable){
    panic("The memory word hashtable is not defined.");
  }
  
  /* if we have indexed enough words flush the memory copies to disk. */
  if(db->fields[field_id].number_of_words_in_hashtable == db->flush_after_n_words) {
    field_flush_memory_hashtable_to_disk(db, false, field_id, doc_id);
  }
  
    /* perform synonym table lookup and if the word is changed skip WordRoot */
    newword = lookup_Synonym( word,syn_Table,syn_Table_Size );
    strncpy( word,newword,MAX_WORD_LENGTH );

  wrd_field_entry = 
    look_up_word(word, the_word_field_memory_hashtable);
  wrd_field_entry->number_of_occurances++;

  if(wrd_field_entry->number_of_occurances > MAX_OCCURANCES){
    /* do nothing. we have enough of that word */
  }
  else{
    /* we have a word to add */
    db->fields[field_id].number_of_words_in_hashtable++;
    
#ifdef AUTO_STOPWORDS
    wrd_field_entry->stop_flag = 0; /* no stopword */
#endif /* AUTO_STOPWORDS */

    if(word_position || doc_id != wrd_field_entry->current_doc_id){
      /* then we have a new doc_id to add to the memory block */
      
      /* check to see if we need more memory */
      if((wrd_field_entry->memory_size -
          (wrd_field_entry->current_memory_ptr - 
           wrd_field_entry->memory_ptr) 
          < 
          INDEX_ELEMENT_SIZE)){
        /* we need more memory. this makes more and frees the old*/
        long new_size =
	  more_memory_size(wrd_field_entry->memory_size,
			   wrd_field_entry->number_of_occurances);

#ifdef LITERAL
	old_memory_ptr = wrd_field_entry->memory_ptr;
#else /* not LITERAL */
        unsigned char* old_memory_ptr = wrd_field_entry->memory_ptr;
#endif /* not LITERAL */
        /* cprintf(PRINT_AS_INDEXING, "Get more memory %ld bytes for %s\n", new_size, word); */
        wrd_field_entry->memory_ptr = 
          more_memory(wrd_field_entry->memory_ptr, 
		      wrd_field_entry->memory_size, &new_size);
	if(new_size == 0) { /* take memory place for new entries */
#ifdef NEW_WEIGHT
	  extern long number_of_bucket_ids; /* defined in irtfiles.c */
	  number_of_bucket_ids -= 1;
#endif /* NEW_WEIGHT */
	  field_flush_memory_hashtable_to_disk(db, false, field_id, doc_id);
	  field_add_word(word, char_pos, line_pos, weight, doc_id, date, 
			 word_pair, db, word_position);
	  return(0L);
	}
        wrd_field_entry->current_memory_ptr = 
          wrd_field_entry->memory_ptr + /* new offset */
            (wrd_field_entry->current_memory_ptr - old_memory_ptr);
        /* just being paranoid... no longer illegal
           if(wrd_field_entry->current_memory_ptr == wrd_field_entry->memory_ptr)
           panic("After allocating more memory, the size went to 0");
           */
        wrd_field_entry->memory_size = new_size;
      }				/* finished making more memory */
#ifdef LITERAL
      wrd_field_entry->index_element_current_pos = 
	wrd_field_entry->current_memory_ptr - wrd_field_entry->memory_ptr;
#endif /* LITERAL */
      /* add away */
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(doc_id, DOCUMENT_ID_SIZE,
                              wrd_field_entry->current_memory_ptr);
#ifdef LITERAL
      ++(wrd_field_entry->docs_incl_term);
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(0L, 
			      NUMBER_OF_OCCURANCES_SIZE,
                              wrd_field_entry->current_memory_ptr);
#else /* not LITERAL */
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(char_pos, 
                              CHARACTER_POSITION_SIZE,
                              wrd_field_entry->current_memory_ptr);
#endif /* not LITERAL */
#ifdef NEW_WEIGHT
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(0,
                              NEW_WEIGHT_SIZE,
                              wrd_field_entry->current_memory_ptr);
      wrd_field_entry->occurances_in_doc += 1;
      if(wrd_field_entry->occurances_in_doc > max_term_frequency)
	max_term_frequency = wrd_field_entry->occurances_in_doc;
#else /* not NEW_WEIGHT */
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(weight +
                              /* add 5 since for the first one */
                              (doc_id != wrd_field_entry->current_doc_id)?5:0,
                              WEIGHT_SIZE,
                              wrd_field_entry->current_memory_ptr);
#endif /* not NEW_WEIGHT */
#ifdef LITERAL
      wrd_field_entry->last_pos_in_doc = char_pos;
      wrd_field_entry->current_memory_ptr +=
        write_bytes_to_memory(char_pos, 
                              CHARACTER_POSITION_SIZE,
                              wrd_field_entry->current_memory_ptr);
#endif /* LITERAL */
      wrd_field_entry->current_doc_id = doc_id;
      
    }
    else{
      /* The word is already there,
       * just increment the weight in the record.
       * This will change when/if position information is kept (for proximity).
       */
      if(wrd_field_entry->current_memory_ptr == wrd_field_entry->memory_ptr){
        panic("Memory hashtable error. Recorded doc_id %ld, current doc_id %ld\n",
              wrd_field_entry->current_doc_id, doc_id);
      }
#ifdef LITERAL
      if((wrd_field_entry->memory_size -
          (wrd_field_entry->current_memory_ptr - 
           wrd_field_entry->memory_ptr) 
          < writtenCompressedIntSize(char_pos - wrd_field_entry->last_pos_in_doc))) {
	long new_size =
	  more_memory_size(wrd_field_entry->memory_size,
			   wrd_field_entry->number_of_occurances);
	old_memory_ptr = wrd_field_entry->memory_ptr;
	wrd_field_entry->memory_ptr = 
	  more_memory(wrd_field_entry->memory_ptr,wrd_field_entry->memory_size,
		      &new_size);
	if(new_size == 0) { /* take memory place for new entries */
	  field_flush_memory_hashtable_to_disk(db, false, field_id, doc_id);
	  field_add_word(word, char_pos, line_pos,  weight, doc_id, date, 
			 word_pair, db, word_position);
	  return(0L);
	}
	wrd_field_entry->current_memory_ptr = 
	  wrd_field_entry->memory_ptr + /* new offset */
	    (wrd_field_entry->current_memory_ptr - old_memory_ptr);
	wrd_field_entry->memory_size = new_size; 
      }
      wrd_field_entry->current_memory_ptr = (unsigned char *)
	writeCompressedInteger(char_pos - wrd_field_entry->last_pos_in_doc,
			       wrd_field_entry->current_memory_ptr,
			       &tmplen);
#if 0
      wrd_field_entry->current_memory_ptr +=
	write_bytes_to_memory(char_pos, 
			      CHARACTER_POSITION_SIZE,
			      wrd_field_entry->current_memory_ptr);
#endif /* 0 */
      wrd_field_entry->last_pos_in_doc = char_pos;

#endif /* LITERAL */
#ifdef NEW_WEIGHT
      wrd_field_entry->occurances_in_doc += 1;
      if(wrd_field_entry->occurances_in_doc > max_term_frequency)
	max_term_frequency = wrd_field_entry->occurances_in_doc;
#else /* not NEW_WEIGHT */
      *(wrd_field_entry->current_memory_ptr - 1) =
        add_weight(*(wrd_field_entry->current_memory_ptr - 1), weight);
#endif /* not NEW_WEIGHT */
    }
  }
  return(0L);
}
#endif /* FIELDS */

#ifdef AUTO_STOPWORDS
void add_stop_words(the_word_memory_hashtable, database_file)
     hashtable *the_word_memory_hashtable;
     char* database_file;
#else /* not AUTO_STOPWORDS */
void add_stop_words(the_word_memory_hashtable)
     hashtable *the_word_memory_hashtable;
#endif /* not AUTO_STOPWORDS */
     /* add the stop words to the hashtable.  this must be done before
	adding other words */
{
#ifdef AUTO_STOPWORDS
  init_stop_list(database_file);
#else /* not AUTO_STOPWORDS */
  init_stop_list();
#endif /* not AUTO_STOPWORDS */
  while(true){
    char *word = next_stop_word();
    hash_entry* wrd_entry;

    if(NULL == word)
      break;
    wrd_entry = look_up_word(word, the_word_memory_hashtable);
    wrd_entry->number_of_occurances = STOP_WORD_FLAG;
#ifdef AUTO_STOPWORDS
    wrd_entry->stop_flag = 1;
#endif /* AUTO_STOPWORDS */
  }
}

