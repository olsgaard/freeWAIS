/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* header for irtfiles.c */

#ifndef IRTFILES_H
#define IRTFILES_H

#include "cdialect.h"
#include "irfiles.h" /* for database struct */

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

#define kWordBreakNonAlnum	0
#define	kWordBreakSpace			1

#ifndef EXTERN_MAKE_COMPILER_HAPPY
#define EXTERN_MAKE_COMPILER_HAPPY extern
#endif /* EXTERN_MAKE_COMPILER_HAPPY */
EXTERN_MAKE_COMPILER_HAPPY int make_compiler_happy;   /* global word break func, dgg */
#undef  EXTERN_MAKE_COMPILER_HAPPY

typedef boolean (boolfunc)();
typedef void (voidfunc)();
typedef long (longfunc)();

char *make_joint_word _AP((char *word1, char *word2));

/* multitype extensions */
typedef struct _dataopsrec {	/* dgg, use this instead of a dozen parameters in several funcs */
		boolean (*separator_function)();
		void (*header_function)();
		long (*date_function)();
		void (*finish_header_function)(); 
		boolean (*wordDelimiter)();  	/* ? use global wordDelimiter ? */
		char *type;			/* TEXT, PICT, ... */
#ifdef SOUND
                char *indextype;                 /* SOUNDEX, PHONIX */
#endif
#ifdef FIELDS /* tung, 1/94 */
                char *prev_indextype;
#endif
		char *multitype;		/* TEXT, PICT, ... */
		boolean addseparatorwords; 	/* add words in line separating documents */
		boolean extraheaderweight; 	/* add header weight */
		int repeat_weight; 		/* 0 for max of 1 word hit per doc */
		int minwordlen;			/* 2= default, 1=symbols */
		char *delimiters;
		} dataopsrec, *dataopsptr;

#ifndef EXTERN_WORD_DELIMITER
#define EXTERN_WORD_DELIMITER extern
#endif /* EXTERN_WORD_DELIMITER */
EXTERN_WORD_DELIMITER boolean (*wordDelimiter)();   /* global word break func, dgg */
#undef EXTERN_WORD_DELIMITER


typedef long (wordfunc) _AP((char *word, long char_pos,
			     long line_pos, long weight,
			     long doc_id, time_t date,
			     boolean capitalized, database* db,
			     boolean word_position, boolean word_pairs));
#ifdef SOUND
long map_over_words
  _AP((char* line,long document_id,long weight,long file_position_before_line,
       long* line_length,boolean* newline_terminated,database* db,
       wordfunc wordfunction, boolean word_position, boolean word_pairs,
       int minwordlen, 
       char* type));	
#else
long map_over_words
  _AP((char* line,long document_id,long weight,long file_position_before_line,
       long* line_length,boolean* newline_terminated,database* db,
       wordfunc wordfunction, boolean word_position, boolean word_pairs,
       int minwordlen));
#endif

#ifdef FIELDS /* tung, 1/94 */
long field_index_text_file _AP((char* filename,
				dataopsrec* dataops,
				database* db,
				boolean check_for_text_file,
				boolean check_for_file_already_indexed,
				boolean word_position, boolean word_pairs,
				long field_id));
long index_text_file _AP((char* filename,
			  dataopsrec* dataops,
			  database* db,
			  boolean check_for_text_file,
			  boolean check_for_file_already_indexed,
			  boolean word_position, boolean word_pairs,
			  long field_id));
#else
long index_text_file _AP((char* filename,
			  dataopsrec* dataops,
			  database* db,
			  boolean check_for_text_file,
			  boolean check_for_file_already_indexed,
			  boolean word_position, boolean word_pairs));
				
#endif
/* void index_text_file _AP((char* filename,
			  boolean (*separator_function)(),
			  void (*header_function)(),
			  longfunc *date_function,
			  void (*put_together_header_function)(), 
			  char *type,
			  database* db,
			  boolean check_for_text_file,
			  boolean check_for_file_already_indexed,
			  boolean word_position, boolean word_pairs,
				int minwordlen));  */

boolean directoryp _AP ((char *file));

boolean filep _AP((char* file));

#ifdef FIELDS /* 5/94, tung */
void index_directory _AP ((char *file,
		 	   dataopsrec* dataops,			  
			   database *db,
			   boolean check_for_text_file,
			   boolean check_for_file_already_indexed,
			   boolean word_position, boolean word_pairs,
			   boolean index_next_field, long field_id));
#else
void index_directory _AP ((char *file,
		 	   dataopsrec* dataops,			  
			   database *db,
			   boolean check_for_text_file,
			   boolean check_for_file_already_indexed,
			   boolean word_position, boolean word_pairs));
#endif

/* void index_directory _AP ((char *file,
			   boolfunc *separator_function,
			   voidfunc *header_function,
			   longfunc *date_function,
			   voidfunc *finish_header_function, 
			   char *type,
			   database *db,
			   boolean check_for_text_file,
			   boolean check_for_file_already_indexed,
			   boolean word_position, boolean word_pairs,
				int minwordlen)); */

char *cleanHeadline _AP((char* headline));


#ifdef __cplusplus
	}
#endif /* def __cplusplus */
#endif /* ndef IRTFILES_H */

