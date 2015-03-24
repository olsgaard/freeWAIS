/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/zutil.c,v 2.0.1.2 1995/12/28 16:47:10 pfeifer Exp $";
#endif

/* Change log:
 * $Log: zutil.c,v $
 * Revision 2.0.1.2  1995/12/28 16:47:10  pfeifer
 * patch59: Init response now tells about the freeWAIS-sf Version.  There
 * patch59: is currently no other way to determine what server you are
 * patch59: connected to!
 *
 * Revision 2.0.1.1  1995/09/15  09:51:17  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:03:09  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.5  1994/08/16  11:48:08  pfeifer
 * adden register definition in readCompressedInteger, purify keeps
 * unhappy :(
 *
 * Revision 1.4  1994/08/12  17:42:59  pfeifer
 * Speeded up, but purify still complains:
 * ****  Purify'd ./waisserver  (pid 8145, forked from ./waisserver pid 8140)  ****
 * Purify (umr): uninitialized memory read:
 *   * This is occurring while in:
 * 	readCompressedInteger [line 241, zutil.c, pc=0xb9384]
 * 	readTag      [line 319, zutil.c, pc=0xb97a8]
 * 	readAny      [line 432, zutil.c, pc=0xb9e14]
 * 	readString   [line 533, zutil.c, pc=0xba44c]
 * 	readInitInfo [line 118, wprot.c, pc=0x776fc]
 * 	readInitAPDU [line 234, zprot.c, pc=0x8c69c]
 *   * Reading 1 byte from 0xeffe5bb7 on the stack
 *     This is local variable "byte" in function readCompressedInteger.
 *
 * ****  Purify'd ./waisserver  (pid 8145, forked from ./waisserver pid 8140)  ****
 * Purify (umr): uninitialized memory read:
 *   * This is occurring while in:
 * 	readCompressedInteger [line 242, zutil.c, pc=0xb93b4]
 * 	readTag      [line 319, zutil.c, pc=0xb97a8]
 * 	readAny      [line 432, zutil.c, pc=0xb9e14]
 * 	readString   [line 533, zutil.c, pc=0xba44c]
 * 	readInitInfo [line 118, wprot.c, pc=0x776fc]
 * 	readInitAPDU [line 234, zprot.c, pc=0x8c69c]
 *   * Reading 1 byte from 0xeffe5bb7 on the stack
 *     This is local variable "byte" in function readCompressedInteger.
 *
 * ****  Purify'd ./waisserver  (pid 8145, forked from ./waisserver pid 8140)  ****
 * Purify (umr): uninitialized memory read:
 *   * This is occurring while in:
 * 	readCompressedInteger [line 241, zutil.c, pc=0xb9384]
 * 	readAny      [line 433, zutil.c, pc=0xb9e38]
 * 	readString   [line 533, zutil.c, pc=0xba44c]
 * 	readInitInfo [line 118, wprot.c, pc=0x776fc]
 * 	readInitAPDU [line 234, zprot.c, pc=0x8c69c]
 * 	handleInit   [line 359, ir.c, pc=0x19d60]
 *   * Reading 1 byte from 0xeffe5c27 on the stack
 *     This is local variable "byte" in function readCompressedInteger.
 *
 * ****  Purify'd ./waisserver  (pid 8145, forked from ./waisserver pid 8140)  ****
 * Purify (umr): uninitialized memory read:
 *   * This is occurring while in:
 * 	readCompressedInteger [line 242, zutil.c, pc=0xb93b4]
 * 	readAny      [line 433, zutil.c, pc=0xb9e38]
 * 	readString   [line 533, zutil.c, pc=0xba44c]
 * 	readInitInfo [line 118, wprot.c, pc=0x776fc]
 * 	readInitAPDU [line 234, zprot.c, pc=0x8c69c]
 * 	handleInit   [line 359, ir.c, pc=0x19d60]
 *   * Reading 1 byte from 0xeffe5c27 on the stack
 *     This is local variable "byte" in function readCompressedInteger.
 *
 * Revision 1.3  1994/05/20  12:58:28  pfeifer
 * beta
 *
 * Revision 1.2  1994/03/08  21:07:27  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.6  92/03/26  18:27:16  jonathan
 * Fixed function declaration.
 * 
 * Revision 1.5  92/03/07  19:41:14  jonathan
 * ANSIfied argument list.
 * 
 * Revision 1.4  92/02/12  14:01:37  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 *
   3.26.90	Harry Morris, morris@think.com
   3.30.90  Harry Morris - Changed any->bits to any->bytes
   4.11.90  HWM - fixed include file names, changed 
   				- writeCompressedIntegerWithPadding() to
                  writeCompressedIntWithPadding()
                - generalized conditional includes (see c-dialect.h)
   3.7.91   Jonny Goldman.  Replaced "short" in makeBitMap with "int" line 632.
*/

