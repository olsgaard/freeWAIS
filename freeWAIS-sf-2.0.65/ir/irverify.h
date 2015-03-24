
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef irverify_H
#define irverify_H

#include "irfiles.h"

#ifndef docID
#define docID 		unsigned long
#define postingWeight 	unsigned char
#endif

typedef struct serialPostingFile
{ 
  FILE* stream;
  long length;
  long current_index_block;
} serialPostingFile;

typedef struct postingsForATerm
{ 
  char		word[MAX_WORD_LENGTH + 1];
  unsigned long entries;
  docID*	docs;
  postingWeight* weights;
} postingsForATerm;

serialPostingFile* initSerialPostingFile _AP((char* filename));
void disposeSerialPostingFile _AP((serialPostingFile* pf));
void  printPostingsForATerm _AP((postingsForATerm* pfat));
postingsForATerm* getPostingsAt _AP((serialPostingFile* spf,long position));
postingsForATerm* getPostingsForNextTerm _AP((serialPostingFile* spf));
void disposePostingsForATerm _AP((postingsForATerm* pfat));
void removePostings _AP((postingsForATerm* pfat,long start,long run));
void printIndex _AP((database* db));
void printIndexUsingDictionary _AP((database* db));

#endif /* ndef irverify_H */
