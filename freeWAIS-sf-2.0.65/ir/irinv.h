/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* include file for irfiles.c */

#ifndef IRINV_H
#define IRINV_H

#include "irfiles.h" /* for database */

#ifdef LITERAL /* tung, 2/95 */
long allocate_index_block _AP((long how_large, long docs_incl_term, FILE* stream));
#else
long allocate_index_block _AP((long how_large, FILE* stream));
#endif

/* tung, 2/95 */
void save_terms_of_doc _AP((database *db));
void add_terms_saved _AP((boolean is_field, long doc_id, database* db));

void flush_memory_hashtable_to_disk _AP((database* db, 
					 boolean completely, 
					 long not_flush_doc_id));

#ifdef FIELDS /* tung, 1/94 */
void field_flush_memory_hashtable_to_disk _AP((database* db, 
					       boolean completely,
					       long field_id,
					       long not_flush_doc_id));
#endif

long read_dictionary_index_block
  _AP((long index_block,long* last_index_block,long* index_block_size,
       long* number_of_occurances,char* word,FILE* stream));

long write_dictionary_index_block _AP((long number_of_occurances,
				       char* word,FILE* stream));


#endif /* ndef IRINV_H */