#define _C_utils_Z39_50_

#include "zutil.h"

#include <math.h>
#include "cdialect.h"
/* #include <string.h> */

char* readErrorPosition = NULL; /* pos where buf stoped making sense */

/*----------------------------------------------------------------------*/
/* A note on error handling 
   read - these are low level routines, they do not check the type tags
   which (sometimes) preceed the data (this is done by the higher
   level functions which call these functions).  There is no 
   attempt made to check that the reading does not exceed the read
   buffer.  Such cases should be very rare and usually will be 
   caught by the calling functions. (note - it is unlikely that 
   a series of low level reads will go far off the edge without
   triggering a type error.  However, it is possible for a single
   bad read in an array function (eg. readAny) to attempt to read a 
   large ammount, possibly causing a segmentation violation or out
   of memory condition.
 */
/*----------------------------------------------------------------------*/

diagnosticRecord* 
makeDiag(surrogate,code,addInfo)
boolean surrogate;
char* code;
char* addInfo;
{
  diagnosticRecord* diag = 
    (diagnosticRecord*)s_malloc((size_t)sizeof(diagnosticRecord));
  
  diag->SURROGATE = surrogate;
  memcpy(diag->DIAG,code,DIAGNOSTIC_CODE_SIZE);
  diag->ADDINFO = s_strdup(addInfo); 

  return(diag);
}

/*----------------------------------------------------------------------*/

void 
freeDiag(diag)
diagnosticRecord* diag;
{ 
  if (diag != NULL)
    { if (diag->ADDINFO != NULL)
	s_free(diag->ADDINFO);
	s_free(diag);
      }
}

/*----------------------------------------------------------------------*/

#define END_OF_RECORD	0x1D

char* 
writeDiag(diag,buffer,len)
diagnosticRecord* diag;
char* buffer;
long* len;
/* diagnostics (as per Appendix D) have a very weird format - this changes
   in SR-1
 */
{
  char* buf = buffer;
  long  length;
  
  if (diag == NULL)		/* handle unspecified optional args */
    return(buf);

  buf = writeTag(DT_DatabaseDiagnosticRecords,buf,len);
  CHECK_FOR_SPACE_LEFT(0L,len);
  
  length = 3; 
  if (diag->ADDINFO != NULL)
    length += strlen(diag->ADDINFO);
    
  if (length >= 0xFFFF )	/* make sure the length is reasonable */
    { length = 0xFFFF - 1;
      diag->ADDINFO[0xFFFF - 3 - 1] = '\0';
    }
   
  buf = writeBinaryInteger(length,2L,buf,len);

  CHECK_FOR_SPACE_LEFT(1L,len);
  buf[0] = diag->DIAG[0]; 
  buf++;
  
  CHECK_FOR_SPACE_LEFT(1L,len);
  buf[0] = diag->DIAG[1];
  buf++;
  
  if (length > 3)
    { CHECK_FOR_SPACE_LEFT(3L,len);
      memcpy(buf,diag->ADDINFO,(size_t)length - 3);
      buf += length - 3;
    }
   
  CHECK_FOR_SPACE_LEFT(1L,len);
  buf[0] = diag->SURROGATE;
  buf++;
  
  CHECK_FOR_SPACE_LEFT(1L,len);
  buf[0] = END_OF_RECORD;
  buf++;

  return(buf);
}

