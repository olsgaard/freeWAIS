/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef lint
static char *RCSid = "$Header: /usr/local/ls6/src+data/src/freeWAIS-sf-2.0/ir/RCS/transprt.c,v 2.0.1.1 1995/09/15 09:48:45 pfeifer Exp $";
#endif

/* this is a krufty transport layer for comm mediums that can not handle
 * all 8 bits of ascii.
 * Written by Harry Morris, modified by brewster.
 * Hexcode written by Steve Swartz
 * 
 */

/* to do
 *
 *  compression
 *  encryption
 */

/* Change log:
 * $Log: transprt.c,v $
 * Revision 2.0.1.1  1995/09/15 09:48:45  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:02:05  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.2  1994/03/08  21:07:02  pfeifer
 * Patchlevel 04
 *
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.2  92/02/12  13:51:34  jonathan
 * Added "$Log" so RCS will put the log message in the header
 * 
 * 
 * 7/7/90 -brewster
 * merged arts and harry's changes 7/7/90 -brewster
 *
 * 11/6/90 -tracy
 * shift 6 bytes starting from [ as IBM gateway program
 * masks out characters [\]^` in input,
 * append H and I to the function names corresponding to the
 * oroginal coding and the modified one. 'I' indicates IBM. 
 */

#include "transprt.h"
#include "wmessage.h" 
#include "ustubs.h"

/*---------------------------------------------------------------------*/
static char alphaH[] = /* Must be 65 consecutive ascii characters */
  "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnop";


/*---------------------------------------------------------------------*/

static void hexCodeH _AP((char* data,long* len));

static void
hexCodeH(data,len)
char* data;
long* len;
/* compose a message for sending over ascii terminal lines. Input is '*len'
 * binary bytes; output is 'ceiling(*len/3) * 4' characters from alpha[].
 *
 * Encoding goes like this. Let capital letters represent bits:
 * Input bytes:  ABCDEFGH IJKLMNOP QRSTUVWX
 * Output chars: alpha[CDEFGH] alpha[ABIJKL] alpha[QRMNOP] alpha[SRUVWX]
 *
 * 1 byte at end: ABCDEFGH
 * Output chars:  alpha[CDEFGH] alpha[AB0000] alpha[1000000] alpha [1000000]
 *
 * 2 bytes at end: ABCDEFGH IJKLMNOP
 * Output chars:   alpha[CDEFGH] alpha[ABIJKL] alpha[00MNOP] alpha [1000000]
 */
{
  long i, j;
  
  switch (*len % 3)
    {
    case 2:
      i = *len; j = (*len+1)/3 * 4 - 1; *len = j + 1;
      data[j]   = alphaH[64];
      data[j-1] = alphaH[data[i-1] & 0x0f];
      data[j-2] = alphaH[(data[i-2] & 0xc0) >> 2 | (data[i-1] & 0xf0) >> 4];
      data[j-3] = alphaH[data[i-2] & 0x3F];
      break;
    case 1:
      i = *len + 1; j = (*len+2)/3 * 4 - 1; *len = j + 1;
      data[j]   = alphaH[64];
      data[j-1] = alphaH[64];
      data[j-2] = alphaH[(data[i-2] & 0xc0) >> 2];
      data[j-3] = alphaH[data[i-2] & 0x3F];
      break;
    default:
      i = *len + 2; j = (*len)/3 * 4 + 3; *len = j - 3;
      break;
    }
  
  for (i -= 3, j -= 4; i > 0; i -= 3, j -= 4)
    { 
      data[j]   = alphaH[data[i] & 0x3F];
      data[j-1] = alphaH[(data[i] & 0xc0) >> 2 | (data[i-1] & 0x0f)];
      data[j-2] = alphaH[(data[i-2] & 0xc0) >> 2 | (data[i-1] & 0xf0) >> 4];
      data[j-3] = alphaH[data[i-2] & 0x3F];
    }
}

/*---------------------------------------------------------------------*/

static void hexDecodeH _AP((char* data,long* len));

static void
hexDecodeH(data,len)
char* data;
long* len;
/* Incoming data is 'len' letters from alpha[] created by hexCode(). 'len'
 *  will always be divisble by 4. Output is the binary bytes that were 
 *  encoded into 'data'.
 *
 * If alpha[] isn't consecutive ascii characters, then the function moving 
 *  from ascii data to the 'bX' values has to become more complicated than
 *  data[] - alpha[0].
 */
{
  unsigned char b0, b1, b2, b3;
  long i, j;

  
  for (i = 3, j = 4; j < *len; i += 3, j += 4)
    {
      b0 = (data[j-4] & 0x07F) - alphaH[0];
      b1 = (data[j-3] & 0x07F) - alphaH[0];
      b2 = (data[j-2] & 0x07F) - alphaH[0];
      b3 = (data[j-1] & 0x07F) - alphaH[0];
      data[i-3] = b0 | (b1 & 0x30) << 2;
      data[i-2] = (b1 & 0x0f) << 4 | (b2 & 0x0f);
      data[i-1] = b3 | (b2 & 0x30) << 2;
    }

  *len = *len/4 * 3 - 2;
  b0 = (data[j-4] & 0x07F) - alphaH[0];
  b1 = (data[j-3] & 0x07F) - alphaH[0];
  b2 = (data[j-2] & 0x07F) - alphaH[0];
  b3 = (data[j-1] & 0x07F) - alphaH[0];
  data[i-3] = b0 | (b1 & 0x30) << 2;

  if (b2 != 64)    
    {
      data[i-2] = (b1 & 0x0f) << 4 | (b2 & 0x0f);
      (*len)++;
    }

  if (b3 != 64)
    {
      data[i-1] = b3 | (b2 & 0x30) << 2;
      (*len)++;
    }
}

