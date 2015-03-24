/*                               -*- Mode: C -*- 
 * 
 * irfiles.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Fri Sep 15 09:54:01 1995
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Thu Nov  9 14:48:35 1995
 * Language        : C
 * Update Count    : 10
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 */

/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
 *
 * $Log: irfiles.h,v $
 * Revision 2.0.1.6  1995/11/09 14:06:58  pfeifer
 * patch46: Added counter for file buffers to database record.
 *
 * Revision 2.0.1.5  1995/10/10  12:39:05  pfeifer
 * patch31: Added parameter match_word to
 * patch31: look_up_partialword_in_dictionary() prototype.
 *
 * Revision 2.0.1.4  1995/10/06  17:37:46  pfeifer
 * patch24: Added DateDescription to store all information needed for
 * patch24: extracting dates.
 *
 * Revision 2.0.1.3  1995/09/19  19:11:14  pfeifer
 * patch13: Changed ISO to MYCTYPE. Broke 8-bit code!
 *
 * Revision 2.0.1.2  1995/09/15  09:43:39  pfeifer
 * patch7: Added header.  Fixed #ifdef in cpp macro call.
 *
 * Revision 2.0.1.1  1995/09/08  19:17:56  pfeifer
 * patch1: Removed superfluous second prototype for allocate_index_block.
 *
 * Revision 2.0  1995/09/08  07:59:47  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.14  1995/05/07  16:04:57  pfeifer
 * More fixes by Tom Snee. Literal serach now works again!
 *
 * Revision 1.13  1995/04/27  12:06:19  pfeifer
 * PROXIMITY patch by Tom Snee <tsnee@merengue.oit.unc.edu>
 *
 * Revision 1.12  1995/04/17  13:16:32  huynh1
 * Release 1.1.2
 *
 * Revision 1.11  1995/01/10  17:02:59  pfeifer
 * Added filed_description
 *
 * Revision 1.10  1994/07/13  07:52:36  huynh1
 * Uli
 *
 * Revision 1.9  1994/06/02  10:48:05  pfeifer
 * DICTIONARY_TOTAL_SIZE_WORD now depends on #define ISO
 *
 * Revision 1.8  1994/05/31  15:40:19  pfeifer
 * DICTIONARY_TOTAL_SIZE_WORD
 *
 * Revision 1.7  1994/05/20  12:57:29  pfeifer
 * beta
 *
 * Revision 1.6  1994/04/05  14:40:10  huynh1
 * stemming component added.
 *
 * Revision 1.5  1994/03/23  12:57:47  huynh1
 * openDatabase modified.
 * patchlevel 07.
 *
 * Revision 1.4  1994/03/10  17:34:37  huynh1
 * Patchlevel 05
 *
 * Revision 1.3  1994/03/08  20:42:28  huynh1
 * Patchlevel 04
 *
 * Revision 1.2  1994/02/14  10:31:12  huynh1
 * new struct for field concept added.
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.19  92/04/16  20:04:44  morris
 * small fix to dictionary_blockword_occurances, lenght read was
 * NEXT_INDEX_BLOCK_SIZE, now its NUMBR_OF_OCCURANCES_SIZE.
 * 
 * Revision 1.18  92/03/19  09:34:08  morris
 * fixed the dictionary header to accurately indicate the number of blocks
 * 
 * Revision 1.17  92/02/17  12:38:00  jonathan
 * Added defines for catalog.
 * 
 */

/* include file for irfiles.c */

#ifndef IRFILES_H
#define IRFILES_H

#include "cdialect.h"
#include "cutil.h"
#include "hash.h"
#include "ustubs.h" /* for time_t */
#include "synonym.h"

/* filename extensions for various components */
#define dictionary_ext			".dct"
#define filename_table_ext		".fn"
#define headline_table_ext		".hl"
#define document_table_ext		".doc"
#define index_ext			".inv"
#define source_ext 			".src"
#define catalog_ext 			".cat"
#define synonym_ext			".syn"
#ifdef BIO
#define delimiters_ext	".dlm"	    /* dgg */
#endif
#ifdef FIELDS /* tung, 12/93 */
#define field_ext "_field_"
#endif
/* these dictionary definitions are used in irhash,irverify, and irfiles */
#define DICTIONARY_HEADER_SIZE 4
#define DICTIONARY_BLOCK_SIZE 1000L  /* in entries, not bytes */
#define DICTIONARY_ENTRY_HASH_CODE_SIZE 2
/* #define DICTIONARY_ENTRY_COUNT_SIZE 3  moved to inverted file */
/* #define DICTIONARY_ENTRY_INDEX_BLOCK_SIZE 4 not used and too long a symbol*/
/* #define DICTIONARY_ELEMENT_SIZE 6 was 9 */
#define DICTIONARY_SIZE 524288L

