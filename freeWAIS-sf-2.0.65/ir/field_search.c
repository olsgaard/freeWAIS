/*                               -*- Mode: C -*- 
 * * field.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Huynh Quoc T. Tung
 * Created On      : Mon Feb 14 11:18:44 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Fri Dec 29 13:14:59 1995
 * Language        : C
 * Update Count    : 51
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: field_search.c,v $
 * Revision 2.0.1.2  1995/12/29 12:30:46  pfeifer
 * patch60: String search in global field did not work if other fields
 * patch60: occurred in the query.
 *
 * Revision 2.0.1.1  1995/11/09  14:05:29  pfeifer
 * patch46: indent.
 *
 */

#include "futil.h"
#include "irfiles.h"
#include "irtfiles.h"
#include "cutil.h"

#define MAX_LINE_LENGTH 1000

long number_of_operands = 1;

/* ------------------------------------------------------------- */

/* return true if it is field,
   return false if the length of field less than 2 
   and it is not field 
*/

boolean is_field_name(word)
     char* word;
{
  if(word[strlen(word) - 1] == FIELD_EQUAL) { /* word is field */
    word[strlen(word) - 1] = '\0';
    if(strlen(word) < 2)
      return(false);
    else return(true);
  }
  else return(false);
}
/* ------------------------------------------------------------- */

/* return the field_id of the field_name,
   else -1.
*/

long pick_up_field_id(field_name, db)
     char* field_name;
     database* db;
{
  long i;

  if ((field_name == NULL) || (field_name[0] == '\0')) 
    field_name = "text";
  for(i=0; i < db->number_of_fields; i++) 
    if(strcmp(field_name, db->fields[i].field_name) == 0)
      return(i);
  return(-1);
}
/* ------------------------------------------------------------- */

static boolean field_exist _AP((char* field_name, long number_of_elements,
                                fields_struct* fields));
static boolean field_exist(field_name, number_of_elements, fields)
     char* field_name;
     long number_of_elements;
     fields_struct* fields;
{
  while(number_of_elements > 0) {
    if(fields[number_of_elements-1].field_name != NULL)
      if(strcmp(field_name, 
                fields[number_of_elements-1].field_name))
        number_of_elements--;
      else return(true);
    else number_of_elements--;
  }
  return(false);
}
/* ------------------------------------------------------------- */

static void clear_field_array _AP((char** field_array, long number_of_fields));
static void clear_field_array(field_array, number_of_fields)
     char** field_array;
     long number_of_fields;
{
  long i;
  
  if(number_of_fields > 0) 
    for(i=0; i < number_of_fields; i++)
      s_free(field_array[i]);
}
/* ------------------------------------------------------------- */

/* 
  insert the field_names which the query contains,
  return number of fields.
 */

long insert_fields(field_array, number_of_fields, db)
     char** field_array;
     long number_of_fields;
     database* db;
{
  long len;
  long i;
  char* field_name;

  if(db->fields == NULL)
    db->fields = (field_db*)s_malloc((size_t)(sizeof(field_db) * (number_of_fields + 1)));

  db->number_of_fields = number_of_fields;

  for(i=0; i< number_of_fields; i++) {
    len = strlen(field_array[i]);
    if((field_array[i][len-1] == EQ) || 
       (field_array[i][len-1] == GREATER) ||
       (field_array[i][len-1] == LESS)) {
      field_array[i][len-1] = '\0';
      db->fields[i].numeric = true;
    }
    else db->fields[i].numeric = false;
    
    /*
    field_name = string_downcase(field_array[i]);
    */
    field_name = field_array[i];
    if(db->fields[i].field_name == NULL)
      db->fields[i].field_name =
        (char*)s_malloc((size_t)(sizeof(char) * (MAX_WORD_LENGTH + 1)));
    s_strncpy(db->fields[i].field_name, field_name, MAX_WORD_LENGTH);
    db->fields[i].field_id = i;
    db->fields[i].total_word_count = 0;
  }
  clear_field_array(field_array, number_of_fields);
  return(db->number_of_fields);
}

