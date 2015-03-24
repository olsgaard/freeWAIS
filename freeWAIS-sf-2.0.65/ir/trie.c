/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/trie.c,v 2.0.1.1 1995/09/15 09:48:53 pfeifer Exp $";
#endif

/* Change log:
 * $Log: trie.c,v $
 * Revision 2.0.1.1  1995/09/15 09:48:53  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:02:10  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/05/20  12:57:46  pfeifer
 * beta
 *
 * Revision 1.3  1994/03/23  15:30:58  pfeifer
 * fixed iso code
 *
 * Revision 1.2  1994/03/08  21:06:44  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.2  92/02/12  13:52:49  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
*/

/*
  trie.c: This module provides an associative lookup table, based on
  tries [see Knuth,D. Art of Computer Programming, Vol 3]

  Author: Simon E Spero (ses@ccgr.technion.ac.il)
  This file is in the public domain.

  public functions:
  
  encode: encodes a string for searching

  make_trie_allocator: contructs a trie allocator
  dispose_trie_allocator: dispose of a trie

  new_trie(string): 
    creates a new trie containing only the entry `string'
    
  trie_lookup(word,dictionary,allocator).
   This function looks up word in the dictionary, and, if found,
   returns a pointer to it's datum. If the word is not found,
   and allocator != NULL, the word will be added to the dictionary.
   If an error occurs, the function returns NULL
*/
  

/* #include <stdio.h> */
#include "cutil.h"
#include <ctype.h>
#include "trie.h"

/*
 * Special purpose allocators for resources that are freed only in bulk
 */

/*
 * make a new allocator 
 */
allocator* make_allocator(item_size,free_func) 
int item_size;
void (*free_func)();
{
  allocator* tmp;
  if (!(tmp = (allocator *)s_malloc(sizeof(allocator)))) {
    return 0L;
  }

  tmp->size=item_size;
  tmp->dispose = free_func;
  return tmp;
}

/*
 * dispose of an allocator
 */
void fast_free(alloc)
allocator* alloc;
{
  
  char *block;
  int i,j;
  int limit;
  for(i=0;i<alloc->blocks_allocated;i++) {
    if(alloc->dispose) {
      block=alloc->tofree[i];
      limit = (i+1 == alloc->blocks_allocated ? CHUNK_SIZE - alloc->items_left : CHUNK_SIZE);
      for(j=0;j<limit;j++) {
	alloc->dispose(block);
	block += alloc->size;
      }
    }
    free(alloc->tofree[i]);
  }
}

/*
 * allocate an item
 */

char* fast_alloc(alloc)
allocator* alloc;
{
  
  char* tmp;

  if (!alloc->items_left) {
    if (alloc->blocks_allocated <MAX_BLOCKS &&
	(tmp = (char *)s_malloc(alloc->size*CHUNK_SIZE))) {
      alloc->tofree[alloc->blocks_allocated++] = tmp;
      alloc->next_location = tmp +alloc->size;
      alloc->items_left = CHUNK_SIZE-1;
      return tmp;
    } else {
      return 0L;
    }
  } else {
    tmp = alloc->next_location;
    alloc->next_location += alloc->size;
    alloc->items_left--;
    return tmp;
  }
}

/*
 * function to free non fast_alloced stuff from a trie.
 *   should really be a lambda expression, but....
 */

void  free_trie(dict)
trie* dict;
{
  free(dict->string);
}

/*
 * make an allocator for tries
 */

trie_allocator* make_trie_allocator()
{
  trie_allocator* tmp;
  if(!(tmp = (trie_allocator*)s_malloc(sizeof(trie_allocator)))) {
    return 0L;
  }
  if (!(tmp->tries = make_allocator(sizeof(trie),free_trie))) {
    free(tmp);
    return 0L;
  }
  if(!(tmp->trie_tables = make_allocator(sizeof(trie_table),0L))) {
    free(tmp->tries);
    free(tmp);
    return 0L;
  }
  return tmp;
}

/*
 * dispose of a trie
 */

void dispose_trie_allocator(alloc)
trie_allocator* alloc;
{
  fast_free(alloc->tries);
  fast_free(alloc->trie_tables);
  free(alloc);
}

/*
 * make a trie with str as it's only entry
 */

trie* new_trie(str,alloc)
char* str;
trie_allocator* alloc;
{
  trie* tmp;

  tmp=(trie*)fast_alloc(alloc->tries);
  tmp->string=s_strdup(str);
  tmp->table=0L;
  tmp->datum=0L;
  return tmp;
}

trie ** new_trie_table(alloc)
allocator* alloc; {

  trie** tmp;

  tmp=(trie **)fast_alloc(alloc);
  if(!tmp) {
    return 0L;
  }
  return tmp;
}

/*
  After all those allocators, finally a useful function! 
  */

void **trie_lookup(key,dict,alloc)
     char* key;
     trie* dict;
     trie_allocator* alloc;
{

  char *c,*d;
  trie *t,*t2;

  c = key;
  d = dict->string;

  while(*c && *d && *c == *d) {
    c++; d++;
  }

  if (!*c ) { 
    if(!*d) {
      /* we found the answer*/
      return &(dict->datum);
    }
    /* key was a prefix*/
    if (!alloc) {
      return 0L;
    }
    /* split this node */
    t = new_trie(d+1,alloc);
    t->table = dict->table;
    t->datum = dict->datum;
    dict->table= (trie **)new_trie_table(alloc->trie_tables);
    dict->table[*d]=t;
    dict->datum=0L;
    dict->string=s_strdup(key);
    return &(dict->datum);
  }

  if(*d && *c != *d) { /* mismatch */
    if (!alloc) {
      return 0L;
    }
    t  = new_trie(d+1,alloc);
    t2 = new_trie(c+1,alloc);
    t->datum=dict->datum;
    t->table=dict->table;
    dict->table= (trie **)new_trie_table(alloc->trie_tables);
    dict->table[*c] = t2;
    dict->table[*d] = t;
    dict->datum =0L;
    *d='\0';
    return &(t2->datum);
  }
  
  /*
    Follow the pointers in table, if there are any
    */
  if (!dict->table) {
    if (!alloc) {
      return 0L;
    }
    dict->table=(trie **)new_trie_table(alloc->trie_tables);
  }
  
  if(dict->table[*c]) {
    return trie_lookup(c+1,dict->table[*c],alloc);
  } else {
    if (!alloc) {
      return 0L;
    }
    dict->table[*c] = new_trie(c+1,alloc);
    return &(dict->table[*c]->datum);
  }

}

/*
  WARNING: IF CHECK_ALNUM IS UNDEFINED, MAKE SURE YOU PASS IN AN ALNUM,
  OR ELSE
*/

int encode( s)
unsigned char* s; {
  register unsigned char * e;
  unsigned char t;
  for(e=s;*e;e++) {
#ifdef CHECK_ALNUM
    if (!isalnum(*e)) {
      return 0;
    }
#endif
    if (isdigit(*e)) {
      *e = *e -'0' + 27;
    } else {
      *e = (*e & 31);
    }
  }
  return 1;
}


void decode(s) 
unsigned char* s;
{
  while(*s) {
    if (*s < 27) {
      *s += ('a'-1);
    } else {
      *s += ('0'-27);
    }
    *s++;
  }
}
