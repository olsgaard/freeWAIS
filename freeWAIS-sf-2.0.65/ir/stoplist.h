/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef STOPLIST_H
#define STOPLIST_H

#include "cdialect.h"
#include "cutil.h"

#ifdef AUTO_STOPWORDS /* tung, 2/95 */
#define STOP ".stop"
extern boolean use_both_stoplist;

void add_auto_stopword _AP((char* stopword));
void init_stop_list _AP((char* database_file));
#else
void init_stop_list _AP((void));
#endif
void stop_list_file _AP((char *filename));
void add_stop_word _AP((char *));
void add_stop_file _AP((char *));
char *next_stop_word _AP((void));

#endif /* ndef STOPLIST_H */
