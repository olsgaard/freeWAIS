/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/*
 * $Log: source.h,v $
 * Revision 2.0.1.1  1995/09/15 09:53:57  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:21  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:18:33  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:38  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.8  92/04/02  14:23:06  jonathan
 * Added more prototypes.
 * 
 * Revision 1.7  92/03/17  14:31:23  jonathan
 * Merged SourceID.h in, prototyped everything.
 * 
 * Revision 1.6  92/03/06  14:51:04  jonathan
 * New and Improved source loading!
 * 
 * Revision 1.5  92/03/01  13:55:10  jonathan
 * fixed boolean to Boolean as X didn't know about it.
 * 
 * Revision 1.4  92/03/01  13:35:44  jonathan
 * Added functions for X interface.
 * 
 */

#ifndef _H_SOURCE
#define _H_SOURCE

#include <cdialect.h>

/* (jp) */
#ifndef EXTERN_SOURCE_ITEMS
#define EXTERN_SOURCE_ITEMS extern
#endif
EXTERN_SOURCE_ITEMS char **Source_items;
#undef EXTERN_SOURCE_ITEMS

#ifndef EXTERN_SOURCE_PATH
#define EXTERN_SOURCE_PATH extern
#endif
EXTERN_SOURCE_PATH char *sourcepath;
#undef EXTERN_SOURCE_PATH

typedef struct SourceID {
  char *filename;
} _SourceID, *SourceID;

typedef struct source{
  char *name;
  char *directory;
  char server[STRINGSIZE];
  char service[STRINGSIZE];
  char database[STRINGSIZE];
  char cost[STRINGSIZE];
  char units[STRINGSIZE];
  char *description;
  FILE *connection;
  long buffer_length;
  Boolean initp;
  char *maintainer;
} _Source, *Source;

typedef struct sourcelist {
  SourceID thisSource;
  struct sourcelist *nextSource;
} _SourceList, *SourceList;

typedef struct slist {
  Source thisSource;
  struct slist *nextSource;
} _SList, *SList;

/* functions */

void freeSourceID _AP((SourceID sid));
SourceID copysourceID _AP((SourceID sid));
char** buildSourceItemList _AP((SourceList sourcelist));
char** buildSItemList _AP((SList sourcelist));
short ReadSourceID _AP((FILE* file, SourceID sid));
SourceList ReadListOfSources _AP((FILE* fp));
Boolean ReadSource _AP((Source source, FILE* file));
Boolean ReadSourceFile _AP((Source asource, char* filename, char* directory));
Source loadSource _AP((char* name, char *path));
void set_connection _AP((Source source));
Boolean newSourcep _AP((char* name));
Boolean is_source _AP((char* name, Boolean test));
void SortSourceNames _AP((int n));
void GetSourceNames _AP((char* directory));
void ReadSourceDirectory _AP((char* directory, Boolean test));
void WriteSource _AP((char* directory, Source source, Boolean overwrite));
SourceList makeSourceList _AP((SourceID source, SourceList rest));
SList makeSList _AP((Source source, SList rest));
void FreeSource _AP((Source source));
void FreeSources _AP((SList sources));
Source findsource _AP((char* name, char *path));
Source findSource _AP((int n));
void format_source_cost _AP((char* str, Source source));
void freeSource _AP((SourceID sourceID));
void freeSourceList _AP((SourceList slist));
Boolean init_for_source _AP((Source source, char* request,
			     long length, char* response));
#endif