/* the word that holds the total number of words in the whole dictionary */
#ifdef MYCTYPE
#define DICTIONARY_TOTAL_SIZE_WORD "\377\377\377"
#else /* not MYCTYPE */
#define DICTIONARY_TOTAL_SIZE_WORD "{}" 
#endif /* not MYCTYPE */

#define INDEX_HEADER_SIZE 4
#ifdef BIO

/* !! Bug in W8B5 -- Increasing this INDEX_BLOCK_SIZE_SIZE above 2 now fails !! */
/* it worked in W8B3 ... both 3 and 4 fail now */
#define INDEX_BLOCK_SIZE_SIZE 2  /* was 2, genbank wants 3, dgg */

#else
#define INDEX_BLOCK_SIZE_SIZE 4
#endif

#define NEXT_INDEX_BLOCK_SIZE 4
#define INDEX_BLOCK_FLAG_SIZE 1

/* dgg -- this is a bug 
#define INDEX_BLOCK_HEADER_SIZE 7
This == 7 only if the component SIZEs don't change
*/
#define INDEX_BLOCK_HEADER_SIZE (INDEX_BLOCK_SIZE_SIZE+NEXT_INDEX_BLOCK_SIZE+INDEX_BLOCK_FLAG_SIZE)


#define NUMBER_OF_OCCURANCES_SIZE 4
#define INDEX_BLOCK_NOT_FULL_FLAG 101            /* 'e' */
#define INDEX_BLOCK_FULL_FLAG 69                 /* 'E' */
#define INDEX_BLOCK_DICTIONARY_FLAG 123          /* '{' */

#define DOCUMENT_ID_SIZE 4
#define WORD_POSITION_SIZE 0
#define CHARACTER_POSITION_SIZE 3

#ifdef NEW_WEIGHT /* tung, 5/94 */
#define WEIGHT_SIZE 0
#define NEW_WEIGHT_SIZE (sizeof(float))

#ifdef LITERAL /* tung, 2/95 */
#define INDEX_ELEMENT_SIZE (DOCUMENT_ID_SIZE+NUMBER_OF_OCCURANCES_SIZE+NEW_WEIGHT_SIZE+CHARACTER_POSITION_SIZE)
#else
#define INDEX_ELEMENT_SIZE (DOCUMENT_ID_SIZE+CHARACTER_POSITION_SIZE+NEW_WEIGHT_SIZE)
#endif /* LITERAL */
#else

#define WEIGHT_SIZE 1
#ifdef LITERAL /* tung, 2/95 */
#define INDEX_ELEMENT_SIZE (DOCUMENT_ID_SIZE+NUMBER_OF_OCCURANCES_SIZE+WEIGHT_SIZE+CHARACTER_POSITION_SIZE)
#else
#define INDEX_ELEMENT_SIZE 8
#endif /* LITERAL */

#endif /* NEW_WEIGHT */

#define WORD_ID_SIZE 4 /* for posting arrays */

#ifdef BOOLEANS			/* dgg */
#define BOOLEAN_AND	"and"	/* may prefer "&", but need symbol fix */
#define BOOLEAN_NOT "not"	/* may prefer "!", but need symbol fix */
#define BOOLEAN_NOT_FLAG -91	/* stick in weight param as flag for search_word */
#endif

#ifdef PARTIALWORD		/* dgg */
#define PARTWORD_WILDCARD  '*'	
#define PARTIAL_FLAG -9999
#endif

#ifdef LITERAL			/* dgg */
#define LITERAL_KEY1	'"'	
#define LITERAL_KEY2	0x27    /* single quote ' dgg */
#define LITERAL_FLAG	-92	/* stick in weight param as flag for search_word */
#define MAX_PHRASE_LENGTH  200
#endif

#ifdef SOUND
#define SOUNDEX "soundex"
#define PHONIX  "phonix"
#endif

#ifdef FIELDS /* tung, 12/93 */
typedef struct field_db {
  char           *field_name;
  long            field_id;
  long            total_word_count;
  long            number_of_words;
  long            number_of_words_in_hashtable;
  long            index_file_number;
  boolean         numeric;
  boolean         stemming;
} field_db;

typedef struct ftable {
  long            begin_tag_pos;
  long            end_tag_pos;
  long            index_pos;
  char          **indextypes;
  int            *index_kind;
} ftable;

