/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Include file for the irhash.c file.
   Implements the building functions in irext.h */

#ifndef IRHASH_H
#define IRHASH_H

#include "cdialect.h"
#include "cutil.h"
#include "hash.h"
#include "irfiles.h"

/* the amount of memory for word occurances (bytes) */
#ifdef NEW_WEIGHT /* tung, 5/94 */
#ifdef LITERAL /* tung, 2/95 */
#define WORD_MEMORY_INIT_BLOCK_SIZE (DOCUMENT_ID_SIZE+NUMBER_OF_OCCURANCES_SIZE+NEW_WEIGHT_SIZE+CHARACTER_POSITION_SIZE)
#else
#define WORD_MEMORY_INIT_BLOCK_SIZE (DOCUMENT_ID_SIZE+CHARACTER_POSITION_SIZE+WEIGHT_SIZE+NEW_WEIGHT_SIZE)
#endif
#else
/* #define WORD_MEMORY_INIT_BLOCK_SIZE 10 */
#ifdef LITERAL /* tung, 2/95 */
#define WORD_MEMORY_INIT_BLOCK_SIZE (DOCUMENT_ID_SIZE+NUMBER_OF_OCCURANCES_SIZE+WEIGHT_SIZE+CHARACTER_POSITION_SIZE)
#else
#define WORD_MEMORY_INIT_BLOCK_SIZE (DOCUMENT_ID_SIZE+CHARACTER_POSITION_SIZE+WEIGHT_SIZE)
#endif
#endif

/* this is the maximum number of occurances that will be stored in the 
 * disk table.  The number of occurances will reflect the total number in
 * all files.  The theory is that if a word is very common, then it
 * is not very useful in descriminating between files.  Also, if it
 * is very common, then it takes up alot of space.
 * Maybe this should be dependent on the number of documents indexed.
 * Therefore if a word is in every document, then it probably does not mean
 * much.  
 * In increasing this, it may not keep all the references in the 
 * inverted file because the max length of an index block is governed
 * by a size that can be represented in INDEX_BLOCK_SIZE_SIZE bytes.
 */
#define MAX_OCCURANCES 20000L
/*#define MAX_OCCURANCES (STOP_WORD_FLAG-1) */

/* this is a flag to be put in the number_of_occurances field of a
word_entry so that it is always greater than the limit and no words will be
collected. */ 
#define STOP_WORD_FLAG (MAX_OCCURANCES + 1)
/*#define STOP_WORD_FLAG ((long)(1 << (NUMBER_OF_OCCURANCES_SIZE * 8 -2)))*/

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

unsigned char *make_word_occurrance_block _AP((long size));

void free_word_occurance_block _AP((unsigned char *block));

void flush_word_occurance_buffers _AP((void));

void gc_word_occurance_buffers _AP((hashtable * the_word_memory_hashtable));

#ifdef AUTO_STOPWORDS /* tung, 2/95 */
void add_stop_words _AP((hashtable *the_word_memory_hashtable, char* database_file));
#else
void add_stop_words _AP((hashtable *the_word_memory_hashtable));
#endif

long write_bytes_to_memory _AP((long value,long size,unsigned char* ptr));

#ifdef LITERAL /* tung, 2/95 */
hash_entry* add_word_saved_into_hashtable _AP((char* word,long doc_id,long occurances_in_doc,
					       long char_list_size,hashtable* the_word_memory_hashtable ));
#else
hash_entry* add_word_saved_into_hashtable _AP((char* word,long doc_id,long occurances_in_doc,
					       hashtable* the_word_memory_hashtable ));
#endif

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* nded IRHASH_H */
