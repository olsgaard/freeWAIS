
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#define s_malloc malloc
#include <stdio.h>
#include "irsparse.h"
/*
$Log: irsparse.c,v $
Revision 2.0.1.1  1995/09/15 09:45:47  pfeifer
patch7: Fixed CNIDR copyright line.

 * Revision 2.0  1995/09/08  08:00:20  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/03/08  20:44:21  pfeifer
 * Patchlevel 04
 *
 * Revision 1.2  1993/06/04  10:25:57  pfeifer
 * Pachtlevel BIBDB
 *
 * Revision 1.1  1993/02/16  20:40:24  freewais
 * Initial revision
 *
*/

#define NEW(type) (type*)(s_malloc(sizeof(type)))


sparse_array* make_sparse_array(size)
     int size;
{
  sparse_array* tmp;
  tmp = (sparse_array* )s_malloc(sizeof(sparse_array));
  tmp->table = (doclist**)s_malloc(sizeof(doclist*)*size);
  tmp->buckets = size;
  return tmp;
}

double* sparse_aref(array,index)
     sparse_array* array;
     long index;
{
  int hash;
  doclist** ptr;
  hash = index % array->buckets;
  ptr = &array->table[hash];
  while(*ptr != NULL) {

    if((*ptr)->item ==index) {
      return &(*ptr)->val;
    }

    if((*ptr)->item > index) {
      doclist* tmp;
      tmp = NEW(doclist);
      tmp->val = 0.0;
      tmp->item = index;
      tmp->next = *ptr;
      *ptr = tmp;
      return &tmp->item;
    }
    ptr = &(*ptr)->next;
  }
  *ptr = NEW(doclist);
  (*ptr)->item =index;
  (*ptr)->val = 0.0;
  (*ptr)->next = 0;
  return &(*ptr)->item;
}


main() {
  
  sparse_array *foo;
  int t;
  foo = make_sparse_array(1024);
  
  for(t=0;t<10000;t++) {
    long *x;
    
    x = sparse_aref(foo,t);
    *x = t;
  }

  printf("%ld is aref(2000)\n",*(sparse_aref(foo,2000)));
}
