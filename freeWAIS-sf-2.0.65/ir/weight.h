/*                               -*- Mode: C -*- 
 * 
 * weight.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Huynh Quoc T. Tung
 * Created On      : Fri May 20 09:18:01 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Thu Dec 28 17:21:17 1995
 * Language        : C
 * Update Count    : 22
 * Status          : Unknown, Use with caution!
 * 
 * (C) Copyright 1995, Universität Dortmund, all rights reserved.
 * 
 * $$
 * $Log: weight.h,v $
 * Revision 2.0.1.2  1995/12/28 16:45:54  pfeifer
 * patch59: Changed return type of read_weight_from*() to double. Some
 * patch59: compilers like that more than float :-(.
 *
 * Revision 2.0.1.1  1995/09/15 09:50:37  pfeifer
 * patch7: Removed prototype for write_weight_in_ptr() which seems to
 * patch7: puzzle the OSF compiler.
 *
 */


#ifndef WEIGHT_H
#define WEIGHT_H

#include "irfiles.h"

void assign_term_weight_for_doc _AP ((long *number_of_elements, database * db));

#ifdef LITERAL
void write_size_in_ptr _AP ((long size, unsigned char *ptr));
#endif /* LITERAL */

double read_weight_from_memory _AP((long new_weight_size, unsigned char *block));
double read_weight_from_stream _AP((long new_weight_size, FILE * stream));

#endif /* not WEIGHT_H */

