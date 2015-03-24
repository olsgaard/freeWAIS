/*                               -*- Mode: C -*- 
 * weight.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Huynh Quoc T. Tung
 * Created On      : Tue May 17 11:45:07 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Thu Dec 28 17:21:27 1995
 * Language        : C
 * Update Count    : 76
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: weight.c,v $
 * Revision 2.0.1.2  1995/12/28 16:45:51  pfeifer
 * patch59: Changed return type of read_weight_from*() to double. Some
 * patch59: compilers like that more than float :-(.
 *
 * Revision 2.0.1.1  1995/10/10 12:41:02  pfeifer
 * patch31: GNU indent.
 *
 */

/*********************       INTRODUCTION        ************************
 * The documents would be presented by term vectors of the form
 *       D = (t_0,w_d0; t_1,w_d1; ..., t_t,w_dt)
 * where each t_k identifies a content term assigned to some sample 
 * document and w_dk represents the weight of term t_k in Document D
 * (or query Q). Thus, a typical query Q might be formulated as
 *       Q = (q_0,w_q0; q_1,w_q1; ...; q_t,w_qt)
 * where q_k once again reprents a term assigned to query Q.
 * The weights could be allowed to vary continuosly between 0 and 1, the
 * higher weight assignments near 1 being used for the most important terms,
 * whereas lower weights near 0 would characterize the less important terms.
 * Given the vector representation, a query-document similarity value may
 * be obtained by comparing the corresponding vectors, using for example
 * the conventional vector product formula
 *       similarity(Q,D) = sum(w_qk * w_dk), k=1 to t.
 * 
 * Three factors important for term_weighting:
 * 1) term frequency in individual document (recall)
 * 2) inverse document frequency (precision)
 * 3) document length (vector length)
 * 
 * Term frequency component using here:  new_wgt = 0.5 + 0.5 * tf / max_tf
 * augmented normalized term frequency (tf factor normalized by maximum tf
 * in the vector, and further normalized to lie between 0.5 and 1.0).
 *
 * Collection frequency component using here: 1.0
 * no change in weight; use original term frequency component.
 *
 * Normalization component using here: sqrt(sum(new_wgt^2)) = vector length.
 *
 * Thus, document term weight is: w_dk = new_wgt / vector length
 *
 * By query term weighting it is assumpted that tf is equal 1. So that
 * w_qk = 1.
 *
 ****************************************************************************/

/*********************   PROCEDURE DESCRIPTION   ************************
 * assign_term_weight_for_doc(max_tf, number_of_elements, bucket_ids_array, db)
 *    long *max_tf;
 *    long *number_of_elements;
 *    long *bucket_ids_array;
 *    database *db;
 *
 * computing weight and assigning it into the buf.
 *
 * write_weight_in_ptr(weight, ptr)
 *    float weight;
 *    char *ptr;
 *
 * assigning weight into the buf
 *
 * float read_weight_from_stream(new_weight_size, stream)
 *    long new_weight_size;
 *    FILE* stream;
 *
 * reading weight from stream by searching. return weight.
 *
 * void save_terms_for_doc(number_of_terms, db)
 *   long *number_of_term;
 *   database *db;
 *
 * save all terms of document before flushing into disk.
 *
 * void add_terms_saved(is_field, number_of_terms, doc_id, db)
 *   boolean is_field;
 *   long *number_of_terms;
 *   long doc_id;
 *   database *db;
 *
 * add all terms saved into hashtable.
 *
 ****************************************************************************/

#include "irfiles.h"
#include "cutil.h"
#ifdef NEW_WEIGHT
#include "futil.h"
#include "hash.h"
#include "irhash.h"
#include "weight.h"
#include <math.h>

long            max_term_frequency = 0;	    /* used in irhash.c */
long            bucket_ids_doc_array[DEFAULT_NUMBER_OF_BUCKETS + 1];	/* used in hash.c */

void 
write_weight_in_ptr (weight, ptr)
     float           weight;
     unsigned char  *ptr;
{
  float           tmp_weight[1];

  tmp_weight[0] = weight;
  memcpy ((unsigned char *) ptr, (unsigned char *) tmp_weight, NEW_WEIGHT_SIZE);
}

#ifdef LITERAL			/* tung, 2/95 */
void 
write_size_in_ptr (size, ptr)
     long            size;
     unsigned char  *ptr;
{
  long            tmp_size[1];

  tmp_size[0] = size;
  memcpy ((unsigned char *) ptr, (unsigned char *) tmp_size, NUMBER_OF_OCCURANCES_SIZE);
}

#endif

double
read_weight_from_memory (new_weight_size, block)
     long            new_weight_size;
     unsigned char  *block;
{
  float           tmp;
  unsigned char  *inc;
  int             i;

  inc = (unsigned char *) &tmp;
  for (i = 0; i < new_weight_size; i++) {
    *inc = *(block + i);
    inc++;
  }
  return (tmp);
}

double
read_weight_from_stream (new_weight_size, stream)
     long            new_weight_size;
     FILE           *stream;
{
  float           tmp;
  unsigned char  *inc;
  int             i;

  inc = (unsigned char *) &tmp;
  for (i = 0; i < new_weight_size; i++) {
    *inc = fgetc (stream);
    inc++;
  }
  return (tmp);
}

void 
assign_term_weight_for_doc (number_of_elements, db)
     long           *number_of_elements;
     database       *db;
{
  long            i;
  long            id;
  long            tf;
  float           new_wgt;
  float           doc_len = 0.0;
  hashtable      *htable = db->the_word_memory_hashtable;

  /* compute document length */
  for (i = 0; i < *number_of_elements; i++) {
    id = bucket_ids_doc_array[i];
    tf = (htable->contents)[id].occurances_in_doc;

    doc_len += (0.5 + (0.5 * tf / max_term_frequency)) *
      (0.5 + (0.5 * tf / max_term_frequency));

  }
  doc_len = sqrt (doc_len);
  for (i = 0; i < *number_of_elements; i++) {
    id = bucket_ids_doc_array[i];
    tf = (htable->contents)[id].occurances_in_doc;
    (htable->contents)[id].occurances_in_doc = 0;

    new_wgt = (0.5 + (0.5 * tf / max_term_frequency)) / doc_len;

    if ((htable->contents)[id].current_memory_ptr == NULL) {
      fprintf (stderr, "panic: assign_term_weight_for_doc current_memory_ptr == NULL\n");
    } else {
#ifdef LITERAL			/* tung, 2/95 */
      write_size_in_ptr ((htable->contents)[id].current_memory_ptr -
			 ((htable->contents)[id].memory_ptr +
			  ((htable->contents)[id].index_element_current_pos +
			   INDEX_ELEMENT_SIZE - CHARACTER_POSITION_SIZE)),
			 (htable->contents)[id].memory_ptr +
			 ((htable->contents)[id].index_element_current_pos +
			  DOCUMENT_ID_SIZE));
      write_weight_in_ptr (new_wgt,
			   (htable->contents)[id].memory_ptr +
			 ((htable->contents)[id].index_element_current_pos +
			  DOCUMENT_ID_SIZE + NUMBER_OF_OCCURANCES_SIZE));
#else
      write_weight_in_ptr (new_wgt,
			   (htable->contents)[id].current_memory_ptr -
			   NEW_WEIGHT_SIZE);
#endif
    }
  }
  max_term_frequency = 0;
  *number_of_elements = 0;
  memset (bucket_ids_doc_array, 0, DEFAULT_NUMBER_OF_BUCKETS * sizeof (long));
}

#endif /* NEW_WEIGHT */