/*----------------------------------------------------------------------*/

char* 
readDiag(diag,buffer)
diagnosticRecord** diag;
char* buffer;
{
  char* buf = buffer;
  diagnosticRecord* d 
    = (diagnosticRecord*)s_malloc((size_t)sizeof(diagnosticRecord));
  data_tag tag;
  long len;
  
  buf = readTag(&tag,buf);
  
  buf = readBinaryInteger(&len,2L,buf);
  
  d->DIAG[0] = buf[0];
  d->DIAG[1] = buf[1];
  d->DIAG[2] = '\0';
    
  if (len > 3)
    { d->ADDINFO = (char*)s_malloc((size_t)(len - 3 + 1));
      memcpy(d->ADDINFO,(char*)(buf + 2),(size_t)(len - 3));
      d->ADDINFO[len - 3] = '\0';
    }
  else
    d->ADDINFO = NULL;
    
  d->SURROGATE = buf[len - 1];
  
  *diag = d;

  return(buf + len + 1);
}

/*----------------------------------------------------------------------*/

#define continueBit	0x80
#define dataMask	0x7F
#define dataBits	7

char*
writeCompressedInteger(num,buf,len)
unsigned long num;
char* buf;
long* len;
/* write a binary integer in the format described on p. 40.
   this might be sped up 
*/
{
  char byte;
  long i;
  unsigned long size;
  
  size = writtenCompressedIntSize(num);
  CHECK_FOR_SPACE_LEFT(size,len);
  
  for (i = size - 1; i >= 0; i--)
    { byte = num & dataMask;
      if (i != (size-1))	/* turn on continue bit */
	byte = (char)(byte | continueBit);
      buf[i] = byte;
      num = num >> dataBits;	/* don't and here */
    }
   
  return(buf + size);
} 

/*----------------------------------------------------------------------*/

char*
readCompressedInteger(num,buf)
unsigned long *num;
char* buf;
/* read a binary integer in the format described on p. 40.
   this might be sped up 
*/
{
  register unsigned char byte;
  *num = 0;
  
  do { 
    byte = *(buf++);
    *num = *num << dataBits;
    *num += (byte & dataMask);
  } while (byte & continueBit);
  return(buf);
} 

/*----------------------------------------------------------------------*/

#define pad	128 /* high bit is set */

char*
writeCompressedIntWithPadding(num,size,buffer,len)
unsigned long num;
unsigned long size;
char* buffer;
long* len;
/* Like writeCompressedInteger, except writes padding (128) to make
   sure that size bytes are used.  This can be read correctly by 
   readCompressedInteger()
*/
{
  char* buf = buffer;
  unsigned long needed,padding;
  long i;
    
  CHECK_FOR_SPACE_LEFT(size,len);
  
  needed = writtenCompressedIntSize(num);
  padding = size - needed;
  i = padding - 1;

  for (i = padding - 1;i >= 0;i--)
    { buf[i] = pad;
    }
  
  buf = writeCompressedInteger(num,buf + padding,len);
  
  return(buf);
} 

/*----------------------------------------------------------------------*/

unsigned long
writtenCompressedIntSize(num)
unsigned long num;
/* return the number of bytes needed to represnet the value num in
   compressed format.  curently limited to 4 bytes
 */
{
  if (num < CompressedInt1Byte) 
    return(1);
  else if (num < CompressedInt2Byte) 
    return(2);
  else if (num < CompressedInt3Byte)
    return(3);
  else
    return(4);    
}

/*----------------------------------------------------------------------*/

char*
writeTag(tag,buf,len)
data_tag tag;
char* buf;
long* len;
/* write out a data tag */
{ 
  return(writeCompressedInteger(tag,buf,len));
} 

/*----------------------------------------------------------------------*/

char*
readTag(tag,buf)
data_tag* tag;
char* buf;
/* read a data tag */
{ 
  return(readCompressedInteger(tag,buf));
} 

