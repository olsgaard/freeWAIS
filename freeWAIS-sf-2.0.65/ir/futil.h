/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
  
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef FUTIL_H
#define FUTIL_H

#include "cdialect.h"
#include "cutil.h"


#define MAX_FILE_NAME_LEN 255

#define FType long

/* the following defines should be in stdio.h, but the
   gnu C compiler doesn't define them for some reason
 */
#ifndef SEEK_SET 
#define SEEK_SET 0  /* ANSI added by brewster */
#define SEEK_CUR 1  /* ANSI added by brewster */
#define SEEK_END 2  /* ANSI added by brewster */
#endif

/* enhanced standard functions - don't call them directly, use the
   macros below */
FILE*	fs_fopen _AP((char* fileName,char* mode));
long	fs_fclose _AP((FILE* file));
long 	fs_fseek _AP((FILE* file,long offset,long wherefrom));
long 	fs_ftell _AP((FILE* file));
char    *fs_fzcat _AP((char *fileName));

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

/* macros for standard functions.  call these in your program.  */
#define s_fopen(name,mode)	fs_fopen((name),(mode))
#define s_fzcat(name)           fs_fzcat((name))
#define s_fclose(file)		{ fs_fclose((FILE*)file); file = NULL; }
#define s_fseek(file,offset,wherefrom) fs_fseek(file,offset,wherefrom)
#define s_ftell(file)	fs_ftell(file)

void grow_file _AP((FILE* file,long length));
long read_bytes _AP((long n_bytes,FILE *stream));
long write_bytes _AP((long value, long n_bytes, FILE* stream));
long read_bytes_from_memory _AP((long n_bytes, unsigned char* block));
time_t file_write_date _AP((char* filename)); /* os dependent */
char *truename _AP((char *filename, char *full_path));
long file_length _AP((FILE* stream));
char *pathname_name _AP((char *pathname));
char *pathname_directory _AP((char *pathname, char *destination));
char *current_user_name _AP((void));
boolean probe_file _AP((char *filename));
boolean probe_file_possibly_compressed _AP((char *filename));
boolean touch_file _AP((char *filename));
char *merge_pathnames _AP((char *pathname, char *directory));

#ifdef THINK_C
void setFileType _AP((char* fileName,FType type,FType creator));
#endif /* def THINK_C */

boolean read_string_from_file _AP((FILE* stream,char* array,
				   long array_length));

long count_lines _AP((FILE *stream));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* FUTIL_H */