typedef struct _DateDescription {
  char           *sscanf_arg;
  char            order[3];
  boolean         month_is_string;
} DateDescription;

typedef struct fields_struct {
  long            field_id;
  char           *field_name;
  char           *field_description;
  boolean         numeric;
  boolean         is_date;
  boolean         stemming;
  int             numeric_len;
  long            n_index_pos;
  long            number_of_Ftable;
  ftable         *Ftable;
  DateDescription date_desc;
} fields_struct;

#define MAX_FIELD_LENGTH 220
#define FIELD_FLAG -99
#define NUMERIC_FLAG -999
#define FIELD_EQUAL '='
#define EQ '='
#define GREATER '>'
#define LESS '<'
#define TEXT "TEXT"
#define FREE_TEXT_FIELD "text"
#define NUMERIC_TAG 'n' 
#endif

#ifdef STEM_WORDS /* tung, 2/95 */
#define STEMMING_TAG 's'
#endif

typedef struct database {
	char*	database_file;
	FILE*	dictionary_stream;
	FILE*	filename_table_stream;
	FILE*	headline_table_stream;
	FILE*	document_table_stream;
	FILE*	index_stream;
#ifdef FIELDS /* tung, 12/93 */
        FILE**   field_index_streams;
        FILE**  field_dictionary_streams;
        long number_of_fields;
        long number_of_fieldstreams;
        long current_field_position;
	long current_field_id;
        field_db* fields;
        fields_struct** index_fields;
#endif
#ifdef STEM_WORDS
	boolean stemming;
#endif
#ifdef BIO
	FILE*	delimiters_stream;
#endif
        long	doc_table_allocated_entries;
	hashtable* the_word_memory_hashtable;
	long 	number_of_words_in_hashtable; /* for building.
						 checked on every add_word.
						 set at start of building,
						 and on every flush.*/
	long 	flush_after_n_words; /* set at the start of building used
					to compare with 
					number_of_words_in_hashtable. */
	long 	number_of_words; /* for building.  number of different words.
				    Set from the headers of .inv files
				    as they are merged. 
				    It is used to set the header when a .inv 
				    file is first created (not by merging).
				    */
	long	index_file_number; /* for building. */
	long    total_word_count; /* Total number of word occurances.
				     set during indexing, saved in 
				     dictionary under 'ALL' entry */
	void*   ext_database;
 t_Synonym* syn_Table;       /* synonym index lookup table */
        int     syn_Table_Size;     /* number of entries in synonym table */
} database;

typedef struct document_table_entry {
	long	filename_id;
	long	headline_id;
	long	source_id;	/* for signature system */
	long	start_character;
	long	end_character;
	long 	document_length; /* in characters */
	long	number_of_lines; /* in lines */
	time_t  date;            /* 0 if unknown */
} document_table_entry;

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

#ifdef FIELDS /* tung, 3/94 */
database* openDatabase _AP((char* name, boolean initialize, boolean for_search, boolean type_field));
#else
database* openDatabase _AP((char* name, boolean initialize, boolean for_search));
#endif
void		closeDatabase _AP((database* the_db));
void		disposeDatabase _AP((database* the_db));

void initialize_index_files _AP((database* db));

char *read_filename_table_entry _AP((long position, 
				  char* filename,
				  char* type, 
				  time_t* file_write_date,
				  database* db));

long write_filename_table_entry _AP((char* filename, char *type, database* db));
boolean filename_in_database _AP((char *filename, char *type,
				  time_t *write_file_date, database *db));
boolean filename_in_filename_file _AP ((char *filename, char*type,
				  time_t *file_write_date, 
				  char* filename_file));
char *read_headline_table_entry _AP((long position,database* db));
long write_headline_table_entry _AP((char* headline, database* db));

#ifdef BIO
char *read_delimiters _AP((database* db));
long write_delimiters _AP((char* delimiters, database* db));
#endif

boolean read_document_table_entry 
  _AP((document_table_entry* doc_entry,long number,database* db));

long write_document_table_entry
  _AP((document_table_entry* doc_table_entry, database* db));

boolean writeUserValToDocIDTable _AP((unsigned long userVal,long doc,
				      database* db));


long next_document_id _AP((database* db));


void close_dictionary_file _AP((database *db));

long add_word_to_dictionary
  _AP((char *word, long index_file_block_number, long number_of_occurances,
       database* db));

