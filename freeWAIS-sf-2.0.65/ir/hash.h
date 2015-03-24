/* hash table routines.  not very general.
 * -brewster
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef HASH_H
#define HASH_H


#define DEFAULT_NUMBER_OF_BUCKETS 65536L
#define DEFAULT_NUMBER_OF_ELEMENTS 131072L

#define MAX_KEY_SIZE MAX_WORD_LENGTH /* this is the string length, so add 1 */

typedef struct hash_entry{
  char key[MAX_KEY_SIZE + 1];	/* the key.  Must be first */
  long next;	/* index of the next entry in the contents */


  /* This part is usage dependent.  Sucks that it is hard coded. (this 
   * was done for efficiency. C sucks.)
   * however, this can be made to be somewhat flexible, by pulling this out
   * into a different .h file, and redefining the structure for different 
   * purposes.  This can be done in the same application since all the 
   * hashtable code cares about is the key and next values.
   * (actually, not quite, take out array refs to contents in hash.c
   *  and replace by explicit multiplies and it will work).
   */
  
  long number_of_occurances;	/* total for the whole db */
  unsigned char* memory_ptr;		/* what will go into the next block */
  unsigned char* current_memory_ptr;	/* the fill ptr into memory_ptr */
  long memory_size;		/* the size of memory_ptr */
  long current_doc_id;		/* the last document-id in memory_ptr
				 * this will change a page pointer eventually
				 */
#ifdef NEW_WEIGHT
  long occurances_in_doc;        /* frequency of word in a document */
#endif /* NEW_WEIGHT */
#ifdef AUTO_STOPWORDS
  int stop_flag;
#endif /* AUTO_STOPWORDS */
#ifdef LITERAL
  long index_element_current_pos;
  long docs_incl_term;
  long last_pos_in_doc;
#endif /* LITERAL */
} hash_entry;

typedef struct hashtable{
  long number_of_buckets;	/* number of buckets */
  long buckets_mask;		/* a mask for fast bitwise and'ing */
  long element_size;		/* sizeof the element to be stored 
				   (including th hash_entry_header) */
  long number_of_elements;	/* total number of elements hashed */
  long max_number_of_elements;	/* size of the contents buffer in elements */

  long *buckets;		/* array of longs, each an index into contents
				 * -1 is an empty entry. */
  hash_entry* contents;		/* pointer to hashtable memory */

} hashtable;


#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* __cplusplus */


/* number_of_buckets must be a power of 2, 
      if -1 then it defaults to DEFAULT_NUMBER_OF_BUCKETS.
   number_of_elements is the number of expected elements that will be hashed.
   element_size must be the sizeof the element to be put in the hashtable 
       (not including hash_entry_header).
   returns NULL if an error
 */
hashtable *make_hashtable _AP ((long number_of_buckets, 
				 long number_of_elements,
				 long element_size));

/* returns a pointer to the element stored or NULL if none. */
hash_entry *get_hash _AP ((char *key, hashtable *htable));

/* puts a copy of the element into the hashtable.
   Returns a pointer to the new element.

   This does not check to see that the key has not already been added. */
hash_entry *put_hash _AP ((char *key, hashtable *htable, hash_entry *element));

/* not implemented yet 
boolean rem_hash _AP ((char *key, hashtable *htable));
 */

/* removes contents without freeing memory.
   returns true if successful, false otherwise */
boolean clr_hashtable _AP ((hashtable *htable));

/* removes contents and free's memory for the hastable.   
   returns true if successful, false otherwise */
boolean free_hashtable _AP ((hashtable *htable));

long hashtable_count _AP ((hashtable *htable));

/* sorts the contents of elements of the hastable.
   this destroys the hashtable */
boolean sort_hashtable _AP ((hashtable *htable));

#ifdef FIELDS
boolean sort_hashtable_numeric _AP ((hashtable *htable));
#endif /* FIELDS */

void analyze_hashtable_distribution _AP ((hashtable * htable));
void print_hashtable _AP ((hashtable *htable));


#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* not HASH_H */
