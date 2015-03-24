/*                               -*- Mode: C -*- 
 * test.c -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Ulrich Pfeifer
 * Created On      : Wed Mar 23 11:29:46 1994
 * Last Modified By: Ulrich Pfeifer
 * Last Modified On: Thu Nov  2 14:52:05 1995
 * Update Count    : 59
 * Status          : Unknown, Use with caution!
 */

#ifdef ORIGINAL
#include <ctype.h>
extern unsigned char _lchars_[];
extern unsigned char _uchars_[];
#define THISCTYPEVAR _ctype_
#else
#define THISCTYPEVAR _fw_ctype_
#define _AP(ARGS) ()
#include "./ctype.h"
#endif
#include "../config.h"
#include <stdio.h>

main()
{
  unsigned char *t;

  int c;
  for (c='\0';c<=255;c++) {
    /*if ( THISCTYPEVAR[c+1]) */
      printf("%c %c %c %d %d %d %d %d %d %d %d %d %d %d\n",
             c,
             toupper(c),
             tolower(c),
             isalnum(c),
             isalpha(c),
             iscntrl(c),
             isdigit(c),
             isgraph(c),
             islower(c),
             isprint(c),
             ispunct(c),
             isspace(c),
             isupper(c),
             isascii(c));
  }

  printf("Testing lower case letters\n");
  for (t=_lchars_;*t;t++) {
    printf("_fw_ctype_[%d]=%d\t",*t, ((THISCTYPEVAR + 1)[(unsigned char)*t]));
#ifdef DEBUG
    printf("index(%s,%c)=%d\t", _lchars_, *t, 
           index(_lchars_,(unsigned char)(*t))-(char *)_lchars_);
#endif
    printf("islower(%c)=%d\t", *t, islower(*t));
    printf("toupper(%c)=%c\t", *t, toupper(*t));
    printf("isupper(%c)=%d\t", *t, isupper(*t));
#ifdef DEBUG
    printf("index(%s,%c)=%d\t", _uchars_, *t, 
           index(_uchars_,(unsigned char)(*t))-(char *)_uchars_);
#endif
    printf("tolower(%c)=%c\n", *t, tolower(*t));
  }
  putchar('\n');

  printf("Testing upper case letters\n");
  for (t=_uchars_;*t;t++) {
    printf("_fw_ctype_[%d]=%d\t",*t, ((THISCTYPEVAR + 1)[(unsigned char)*t]));
#ifdef DEBUG
    printf("index(%s,%c)=%d\t", _lchars_, *t, 
           index(_lchars_,(unsigned char)(*t))-(char *)_lchars_);
#endif
    printf("islower(%c)=%d\t", *t, islower(*t));
    printf("toupper(%c)=%c\t", *t, toupper(*t));
    printf("isupper(%c)=%d\t", *t, isupper(*t));
#ifdef DEBUG
    printf("index(%s,%c)=%d\t", _uchars_, *t, 
           index(_uchars_,(unsigned char)(*t))-(char *)_uchars_);
#endif
    printf("tolower(%c)=%c\n", *t, tolower(*t));
  }
  putchar('\n');
  printf("strcmp(\"AA\",\"BB\")=%d\n",strcmp("AA","BB"));
  printf("strcmp(\"{}\",\"\344hnlich\")=%d\n",strcmp("{}","\344hnlich"));
  printf("strcmp(\"\377\377\377\",\"\344hnlich\")=%d\n",
         strcmp("\377\377\377","\344hnlich"));
  exit(0);
}