/*----------------------------------------------------------------------*/

unsigned long 
writtenTagSize(tag)
data_tag tag;
{ 
  return(writtenCompressedIntSize(tag));
}

/*----------------------------------------------------------------------*/

data_tag
peekTag(buf)
char* buf;
/* read a data tag without advancing the buffer */
{
  data_tag tag;
  readTag(&tag,buf);
  return(tag);
} 

/*----------------------------------------------------------------------*/

any* 
makeAny(size,data)
unsigned long size;
char* data;
{
  any* a = (any*)s_malloc((size_t)sizeof(any));
  a->size = size;
  a->bytes = data;
  return(a);
}

/*----------------------------------------------------------------------*/

void
freeAny(a)
any* a;
/* destroy an any and its associated data. Assumes a->bytes was
   allocated using the s_malloc family of libraries 
 */
{
  if (a != NULL)
    { if (a->bytes != NULL)
	s_free(a->bytes);
      s_free(a);
    }
}

/*----------------------------------------------------------------------*/

any* 
duplicateAny(a)
any* a;
{
  any* copy = NULL;

  if (a == NULL)
    return(NULL);

  copy = (any*)s_malloc((size_t)sizeof(any));
  copy->size = a->size;
  if (a->bytes == NULL)
    copy->bytes = NULL;
  else
    { copy->bytes = (char*)s_malloc((size_t)copy->size);
      memcpy(copy->bytes,a->bytes,(size_t)copy->size);
    }
  return(copy);
}

/*----------------------------------------------------------------------*/

char* 
writeAny(a,tag,buffer,len)
any* a;
data_tag tag;
char* buffer;
long* len;
/* write an any + tag and size info */
{
  char* buf = buffer;

  if (a == NULL)		/* handle unspecified optional args */
    return(buf);
  
  /* write the tags */
  buf = writeTag(tag,buf,len);
  buf = writeCompressedInteger(a->size,buf,len);

  /* write the bytes */
  CHECK_FOR_SPACE_LEFT(a->size,len);
  memcpy(buf,a->bytes,(size_t)a->size);

  return(buf+a->size);
}

/*----------------------------------------------------------------------*/

char* 
readAny(anAny,buffer)
any** anAny;
char* buffer;
/* read an any + tag and size info */
{
  char* buf = buffer;
  any* a = (any*)s_malloc((size_t)sizeof(any));
  data_tag tag;
  
  buf = readTag(&tag,buf);
  buf = readCompressedInteger(&a->size,buf);
  /* now simply copy the bytes */
  a->bytes = (char*)s_malloc((size_t)a->size);
  memcpy(a->bytes,buf,(size_t)a->size);
  *anAny = a;
  return(buf+a->size);
}

/*----------------------------------------------------------------------*/

unsigned long 
writtenAnySize(tag,a)
data_tag tag;
any* a;
{
  unsigned long size;

  if (a == NULL)
    return(0);

  size = writtenTagSize(tag);
  size += writtenCompressedIntSize(a->size);
  size += a->size;
  return(size);
}

/*----------------------------------------------------------------------*/

any*
stringToAny(s)
char* s;
{
  any* a = NULL;
  
  if (s == NULL)
    return(NULL);
    
  a = (any*)s_malloc((size_t)sizeof(any));
  a->size = strlen(s);
  a->bytes = (char*)s_malloc((size_t)a->size);
  memcpy(a->bytes,s,(size_t)a->size);
  return(a);
}

/*----------------------------------------------------------------------*/

char*
anyToString(a)
any* a;
{
  char* s = NULL;
  
  if (a == NULL)
    return(NULL);
    
  s = s_malloc((size_t)(a->size + 1));
  memcpy(s,a->bytes,(size_t)a->size);
  s[a->size] = '\0';
  return(s);
}

/*----------------------------------------------------------------------*/