/* ------------------------------------------------------------- */

/* open only fields which the query contains */

boolean 
open_field_streams_for_search (field_name, global_dictionary_exists, db)
     char           *field_name;
     int             global_dictionary_exists;
     database       *db;
{
  long            i;
  boolean         is_field_exist = false;
  char            file[1001];

  if (global_dictionary_exists) {
    db->dictionary_stream = s_fopen (dictionary_filename (file, db), "rb");
    if (db->dictionary_stream == NULL) {
      strcpy (field_name, "global database");
      waislog (WLOG_HIGH, WLOG_ERROR, 
               "can't open the word hash file %s\n", file);
      return (false);
    }
    if (false == look_up_total_word_count (db)) {	/* side effects db */
      strcpy (field_name, "global database");
      return (false);
    }
    db->index_stream = s_fopen (index_filename (file, db), "rb");
    if (db->index_stream == NULL) {
      strcpy (field_name, "global database");
      waislog (WLOG_HIGH, WLOG_ERROR,
	       "2can't open the inverted index file %s\n",
	       file);
      return (false);
    }
  }
  if (db->field_dictionary_streams == NULL)
    db->field_dictionary_streams =
      (FILE **) s_malloc ((size_t) (sizeof (FILE *) * 
                                    (db->number_of_fields + 1)));
  if (db->field_index_streams == NULL) {
    db->field_index_streams =
      (FILE **) s_malloc ((size_t) (sizeof (FILE *) * 
                                    (db->number_of_fields + 1)));
    db->number_of_fieldstreams = db->number_of_fields+1;
  }
  for (i = 0; i < db->number_of_fields; i++) {
    if (strncmp (db->fields[i].field_name, FREE_TEXT_FIELD,
		 strlen (FREE_TEXT_FIELD))) {
      db->field_dictionary_streams[i] =
	s_fopen (field_dictionary_filename (file, db->fields[i].field_name, db),
		 "rb");
      if (db->field_dictionary_streams[i] == NULL) {
	strcpy (field_name, db->fields[i].field_name);
	waislog (WLOG_HIGH, WLOG_ERROR, 
                 "can't open the word hash file %s\n", file);
	return (false);
      } else {
	if (false == field_look_up_total_word_count 
            (db->fields[i].field_name, i,
             db)) {	/* side effects db */
	  strcpy (field_name, db->fields[i].field_name);
	  return (false);
	}
	is_field_exist = true;
	db->fields[i].index_file_number = 0;
      }
      if (db->field_dictionary_streams[i] != NULL) {
	db->field_index_streams[i] =
	  s_fopen (field_index_filename (file, db->fields[i].field_name, db),
		   "rb");
	if (db->field_index_streams[i] == NULL) {
	  strcpy (field_name, db->fields[i].field_name);
	  waislog (WLOG_HIGH, WLOG_ERROR,
		   "2can't open the inverted index file of field %s\n",
		   db->fields[i].field_name);
	  return (false);
	}
      }
    } else {
      /* make 'text' a real field up 12/95 */
      if (db->dictionary_stream == NULL) {
        db->dictionary_stream = 
          s_fopen(dictionary_filename(file, db), "rb");
        if (db->dictionary_stream == NULL){ 
          waislog(WLOG_HIGH,WLOG_ERROR,"can't open the word hash file %s\n",file); 
          disposeDatabase(db);
          return(NULL);
        }
      }
      if (db->index_stream == NULL) {
        db->index_stream = s_fopen (index_filename (file, db), "rb");
        if (db->index_stream == NULL){ 
          waislog(WLOG_HIGH,WLOG_ERROR,"can't open the inverted index file %s\n",
                  file); 
          disposeDatabase(db);
          return(NULL);
        }
      }
      db->field_dictionary_streams[i] = db->dictionary_stream;
      db->field_index_streams[i]      = db->index_stream;
      is_field_exist = true;
    }
  }
  return (is_field_exist);
}
/* ------------------------------------------------------------- */
