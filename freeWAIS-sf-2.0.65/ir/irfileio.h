/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Reading and writing to files
 * -brewster 5/10/90
 */

#ifndef IRCFILEIO_H
#define IRCFILEIO_H

#include "cdialect.h"
#include "futil.h"
#include "zprot.h"

#define MAX_SYMBOL_SIZE 255
#define END_OF_STRUCT_OR_LIST 6

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

long SkipObject _AP((FILE* file));

long ReadLong _AP((FILE* file,long* num));

long ReadDouble _AP((FILE* file,double* num));

long ReadSymbol _AP((char* string, FILE* file, long string_size));

long ReadString _AP((char* string, FILE* file, long string_size));

long CheckStartOfStruct _AP((char* name, FILE* file));

long ReadAny _AP((any* destination, FILE* file));

long ReadTM _AP((struct tm* time, FILE* file));

long Read8BitArray _AP((char* destination, FILE* file, long length));

long ReadEndOfListOrStruct _AP((FILE* file));

long ReadStartOfStruct _AP((char* name, FILE* file));

Boolean ReadStartOfList _AP((FILE* file));

Boolean ReadEndOfList _AP((FILE* file));

Boolean	readAbsoluteTime _AP((struct tm* time,FILE* file));

/* writing */

long WriteStartOfStruct _AP((char* name, FILE* file));

long WriteEndOfStruct _AP((FILE* file));

long WriteSymbol _AP((char* name, FILE* file));

long WriteString _AP((char* string, FILE* file));

long WriteNewline _AP((FILE* file));

long WriteLong _AP((long number, FILE* file));

long WriteDouble _AP((double number, FILE* file));

long WriteAny _AP((any* value, FILE* file));

long Write8BitArray _AP((long length, char* array, FILE* file));

long WriteTM _AP((struct tm* time, FILE* file));

long WriteStartOfList _AP((FILE* file));

long WriteEndOfList _AP((FILE* file));

Boolean	writeAbsoluteTime _AP((struct tm* time,FILE* file)); 


#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* def IRCFILEIO_H */