char* 
writeString(s,tag,buffer,len)
char* s;
data_tag tag;
char* buffer;
long* len;
/* Write a C style string.  The terminating null is not written. 
   This function is not part of the Z39.50 spec.  It is provided
   for the convienience of those wishing to pass C strings in 
   the place of an any.
 */
{
  char* buf = buffer;
  any* data = NULL;
  if (s == NULL)
    return(buffer);		/* handle unused optional item before making an any */
  data = (any*)s_malloc((size_t)sizeof(any)); 
  data->size = strlen(s);
  data->bytes = s;		/* save a copy here by not using stringToAny() */
  buf = writeAny(data,tag,buf,len);
  s_free(data);			/* don't use freeAny() since it will free s too */
  return(buf);
}

/*----------------------------------------------------------------------*/

char* 
readString(s ,buffer)
char** s ;
char* buffer;
/* Read an any and convert it into a C style string.
   This function is not part of the Z39.50 spec.  It is provided
   for the convienience of those wishing to pass C strings in 
   the place of an any. 
 */
{
  any* data = NULL;
  char* buf = readAny(&data,buffer);
  *s = anyToString(data);
  freeAny(data);
  return(buf);
}

/*----------------------------------------------------------------------*/

unsigned long 
writtenStringSize(tag,s)
data_tag tag;
char* s;
{
  unsigned long size;

  if (s == NULL)
   return(0);

  size = writtenTagSize(tag);
  size += writtenCompressedIntSize(size);
  size += strlen(s);
  return(size);
}

/*----------------------------------------------------------------------*/

any* 
longToAny(num)
long num;
/* a convienience function */
{
  char s[40];

  sprintf(s,"%ld",num);
  
  return(stringToAny(s));
}

/*----------------------------------------------------------------------*/

long
anyToLong(a)
any* a;
/* a convienience function */
{
  long num;
  char* str = NULL;
  str = anyToString(a);
  sscanf(str,"%ld",&num);	/* could check the result and return
				   an error */
  s_free(str);
  return(num);
}
 
/*----------------------------------------------------------------------*/

#define bitsPerByte	8

#ifdef ANSI_LIKE /* use ansi */

bit_map*
makeBitMap(unsigned long numBits,...)
/* construct and return a bitmap with numBits elements */
{
  va_list ap;
  long i,j;
  bit_map* bm = NULL;

  va_start(ap,numBits);
  
  bm = (bit_map*)s_malloc((size_t)sizeof(bit_map));
  bm->size = (unsigned long)ceil((double)numBits / bitsPerByte); 
  bm->bytes = (char*)s_malloc((size_t)bm->size);
  
  /* fill up the bits */
  for (i = 0; i < bm->size; i++) /* iterate over bytes */
    { char byte = 0;
      for (j = 0; j < bitsPerByte; j++) /* iterate over bits */
	{ if ((i * bitsPerByte + j) < numBits)
	    { boolean bit = false;
	      bit = (boolean)va_arg(ap,boolean); 
	      if (bit)
	        { byte = byte | (1 << (bitsPerByte - j - 1));
	        }
	    }
	  }
      bm->bytes[i] = byte;
    }

  va_end(ap);
  return(bm);
}

#else /* use K & R */

bit_map*
makeBitMap(va_alist)
va_dcl
/* construct and return a bitmap with numBits elements */
{
  va_list ap;
  long i,j;
  unsigned long numBits;
  bit_map* bm = (bit_map*)s_malloc((size_t)sizeof(bit_map));

  va_start(ap);
  
  numBits = va_arg(ap,unsigned long);

  bm->size = (unsigned long)ceil((double)numBits / bitsPerByte);
  
  bm->bytes = (char*)s_malloc((size_t)bm->size);
  
  /* fill up the bits */
  for (i = 0; i < bm->size; i++) /* iterate over bytes */
    { char byte = 0;
      for (j = 0; j < bitsPerByte; j++) /* iterate over bits */
	{ if ((i * bitsPerByte + j) < numBits)
	    { boolean bit;
	      bit = (boolean)va_arg(ap,int); /* really boolean, but this 
						is how it is passed on stack */
	      if (bit)
	        { byte = byte | (1 << (bitsPerByte - j - 1));
	        }
	    }
	  }
      bm->bytes[i] = byte;
    }

  va_end(ap);

  return(bm);
}

