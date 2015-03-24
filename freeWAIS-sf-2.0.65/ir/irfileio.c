/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* Change log:
 * $Log: irfileio.c,v $
 * Revision 2.0.1.1  1995/09/15 09:42:38  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  07:59:29  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.5  1994/09/07  13:29:22  pfeifer
 * ctype is now included from cdialect.h after inclusion of string.h.
 * Since ctype.h (the local version defines strcmp and friends, inclusion o
 * of string.h after that caused probems
 *
 * Revision 1.4  1994/07/15  17:11:03  pfeifer
 * negative numerics
 *
 * Revision 1.3  1994/05/20  12:56:05  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:06:19  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.12  92/04/12  17:30:21  jonathan
 * Fixed WriteString to quote backslash as well as double-quotes.  Thanks to
 * dxl0243@hertz.njit.edu (Dong Liu).
 * 
 * Revision 1.11  92/03/06  10:57:16  jonathan
 * removed extraneous ; after function WriteTM.
 * 
 * Revision 1.10  92/03/04  19:20:33  jonathan
 * Made SkipObject return EOF if at end of file.
 * 
 * Revision 1.9  92/02/16  21:08:37  jonathan
 * Changed a few more char ch to long ch (for return value of fgetc).
 * 
 * Revision 1.8  92/02/12  13:24:13  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
*/

/********************************************************
 *  Writing and reading structures to files.		*
 *							*
 *  These use the Lisp printer format with the		*
 *  lisp naming conventions.  You ask: "Why would	*
 *  you want to use that?".  Well, we need an		*
 *  easily readable data syntax that can handle		*
 *  a large number of different data types.		*
 *  Further, we need it to be tagged so that		*
 *  run time tagged languages can read it and 		*
 *  it is flexible.  We need one that supports		*
 *  optional fields so that the format can 		*
 *  grow backcompatibly.  And (the kicker),		*
 *  it must be read from many languages since		*
 *  user interfaces may be written in anything		*
 *  from smalltalk to hypercard.			*
 * 							*
 *  -brewster 5/10/90					*
 ********************************************************/

/*
#include <string.h>
*/
#include "irfileio.h"
#include "cutil.h"

#define INDENT_SPACES 2
#define MAX_INDENT 40
static long indent_level;  /* this is the number of indent levels */


/**********************/
/*  WRITING TO FILES  */
/**********************/


static void indent _AP((FILE* file));

static void indent(file)
FILE* file;
/* indent the right number of spaces.  make sure that indent_level is 
 * non-negative, and that it does not indent too much
 */
{
  long i;
  for(i = 0; i <= MINIMUM(MAX_INDENT, MAXIMUM(0L, indent_level * INDENT_SPACES)); i++){
    putc(' ', file);
  }
}

long WriteStartOfList(file)
FILE* file;
{
  indent_level++;
  return(fprintf(file, " ( "));
}

long WriteEndOfList(file)
FILE* file;
{
  indent_level--;
  return(fprintf(file, " ) "));
}

long WriteStartOfStruct(name,file)
char* name;
FILE* file;
{
  indent_level++;
  return(fprintf(file, " (:%s ", name));
}

long WriteEndOfStruct(file)
FILE* file;
{
  indent_level--;
  return(fprintf(file, " ) "));
}

long WriteSymbol(name,file)
char* name;
FILE* file;
{
  return(fprintf(file, " %s ", name));
}

long WriteNewline(file)
FILE* file;
{
  long return_value = fprintf(file, "\n");
  indent(file);
  return(return_value);
}

long WriteLong(number,file)
long number;
FILE* file;
{
  return(fprintf(file, " %ld ", number));  
}

long WriteDouble(number,file)
double number;
FILE* file;
{
  return(fprintf(file, " %f ", number));
}

long WriteString(string,file)
char* string;
FILE* file;
{
  long i;
  putc('\"', file);
  for(i = 0; i < strlen(string); i++){
    if(string[i] == '\\' || string[i] == '\"')
      putc('\\', file);		/* quote the string quotes going into the file */
    putc(string[i], file);
  }
  putc('\"', file);
  return(1);
}

long WriteAny(value,file)
any* value;
FILE* file;
{
  WriteStartOfStruct("any", file);
  WriteSymbol(":size", file); WriteLong(value->size, file);
  WriteSymbol(":bytes", file);
  Write8BitArray(value->size, value->bytes, file);
  return(WriteEndOfStruct(file));
}

long Write8BitArray(length,array,file)
long length;
char* array;
FILE* file;
{
  long i;
  fprintf(file, " #( ");
  for(i=0; i<length; i++){
    WriteLong((long)array[i], file);
  }
  return(fprintf(file, " ) "));
}

/* Writes a time object to a file */
long WriteTM(atime,file)
struct tm* atime;
FILE* file;
{
  WriteStartOfStruct("tm", file);
  WriteSymbol(":tm-sec", file); WriteLong(atime->tm_sec, file);
  WriteSymbol(":tm-min", file); WriteLong(atime->tm_min, file);
  WriteSymbol(":tm-hour", file); WriteLong(atime->tm_hour, file);
  WriteSymbol(":tm-mday", file); WriteLong(atime->tm_mday, file);
  WriteSymbol(":tm-mon", file); WriteLong(atime->tm_mon, file);
  WriteNewline(file);
  WriteSymbol(":tm-year", file); WriteLong(atime->tm_year, file);
  WriteSymbol(":tm-wday", file); WriteLong(atime->tm_wday, file);
  WriteNewline(file);
  WriteSymbol(":tm-yday", file); WriteLong(atime->tm_yday, file);
  WriteSymbol(":tm-isdst", file); WriteLong(atime->tm_isdst, file);
  WriteEndOfStruct(file);
  return(WriteNewline(file));
}

Boolean  
writeAbsoluteTime(atime,file)
struct tm* atime;
FILE* file;
{
  WriteStartOfStruct("absolute-time",file);
  WriteNewline(file);
  WriteSymbol(":year",file); WriteLong((long)atime->tm_year,file);
  WriteNewline(file);
  WriteSymbol(":month",file); WriteLong((long)atime->tm_mon,file);
  WriteNewline(file);
  WriteSymbol(":mday",file); WriteLong((long)atime->tm_mday,file);
  WriteNewline(file);
  WriteSymbol(":hour",file); WriteLong((long)atime->tm_hour,file);
  WriteNewline(file);
  WriteSymbol(":minute",file); WriteLong((long)atime->tm_min,file);
  WriteNewline(file);
  WriteSymbol(":second",file); WriteLong((long)atime->tm_sec,file);
  WriteNewline(file);
  return(WriteEndOfStruct(file));
}




/************************/
/*  READING FROM FILES  */
/************************/


/* these are states of the parser */
#define BEFORE 1
#define DURING 2
#define HASH 3
#define S 4
#define QUOTE 5

/* returns TRUE if it hits an '(' before hitting any non whitespace.
   It has an added hack to detect NIL: it looks to make sure that it is nil,
   and then ungetc's a \) on the stream so that the end-checkers will catch it.
   Quack. */
Boolean ReadStartOfList(file)
FILE* file;
{
  long ch;
  while(TRUE){
    ch = getc(file);
    if(ch == '(') 
      return(TRUE);
    if(!isspace(ch)){
      /* check for NIL */
      if(ch == 'N' || ch == 'n'){
	ch = getc(file);
	if(ch == 'I' || ch == 'i'){
	  ch = getc(file);
	  if(ch == 'L' || ch == 'l'){
	    ungetc(')', file);
	    return(TRUE);
	  }
	}
      }
      return(FALSE); /* not NIL */
    }
  }
}


/* returns TRUE if it hits an ')' before hitting any non whitespace*/
Boolean ReadEndOfList(file)
FILE* file;
{
  long ch;
  while(TRUE){
    ch = getc(file);
    if(ch == ')') 
      return(TRUE);
    if(!isspace(ch)) 
      return(FALSE);
  }
}

#define STRING_ESC '\\'

long 
SkipObject(file)
FILE* file;
/* read an object of unknown type out of the file.  We handle:
      strings
      longs, doubles, and symbols
      structs, lists, and arrays
   and anything composed of them (absolute time etc)
*/
{
  long ch;

  while (true)	
    { ch = getc(file);
      if (ch == EOF)
	return (EOF); /* we are done */
      else
	{ if (isspace(ch))
	    continue; /* skip this char */
	  else if (ch == '"') /* string */
	    { long escapeCount = 0;
	      while (true)
		{ ch = getc(file);
		  if (ch == EOF)
		    return (EOF);
		  else
		    { if (ch == STRING_ESC)
			{ escapeCount++;
			  escapeCount = escapeCount % 2;
			}
		      if (ch == '"' && escapeCount == 0)
			break; /* out of reading string */
                    }
		}
	      break; /* we are done */
            }
	  else if (isnumchar(ch) || /* number */
		   (ch == ':')) /* symbol */
	    { while (!isspace(ch)) /* just read till there is white space */
		{ ch = getc(file);
		  if (ch == EOF)
		    return(EOF);
		}
	      break; /* we are done */
	    }
	  else if ((ch == '#') || /* array */
		   (ch == '(')) /* struct or list */
	    { long parenCount = 1;
	      if (ch == '#')	
		ch = getc(file); /* read in the '(' so we can think of it
				    as a list */
	      while (parenCount > 0)
		{ ch = getc(file);
		  if (ch == EOF)
		    return(EOF);
		  else if (ch == '"')
		    { /* start of a string, it may contain parens, 
			 so we will have to skip it */
		      ungetc(ch,file);
		      SkipObject(file);
		    }
		  else if (ch == '(') /* entering a new structure */	
		    parenCount++;
		  else if (ch == ')') /* leaving a structure */
		    parenCount--;
		}
	      break; /* we are done */
	    }
	}
    }

  return(true);
}

long ReadLong(file,answer)
FILE* file;
long* answer;
/* reads a long int a file returns true if successful, false otherwise */
{
  long ch;
  long state = BEFORE;
  boolean isNegative = false;
  long count = 0;
  
  *answer = 0;
  
  while(TRUE){
    ch = getc(file);
    if (ch == EOF){
      break;			/* we are done */
    }
    else if (isdigit(ch)){
      if(state == BEFORE){
	state = DURING;
      }
      count++;
      if(count == 12){
	/* then we have an error in the file, 32 bit numbers can not be more
	   than 10 digits long */
	return(false);
      }
      *answer = *answer * 10 + (ch - '0');
    }
    else if (ch == '-') {
      if (isNegative)
	/* then we have an error since there should be only one - in a number */
	return(false);
      if (state == BEFORE) {
	/* we are ok since the - must come before any digits */
	isNegative = true;
	state = DURING;
      }
      else {
	ungetc(ch,file);
	break;			/* we are done */
      }
    }
    else if(ch == ')' && (state == DURING)){
      ungetc(ch, file);
      return(true);		/* we are done */
    }
    else if(!isspace(ch)){
      /* then we have an error since it should be a digit or a space */
      return(false);
    }
    /* we do not have an digit */
    else if(state == DURING){
      ungetc(ch, file);
      break;			/* we are done */
    }
    /* otherwise we are still before the start */
  }
  
  if (isNegative)
    *answer *= -1;
  return(true);
}

long ReadDouble(file,answer)
FILE* file;
double* answer;
{
  /* XXX this routine needs to deal with negative numbers! */
  long ch;
  long state = BEFORE;
  long count = 0;
  long decimal_count = 0;
  
  *answer = 0.0;
  
  while(TRUE){	
    ch = getc(file);
    if (ch == EOF){
      return(true);
    }
    else if (ch == '.'){
      decimal_count ++;
    }
    else if (isdigit(ch)){
      if(state == BEFORE){
	state = DURING;
      }
      count++;
      if(count == 12){
	/* then we have an error in the file, 32 bit numbers can not be more
	   than 10 digits long */
	return(false);
      }
      if (decimal_count == 0){
	*answer = *answer * 10 + (ch - '0');
      }
      else{			/* then we are in the fraction part */
	double fraction = (ch - '0');
	long internal_count;
	for(internal_count = 0; internal_count < decimal_count; 
	    internal_count++){
	  fraction = fraction / 10.0;
	}
	*answer = *answer + fraction;
	decimal_count++;
      }
    }
    else if(!isspace(ch)){
      /* then we have an error since it should be a digit or a space */
      return(false);
    }
    /* we do not have an digit */
    else if(state == DURING){
      ungetc(ch, file);
      return(true);		/* we are done */
    }
    /* otherwise we are still before the start */
  }
}

static Boolean issymbolchar _AP((long ch));

static 
Boolean issymbolchar(ch)
long ch;
/* reads a symbol from a file and put it in the string argument.
 * The string_size argument is used to make sure the string is not 
 * overflowed.
 */
{
  return(!( isspace(ch) || ch == ')' || ch == '(' || ch == EOF));
}

/* reads a symbol from a file */
long ReadSymbol(string,file,string_size)
char* string;
FILE* file;
long string_size;
{
  long ch;
  long state = BEFORE;
  long position = 0;
  
  while(TRUE){
    ch = getc(file);
    if((state == BEFORE) && (ch == ')'))
      return(END_OF_STRUCT_OR_LIST);
    if(issymbolchar((long)ch)){	/* we are in a symbol */
      if(state == BEFORE)
	state = DURING;
      string[position] = ch;
      position++;
      if(position >= string_size){
	string[string_size - 1] = '\0';
	return(FALSE);
      }
    }
    /* we do not have an symbol character. we are done */
    else if((state == DURING) || ch == EOF){
      if(ch != EOF) ungetc(ch, file);
      string[position] = '\0';
      return(TRUE);		/* we are done */
    }
    /* otherwise we are still before the start of the symbol */
  }
}

long ReadEndOfListOrStruct(file)
FILE* file;
{
  long ch;
  while(TRUE){
    ch = getc(file);
    if (EOF == ch) 
      return(FALSE);
    else if(')' == ch) 
      return(TRUE);
    else if(!isspace(ch)) 
      return(FALSE);
  }
}

/* reads a string from a file */
long ReadString(string,file,string_size)
char* string;
FILE* file;
long string_size;
{
  long ch;
  long state = BEFORE;
  long position = 0;
  string[0] = '\0';  /* initialize to nothing */
  
  while(TRUE){
    ch = getc(file);
    if((state == BEFORE) && (ch == '\"'))
      state = DURING;
    else if (EOF == ch){
      string[position] = '\0';
      return(FALSE);
    }
    else if ((state == BEFORE) && (ch == ')'))
      return(END_OF_STRUCT_OR_LIST);
    else if ((state == DURING) && (ch == '\\'))
      state = QUOTE; /* do nothing */
    else if ((state == DURING) && (ch == '"')){	
      string[position] = '\0';
      return(TRUE);
    }
    else if ((state == QUOTE) || (state == DURING)){
			if(state == QUOTE)
				state = DURING;
			string[position] = ch;
			position++;
			if(position >= string_size){
				string[string_size - 1] = '\0';
				return(FALSE);
			}
		}
		/* otherwise we are still before the start of the string */
	}
}

/* returns TRUE if it is the start of a struct
 * returns END_OF_STRUCT_OR_LIST if it is a ')'
 * returns FALSE if it is something unexpected
 */
long ReadStartOfStruct(name,file)
char* name;
FILE* file;
{
  long ch;
  long state = BEFORE;
	
  name[0] = '\0';
	
  while(TRUE){
    ch = getc(file);
    if((state == BEFORE) && (ch == '#'))
      state = HASH;
    if((state == BEFORE) && (ch == '('))
      state = DURING;
    else if((state == BEFORE) && (ch == ')'))
      return(END_OF_STRUCT_OR_LIST);
    else if((state == BEFORE) && !isspace(ch))
      return(FALSE);		/* we have a problem */
    else if(state == HASH){
      if (ch == 's')
	state = S;
      else{
	fprintf(stderr,"Expected an 's' but got an %c\n", ch);
	return(FALSE);
      }
    }
    else if(state == S){
      if (ch == '(')
	state = DURING;
      else{
	fprintf(stderr,"Expected an '(' but got an an %c\n",ch);
	return(FALSE);
      }
    }
    else if(state == DURING){
      return(ReadSymbol(name, file, MAX_SYMBOL_SIZE));
    }
  }
}

/* returns TRUE if it is the right start of a struct,
 * returns END_OF_STRUCT_OR_LIST if it is the end of a list,
 * returns FALSE if it is something weird
 */
long CheckStartOfStruct(name,file)
char* name;
FILE* file;
{
  char temp_string[MAX_SYMBOL_SIZE];
  long result = ReadStartOfStruct(temp_string, file);
  if(result == END_OF_STRUCT_OR_LIST)
    return(END_OF_STRUCT_OR_LIST);
  else if(result == FALSE)
    return(FALSE);
  else if(0 == strcmp(temp_string, name))
    return(TRUE);
  else 
    return(FALSE);
}

/* reads an any.  an any with no bytes allocated.  The right number of bytes
 * will be malloc'ed while reading
 */
long ReadAny(destination,file)
any* destination;
FILE* file;
{
  char temp_string[MAX_SYMBOL_SIZE];
	
  destination->size = 0; /* initialize so that if an error happens 
			    it does not blow up */
  if(FALSE == CheckStartOfStruct("any", file)){
    fprintf(stderr,"An 'any' structure was not read from the disk");
    return(FALSE);
  }
	
  while(TRUE){
    long check_result;
    check_result = ReadSymbol(temp_string, file, MAX_SYMBOL_SIZE);
    if(FALSE == check_result) 
      return(FALSE);
    if(END_OF_STRUCT_OR_LIST == check_result) 
      return(TRUE);
		
    if(0 == strcmp(temp_string, ":size")) {
      long	size;
      ReadLong(file,&size);
      destination->size = (unsigned long)size;
    }
    else if(0 == strcmp(temp_string, ":bytes")){
      long result;
      /* the size must have been read in by now */
      destination->bytes = (char*)s_malloc(destination->size);
      if(NULL == destination->bytes){
	fprintf(stderr,
		"Error on reading file. Malloc ran out of memory in an ANY");
	return(FALSE);
      }
      result = Read8BitArray(destination->bytes, file, destination->size);
      if(FALSE == result)
	return(FALSE);
    }
    else{
      fprintf(stderr,"Unknown keyword for ANY %s\n", temp_string);
      return(FALSE);
    }
  }
}

/* this does not need the length, but it will probably know it in all cases */
long Read8BitArray(destination,file,length)
char* destination;
FILE* file;
long length;
{
  /* arrays start with #( */
  long ch;
  long state = BEFORE;
  while(TRUE){
    ch = getc(file);
    if((state == BEFORE) && ((ch == '#') || (ch == '('))) {
      if (ch == '(') state = DURING;
      else state = HASH;
    }
    else if((state == BEFORE) && !isspace(ch)){
      fprintf(stderr,"error in reading array.  Expected # and got %c", ch);
      return(FALSE);
    }
    else if(state == HASH){
      if (ch == '(')
	state = DURING;
      else{
	fprintf(stderr,"Expected an '(' but got an %c\n", ch);
	return(FALSE);
      }
    }
    else if(state == DURING){
      long i;
      ungetc(ch, file);
      for(i = 0; i < length; i++){
	long value;
	if(ReadLong(file,&value) == false){ /* then it error'ed */
	  fprintf(stderr,"Error in reading a number from the file.");
	  return(FALSE);
	}
	if(value > 255){	/* then we have read a non-char */
	  fprintf(stderr,"Error in reading file.  Expected a byte in an ANY, but got %ld", value);
	  return(FALSE);
	}
	destination[i] = (char)value;
      }
      if(FALSE == ReadEndOfListOrStruct(file)){
	fprintf(stderr,"array was wrong length");
	return(FALSE);
      }
      return(TRUE);
    }
  }
}
			

Boolean
readAbsoluteTime(atime,file)
struct tm* atime;
FILE* file;
{
  if (CheckStartOfStruct("absolute-time",file) == FALSE)
    return(false);
		  
  while (true)
    { long result;
      long val;
      char temp_string[MAX_SYMBOL_SIZE + 1];
     
      result = ReadSymbol(temp_string,file,MAX_SYMBOL_SIZE);
     
      if (result == END_OF_STRUCT_OR_LIST)
	break;
      else if (result == false)
	return(false);
	   	       
      if (strcmp(temp_string,":second") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_sec = val;
	}

      else if (strcmp(temp_string,":minute") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_min = val;
	}

      else if (strcmp(temp_string,":hour") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_hour = val;
	}

      else if (strcmp(temp_string,":mday") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_mday = val;
	}

      else if (strcmp(temp_string,":month") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_mon = val;
	}

      else if (strcmp(temp_string,":year") == 0)
	{ if (ReadLong(file,&val) == false)
	    return(false);
	  atime->tm_year = val;
	}
      
      else
	SkipObject(file);

    }

  return(true);
}
