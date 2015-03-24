/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

 
#ifndef docid_h
#define docid_h

#include "cdialect.h"
#include "zprot.h"

#define COPY_WITHOUT_RESTRICTION        0
#define ALL_RIGHTS_RESERVED             1
#define DISTRIBUTION_RESTRICTIONS_APPLY 2

/*---------------------------------------------------------------------------*/

typedef struct DocID{
   any* originalServer;
   any* originalDatabase;
   any* originalLocalID;
   any* distributorServer;
   any* distributorDatabase;
   any* distributorLocalID;
   long copyrightDisposition;
} DocID;

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

DocID* makeDocID _AP((void));

DocID* copyDocID _AP((DocID* doc));

void freeDocID _AP((DocID* doc));

any* GetServer _AP((DocID* doc));

DocID* docIDFromAny _AP((any* rawDocID));

any* anyFromDocID _AP((DocID* docID));

any* GetDatabase _AP((DocID* doc));

any* GetLocalID _AP((DocID* doc));

long GetCopyrightDisposition _AP((DocID* doc));

long ReadDocID _AP((DocID* doc, FILE* file));

long WriteDocID _AP((DocID* doc, FILE* file));

Boolean cmpDocIDs _AP((DocID* d1,DocID* d2));

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* ifndef docid_h */