/*---------------------------------------------------------------------*/
 
void
transportCodeH(data,len)
char* data;
long* len;
/* krufty serial line transport layer - encode in hex and append in cr */
/* Now it also writes a wais packet header on to the front of the packet.
 *   --art 
 */
{ 
  hexCodeH(data,len);
  data[*len] = '\r';
  data[*len + 1] = '\0';
  *len += 1;
}

/*---------------------------------------------------------------------*/

void
transportDecodeH(data,len)
char* data;
long* len;
/* decode above */
/* This does not have to deal with the wais packet header since it has
   been removed at the end of the read.  */
{ 
  if (*len == 0)
    return;
    
  *len -= 1;
  hexDecodeH(data,len);
}

/*---------------------------------------------------------------------
 Below are the set of transport coding/decoding functions for clients
 dialing through IBM to DowQuest. Unlike the above one that
 uses 65 consecutive ASCII character for the coding/decoding table,
 table entries after charcater 'Z' are shift 6 characters.
 *--------------------------------------------------------------------*/ 

#define LAST_NON_SHIFT_CHAR 90
#define SHIFT_OFFSET   6

static char alphaI[] = 
  "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv";


static void hexCodeI _AP((char* data,long* len));

static void
hexCodeI(data,len)
char* data;
long* len;
/* compose a message for sending over ascii terminal lines. Input is '*len'
 * binary bytes; output is 'ceiling(*len/3) * 4' characters from alpha[].
 *
 * Encoding goes like this. Let capital letters represent bits:
 * Input bytes:  ABCDEFGH IJKLMNOP QRSTUVWX
 * Output chars: alpha[CDEFGH] alpha[ABIJKL] alpha[QRMNOP] alpha[SRUVWX]
 *
 * 1 byte at end: ABCDEFGH
 * Output chars:  alpha[CDEFGH] alpha[AB0000] alpha[1000000] alpha [1000000]
 *
 * 2 bytes at end: ABCDEFGH IJKLMNOP
 * Output chars:   alpha[CDEFGH] alpha[ABIJKL] alpha[00MNOP] alpha [1000000]
 */
{
  long i, j;
  
  switch (*len % 3)
    {
    case 2:
      i = *len; j = (*len+1)/3 * 4 - 1; *len = j + 1;
      data[j]   = alphaI[64];
      data[j-1] = alphaI[data[i-1] & 0x0f];
      data[j-2] = alphaI[(data[i-2] & 0xc0) >> 2 | (data[i-1] & 0xf0) >> 4];
      data[j-3] = alphaI[data[i-2] & 0x3F];
      break;
    case 1:
      i = *len + 1; j = (*len+2)/3 * 4 - 1; *len = j + 1;
      data[j]   = alphaI[64];
      data[j-1] = alphaI[64];
      data[j-2] = alphaI[(data[i-2] & 0xc0) >> 2];
      data[j-3] = alphaI[data[i-2] & 0x3F];
      break;
    default:
      i = *len + 2; j = (*len)/3 * 4 + 3; *len = j - 3;
      break;
    }
  
  for (i -= 3, j -= 4; i > 0; i -= 3, j -= 4)
    { 
      data[j]   = alphaI[data[i] & 0x3F];
      data[j-1] = alphaI[(data[i] & 0xc0) >> 2 | (data[i-1] & 0x0f)];
      data[j-2] = alphaI[(data[i-2] & 0xc0) >> 2 | (data[i-1] & 0xf0) >> 4];
      data[j-3] = alphaI[data[i-2] & 0x3F];
    }
}

/*---------------------------------------------------------------------*/

static void hexDecodeI _AP((char* data,long * len));

