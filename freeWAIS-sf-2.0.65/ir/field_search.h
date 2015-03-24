/* 
 * field_search.h -- 
 * SCCS Status     : %W%	%G%
 * Author          : Huynh Quoc T. Tung
 * Created On      : Fri Feb 18 10:21:52 1994
 * Last Modified By: Huynh Quoc T. Tung
 * Last Modified On: Mon May 16 14:26:49 1994
 * Update Count    : 5
 * Status          : Unknown, Use with caution!
 */
#ifndef IRFILES_H
#define IRFILES_H
#include "irfiles.h"
#endif

#ifndef FIELD_SEARCH_H
#define FIELD_SEARCH_H

boolean is_field_name _AP((char* word));
long pick_up_field_id _AP((char* field_name, database* db));
long insert_fields _AP((char** field_array, long number_of_fields, database* db));
boolean open_field_streams_for_search _AP((char* field_name, int global_dictionary_exists, database* db));
#endif