#ifdef FIELDS /* tung, 12/93 */
long field_add_word_to_dictionary
  _AP((char *word, long index_file_block_number, long number_of_occurances,
       long field_id, database* db));
#endif

#ifdef PARTIALWORD
#ifdef FIELDS
long look_up_partialword_in_dictionary _AP((
                                            char* field_name, 
                                            char *word, long *word_id, char **match_word, database* db));
#else
long look_up_partialword_in_dictionary _AP((
                                            char *word, long *word_id, database* db));
#endif
#endif

#ifdef FIELDS /* tung, 12/93 */
long field_look_up_word_in_dictionary _AP((char* field_name, char *word, long *word_id, database* db));
#endif

long look_up_word_in_dictionary _AP((char *word, long *word_id, database* db));

long init_dict_file_for_writing _AP((database *db));

#ifdef FIELDS /* tung, 12/93 */
long field_init_dict_file_for_writing _AP((long field_id, database *db));
#endif

void init_dict_file_detailed _AP((FILE* dictionary_stream,
				  long number_of_blocks));

#ifdef FIELDS /* tung, 12/93 */
void field_init_dict_file_detailed _AP((FILE* dictionary_stream,
                                        long number_of_blocks));
#endif
void record_num_blocks_in_dict _AP((FILE* dictionary_stream,
				    long number_of_words));

long finished_add_word_to_dictionary _AP((database *db));

#ifdef FIELDS /* tung, 12/93 */
long field_finished_add_word_to_dictionary _AP((long field_id, database *db));
#endif

boolean register_src_structure _AP((char *filename));
#ifdef FIELDS /* tung, 3/94 */
boolean write_src_structure _AP((char *filename, 
				 char *database_name, 
				 char *typename,
				 char **filenames, 
				 long number_of_filename,
				 boolean export_database,
				 long tcp_port,
                                 database* db));
#else
boolean write_src_structure _AP((char *filename, 
				 char *database_name, 
				 char *typename,
				 char **filenames, 
				 long number_of_filename,
				 boolean export_database,
				 long tcp_port));
#endif
boolean build_catalog _AP((database* db));

unsigned char *read_dictionary_block _AP((unsigned char* block,
					  long position,long length,
					  FILE* stream));
				     				 
void print_dictionary _AP((database* db));

#define DICTIONARY_ENTRY_SIZE (MAX_WORD_LENGTH + 1 + NEXT_INDEX_BLOCK_SIZE + NUMBER_OF_OCCURANCES_SIZE) 

#ifdef DICT_FUNC

char *dictionary_block_word _AP((long i,unsigned char* block));
long dictionary_block_position _AP((long i,unsigned char* block));
long dictionary_block_word_occurances _AP((long i,unsigned char* block));

#else /* DICT_FUNC */

#define dictionary_block_word(i,block) \
  ((char *)((block) + ((i) * DICTIONARY_ENTRY_SIZE)))

#define dictionary_block_position(i,block) \
  read_bytes_from_memory(NEXT_INDEX_BLOCK_SIZE, \
			 (block) + ((i) * DICTIONARY_ENTRY_SIZE) + \
			  MAX_WORD_LENGTH + 1)

#define dictionary_block_word_occurances(i,block) \
  read_bytes_from_memory(NUMBER_OF_OCCURANCES_SIZE, \
                         (block) + ((i) * DICTIONARY_ENTRY_SIZE) + \
                         MAX_WORD_LENGTH + 1 + NEXT_INDEX_BLOCK_SIZE)
#endif

void print_dictionary_block _AP((unsigned char* block,long size));

/* database functions */
char* dictionary_filename _AP((char* destination, database* db));
#ifdef FIELDS /* tung, 12/93 */
char* field_dictionary_filename 
  _AP((char* destination, char* field_name, database* db));
#endif
char* filename_table_filename _AP((char* destination, database* db));
char* headline_table_filename _AP((char* destination, database* db));
char* document_table_filename _AP((char* destination, database* db));
char* index_filename _AP((char* destination, database* db));
char* index_filename_with_version _AP((long version, char* destination, 
                                       database* db));
#ifdef FIELDS /* tung, 12/93 */
char* field_index_filename  _AP((char* destination,
                                 char* field_name, database* db));
char* field_index_filename_with_version _AP((long version, char* destination, 
                                             long field_id, database* db));
#endif

long fread_from_stream _AP((FILE* stream, unsigned char* buf,
			    long nbytes));

char* source_filename _AP((char* destination, database* db));

#ifdef BIO
char* delimiters_filename _AP((char* destination, database* db));
#endif

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* IRFILES_H */