#endif 

/*----------------------------------------------------------------------*/

void
freeBitMap(bm)
bit_map* bm;
/* destroy a bit map created by makeBitMap() */
{
  s_free(bm->bytes);
  s_free(bm);
}

/*----------------------------------------------------------------------*/

/* use this routine to interpret a bit map.  pos specifies the bit 
   number.  bit 0 is the Leftmost bit of the first byte.  
   Could do bounds checking.
 */

boolean
bitAtPos(pos,bm)
long pos;
bit_map* bm;
{
  if (pos > bm->size*bitsPerByte)
    return false;
  else
    return((bm->bytes[(pos / bitsPerByte)] & 
	    (0x80>>(pos % bitsPerByte))) ?
	   true : false);
}

/*----------------------------------------------------------------------*/

char*
writeBitMap(bm,tag,buffer,len)
bit_map* bm;
data_tag tag;
char* buffer;
long* len;
/* write a bitmap + type and size info */
{ 
  return(writeAny((any*)bm,tag,buffer,len));
}

/*----------------------------------------------------------------------*/

char*
readBitMap(bm,buffer)
bit_map** bm;
char* buffer;
/* read a bitmap + type and size info */
{ 
  return(readAny((any**)bm,buffer));
}

/*----------------------------------------------------------------------*/

char* 
writeByte(byte,buf,len)
unsigned long byte;
char* buf;
long* len;
{
  CHECK_FOR_SPACE_LEFT(1L,len);
  buf[0] = byte & 0xFF; /* we really only want the first byte */
  return(buf + 1);
}

/*----------------------------------------------------------------------*/

char* 
readByte(byte,buf)
unsigned char* byte;
char* buf;
{
  *byte = buf[0];
  return(buf + 1);
}

/*----------------------------------------------------------------------*/

char* 
writeBoolean(flag,buf,len)
boolean flag;
char* buf;
long* len;
{
  return(writeByte(flag,buf,len));
}

/*----------------------------------------------------------------------*/

char* 
readBoolean(flag,buffer)
boolean* flag;
char* buffer;
{
  unsigned char byte;
  char* buf = readByte(&byte,buffer);
  *flag = (byte == true) ? true : false;
  return(buf);
}

/*----------------------------------------------------------------------*/

char*
writePDUType(pduType,buf,len)
pdu_type pduType;
char* buf;
long* len;
/* PDUType is a single byte */
{
  return(writeBinaryInteger((long)pduType,(unsigned long)1,buf,len));
} 

/*----------------------------------------------------------------------*/

char*
readPDUType(pduType,buf)
pdu_type* pduType;
char* buf;
/* PDUType is a single byte */
{
  return(readBinaryInteger((long*)pduType,(unsigned long)1,buf));
} 

/*----------------------------------------------------------------------*/

pdu_type
peekPDUType(buf)
char* buf;
/* read the next pdu without advancing the buffer, Note that this 
   function is to be used on a buffer that is known to contain an
   APDU.  The pdu_type is written HEADER_LEN bytes into the buffer
 */
{
  pdu_type pdu;
  readPDUType(&pdu,buf + HEADER_LEN);
  return(pdu);
}

/*----------------------------------------------------------------------*/

#define BINARY_INTEGER_BYTES	sizeof(long) /* the number of bytes used by
						a "binary integer" */
char*
writeBinaryInteger(num,size,buf,len)
long num;
unsigned long size;
char* buf;
long* len;
/* write out first size bytes of num - no type info
  XXX should this take unsigned longs instead ???  */
{
  long i;
  char byte;

  if (size < 1 || size > BINARY_INTEGER_BYTES)
    return(NULL);		/* error */

  CHECK_FOR_SPACE_LEFT(size,len);

  for (i = size - 1; i >= 0; i--)
    { byte = (char)(num & 255);
      buf[i] = byte;
      num = num >> bitsPerByte; /* don't and here */
    }

  return(buf + size);
}

