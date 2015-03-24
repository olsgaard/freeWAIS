
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef _sigsrch_h
#define _sigsrch_h

extern long sig_close_database _AP(( database *));
extern long sig_open_database _AP(( database *, long, long, long, long, long, long));
extern long sig_exit _AP(( void));
extern long sig_init _AP(( char *, long, long, long, long));
extern long sig_init_add_word _AP(( database *, long, long));
extern long sig_finished_add_word _AP(( database *));
extern long sig_add_word _AP(( char *, long, long, long, long, long, long));
extern long sig_delete_doc_id _AP(( long, long));
extern long sig_init_search_word _AP(( database *));
extern long sig_search_word _AP(( char *, long, long, long, long, long,
                    long, long));
extern long sig_finished_search_word _AP(( database *));
extern long sig_best_hit _AP(( long *, long *, long *, long *));
extern long sig_finished_best_hit _AP(( void));

#endif