static void
hexDecodeI(data,len)
char* data;
long* len;
/* Incoming data is 'len' letters from alpha[] created by hexCode(). 'len'
 *  will always be divisble by 4. Output is the binary bytes that were 
 *  encoded into 'data'.
 *
 * If alpha[] isn't consecutive ascii characters, then the function moving 
 *  from ascii data to the 'bX' values has to become more complicated than
 *  data[] - alpha[0].
 */
{
  unsigned char b0, b1, b2, b3;
  long i, j;

  for ( i=0; i < *len ; i++ )
    if ( data[i] > LAST_NON_SHIFT_CHAR ) data[i] -= SHIFT_OFFSET;
  
  for (i = 3, j = 4; j < *len; i += 3, j += 4)
    {
      b0 = (data[j-4] & 0x07F) - alphaI[0];
      b1 = (data[j-3] & 0x07F) - alphaI[0];
      b2 = (data[j-2] & 0x07F) - alphaI[0];
      b3 = (data[j-1] & 0x07F) - alphaI[0];
      data[i-3] = b0 | (b1 & 0x30) << 2;
      data[i-2] = (b1 & 0x0f) << 4 | (b2 & 0x0f);
      data[i-1] = b3 | (b2 & 0x30) << 2;
    }

  *len = *len/4 * 3 - 2;
  b0 = (data[j-4] & 0x07F) - alphaI[0];
  b1 = (data[j-3] & 0x07F) - alphaI[0];
  b2 = (data[j-2] & 0x07F) - alphaI[0];
  b3 = (data[j-1] & 0x07F) - alphaI[0];
  data[i-3] = b0 | (b1 & 0x30) << 2;

  if (b2 != 64)    
    {
      data[i-2] = (b1 & 0x0f) << 4 | (b2 & 0x0f);
      (*len)++;
    }

  if (b3 != 64)
    {
      data[i-1] = b3 | (b2 & 0x30) << 2;
      (*len)++;
    }
}

/*---------------------------------------------------------------------*/
 
void
transportCodeI(data,len)
char* data;
long* len;
/* krufty serial line transport layer - encode in hex and append in cr */
/* Now it also writes a wais packet header on to the front of the packet.
 *   --art 
 */
{ 
  hexCodeI(data,len);
  data[*len] = '\r';
  data[*len + 1] = '\0';
  *len += 1;
}

/*---------------------------------------------------------------------*/

void
transportDecodeI(data,len)
char* data;
long* len;
/* decode above */
/* This does not have to deal with the wais packet header since it has
   been removed at the end of the read.  */
{ 
  if (*len == 0)
    return;
    
  *len -= 1;
  hexDecodeI(data,len);
}

/*---------------------------------------------------------------------*/


#ifdef NOTDEFINED /* here for back compatibility */

static void hexCode _AP((char* data,long * len));

static void
hexCode(data,len)
char* data;
long* len;
/* compose a message for sending over ascii terminal lines.  The data is hex,
 */
{ long i;
  for (i = *len-1; i >= 0; i--)
    { data[i*2+1] = (data[i]&0x0F) + 'a';
      data[i*2] = ((data[i]>>4)&0x0F) + 'a';
    }
  data[*len*2] = '\0';
  *len = *len * 2;
}

/*---------------------------------------------------------------------------*/

static void hexDecode _AP((char* data,long * len));

static void
hexDecode(data,len)
char* data;
long* len;
/* converts a buffer full of dataLen bytes of hex in place into the
equivalent binary form. len is filled with the actual number of binary bytes.
 */
{ long i;
  *len = *len / 2;
  for (i = 0; i < *len; i++)
    { long temp = 0;
      temp = (data[i*2]-'a')<<4;
      temp += data[i*2+1]-'a';
      data[i] = (char)temp;
    }
  data[*len] = '\0';
}
 
#endif /* def NOTDEFINED */
/*---------------------------------------------------------------------------*/

boolean
transportCode(encoding,data,len)
long encoding;
char* data;
long* len;
/* krufty serial line transport layer - encode in hex and add trailing nl */
{
  switch (encoding){
  case NO_ENCODING:
    /* do nothing */
    return(true);
  case HEX_ENCODING:
    transportCodeH(data,len);
    return(true);
  case IBM_HEXCODING:
    transportCodeI(data,len);
    return(true);
  default:
    return(false);
  }
}

/*---------------------------------------------------------------------------*/

boolean
transportDecode(encoding,data,len)
long encoding;
char* data;
long* len;
/* decode above */
{
  switch (encoding){
  case NO_ENCODING:
    /* do nothing */
    return(true);
  case HEX_ENCODING:
    transportDecodeH(data,len);
    return(true);
  case IBM_HEXCODING:
    transportDecodeI(data,len);
    return(true);
  default:
    return(false);
  }
}

/*---------------------------------------------------------------------------*/






#ifdef old /* from the bad old days */


/*---------------------------------------------------------------------------*/

void
transportCode(data,len)
char* data;
long* len;
/* krufty serial line transport layer - encode in hex and add trailing nl */
{
  hexCode(data,len);
  data[*len] = '\r';
  data[*len+1] = '\0';
}

/*---------------------------------------------------------------------------*/

void
transportDecode(data,len)
char* data;
long* len;
/* decode above */
{
  hexDecode(data,len);
}

/*---------------------------------------------------------------------------*/

#endif /* old */
