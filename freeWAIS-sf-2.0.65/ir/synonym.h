
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef __SYNONYM_H
#  define _SYNONYM_H
#include "cdialect.h"
struct s_Synonym {
  char *root;
  char *key;
};

typedef struct s_Synonym t_Synonym;

#define synptr( elem ) syn_Table[*syn_Table_Size-1].elem
#define SYN_FILE_LINE_LENGTH 2048

#ifdef CACHE_SYN
/* use shared memory to keep around synonym tables */
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_SYN_CACHE 16

/* key to master shmem area */
extern int cacheSynId;

struct s_cacheSyn {
  int id;
  char synfile [256];
  int table_size;
};
typedef struct s_cacheSyn t_cacheSyn;

#endif /* CACHE_SYN */

/* prototypes */

int syn_compare _AP((void*,void* ));
char* lookup_Synonym _AP(( char*,t_Synonym*,int ));
void syn_ReadFile _AP(( char*,t_Synonym**,int* ));
void syn_Free _AP(( t_Synonym*,int* ));
#endif
