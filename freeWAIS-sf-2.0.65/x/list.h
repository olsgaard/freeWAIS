
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef _H_LIST
#define _H_LIST

typedef struct list {
  char *thisNode;
  struct list *nextNode;
} _List, *List;

#endif
