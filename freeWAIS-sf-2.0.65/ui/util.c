/* 
  WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface tools for the WAIS software.
   Do with it as you please.

   jonathan@Think.COM
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* $Log: util.c,v $
 * Revision 2.0.1.1  1995/09/15 09:54:12  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:30  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/08/05  07:30:19  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:22  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 * Revision 1.13  92/03/17  14:32:21  jonathan
 * Generally cleaned up.
 * 
 * Revision 1.12  92/03/08  09:17:48  jonathan
 * Fixed strip_lf.
 * 
 * Revision 1.11  92/03/08  09:12:33  jonathan
 * Added new function, strip_lf.
 * 
 */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ui/RCS/util.c,v 2.0.1.1 1995/09/15 09:54:12 pfeifer Exp $";
#endif

#define _C_UTIL

#include "wais.h"
#include "globals.h"

int charlistlength(list)
char **list;
{
  int num;

  if(list) {
    for(num = 0; list[num] != NULL; num++);
    return num;
  }
  else 
    return 0;
}

int listlength(list)
List list;
{
  int num;
  List l;

  for(num = 0, l = list; l != NULL; num++, l = l->nextNode);

  return num;
}

#define BEFORE 1
#define DURING 2
#define QUOTE 5

/* ripped out of gmacs-ui.c */
int find_string_slot(source, key, value, value_size, delete_internal_quotes)
char *source, *key, *value;
long value_size;
Boolean delete_internal_quotes;
{
  char ch;
  short state = BEFORE;
  long position = 0;  /* position in value */
  char *pos =(char*)strstr(source, key); /* address into source */

  value[0] = '\0';		/* initialize to nothing */

  if(NULL == pos)
    return(1);

  for(pos = pos + strlen(key); pos < source + strlen(source); pos++){
    ch = *pos;
    if((state == BEFORE) && (ch == '\"'))
      state = DURING;
    else if ((state == DURING) && (ch == '\\')){
      state = QUOTE;	
      if(!delete_internal_quotes){
	value[position] = ch;
	position++;
	if(position >= value_size){
	  value[value_size - 1] = '\0';
	  return(-1);
	}
      }
    }
    else if ((state == DURING) && (ch == '"')){	
      value[position] = '\0';
      return(0);
    }
    else if ((state == QUOTE) || (state == DURING)){
      if(state ==  QUOTE)
	state = DURING;
      value[position] = ch;
      position++;
      if(position >= value_size){
	value[value_size - 1] = '\0';
	return(-1);
      }
    }
    /* otherwise we are still before the start of the value */
  }
  value[position] = '\0';
  return(-1); /* error because we are in the middle of the string */
}

void find_value(source, key, value, value_size)
char *source, *key, *value;
int value_size;
{
  char ch;
  long position = 0;  /* position in value */
  char *pos =(char*)strstr(source, key); /* address into source */

  value[0] = '\0';		/* initialize to nothing */

  if(NULL == pos)
    return;

  pos = pos + strlen(key);
  ch = *pos;
  /* skip leading quotes and spaces */
  while((ch == '\"') || (ch == ' ')) {
    pos++; ch = *pos;
  }
  for(position = 0; pos < source + strlen(source); pos++){
    if((ch = *pos) == ' ') {
      value[position] = '\0';
      return;
    }
    value[position] = ch;
    position++;
    if(position >= value_size){
      value[value_size - 1] = '\0';
      return;
    }
  }
  value[position] = '\0';
}