/*----------------------------------------------------------------------*/

char*
readBinaryInteger(num,size,buf)
long* num;
unsigned long size;
char* buf;
/* read in first size bytes of num - no type info
  XXX this should take unsigned longs instead !!! */
{
  long i;
  unsigned char byte;
  if (size < 1 || size > BINARY_INTEGER_BYTES)
    return(buf);		/* error */
  *num = 0;
  for (i = 0; i < size; i++)
    { byte = buf[i];
      *num = *num << bitsPerByte;
      *num += byte;
    }
  return(buf + size);
}

/*----------------------------------------------------------------------*/

unsigned long 
writtenCompressedBinIntSize(num)
long num;
/* return the number of bytes needed to represent the value num.
   currently limited to max of 4 bytes 
   Only compresses for positive nums - negatives get whole 4 bytes
 */
{
  if (num < 0L)
    return(4);
  else if (num < 256L)		/* 2**8 */
    return(1);
  else if (num < 65536L)	/* 2**16 */
    return(2);
  else if (num < 16777216L)	/* 2**24 */
    return(3);
  else
    return(4);
}
 
/*----------------------------------------------------------------------*/

char*
writeNum(num,tag,buffer,len)
long num;
data_tag tag;
char* buffer;
long* len;
/* write a binary integer + size and tag info */
{
  char* buf = buffer;
  long size = writtenCompressedBinIntSize(num);
  
  if (num == UNUSED)
    return(buffer);
    
  buf = writeTag(tag,buf,len);
  buf = writeCompressedInteger(size,buf,len); 
  buf = writeBinaryInteger(num,(unsigned long)size,buf,len); 
  return(buf);
}

/*----------------------------------------------------------------------*/

char*
readNum(num,buffer)
long* num;
char* buffer;
/* read a binary integer + size and tag info */
{
  char* buf = buffer;
  data_tag tag;
  unsigned long size;
  unsigned long val;
  
  buf = readTag(&tag,buf);
  buf = readCompressedInteger(&val,buf);
  size = (unsigned long)val;
  buf = readBinaryInteger(num,size,buf);
  return(buf);
}

/*----------------------------------------------------------------------*/

unsigned long 
writtenNumSize(tag,num)
data_tag tag;
long num;
{
  long dataSize = writtenCompressedBinIntSize(num);
  long size;
  
  size = writtenTagSize(tag); /* space for the tag */
  size += writtenCompressedIntSize(dataSize); /* space for the size */
  size += dataSize; /* space for the data */
  
  return(size);
}

/*----------------------------------------------------------------------*/

typedef void (voidfunc)();

void
doList(list,func)
void** list;
voidfunc *func;
/* call func on each element of the NULL terminated list of pointers */
{
  register long i;
  register void* ptr = NULL;
  if (list == NULL)
    return;
  for (i = 0,ptr = list[i]; ptr != NULL; ptr = list[++i])
    (*func)(ptr);
}

/*----------------------------------------------------------------------*/

char* 
writeProtocolVersion(buf,len)
char* buf;
long* len;
/* write a bitmap describing the protocols available */
{
  static bit_map* version = NULL;

  if (version == NULL)
   { version = makeBitMap((unsigned long)1,true); /* version 1! */
   }
    
  return(writeBitMap(version,DT_ProtocolVersion,buf,len));
}

/*----------------------------------------------------------------------*/

char*
defaultImplementationID()
{
  static char	ImplementationID[] = "LS6";
  return(ImplementationID);
}

/*----------------------------------------------------------------------*/

char*
defaultImplementationName()
{
  static char ImplementationName[] = "University of Dortmund Informatik VI";
  return(ImplementationName);
}

/*----------------------------------------------------------------------*/

char*
defaultImplementationVersion()
{
  static char	ImplementationVersion[] = VERSION_STRING;
  char *s = ImplementationVersion;
  while (*s && *s != ' ') { s++; };
  while (*s && *s == ' ') { s++; };
  return(s);
}

/*----------------------------------------------------------------------*/

