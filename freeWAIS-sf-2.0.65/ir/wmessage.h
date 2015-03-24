/* WIDE AREA INFORMATION SERVER SOFTWARE
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.    
   3.26.90
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* wais-message.h
 *
 * This is the header outside of WAIS Z39.50 messages.  The header will be
 * printable ascii, so as to be transportable.  This header will precede each
 * Z39.50 APDU, or zero-length message if it is an ACK or NACK.  Be sure to
 * change hdr_vers current value if you change the structure of the header.
 *
 * The characters in the header are case insensitive so that the systems from
 * the past that only handle one case can at least read the header.
 *
 * 7.5.90 HWM - added constants
 * 7/5/90 brewster added funtion prototypes and comments
 * 11/30/90 HWM - went to version 2 (inits and typed retrieval)
 */

#ifndef WMESSAGE_H
#define WMESSAGE_H

#include "cdialect.h"

typedef struct wais_header {
        char    msg_len[10];    /* length in bytes of following message */
        char    msg_type;       /* type of message: 'z'=Z39.50 APDU,
                                   'a'=ACK, 'n'=NACK */
        char    hdr_vers;       /* version of this header, currently = '2' */
        char    server[10];     /* name or address of server */
        char    compression;    /* <sp>=no compression, 'u'=unix compress */
        char    encoding;       /* <sp>=no encoding, 'h'=hexize, 
				   'u'=uuencode */
        char    msg_checksum;   /* XOR of every byte of message */
        } WAISMessage;

#define HEADER_LENGTH 	25	/* number of bytes needed to write a 
				   wais-header (not sizeof(wais_header)) */

#define HEADER_VERSION 	(long)'2'

/* message type */
#define Z3950		'z'  
#define ACK			'a'  
#define	NAK			'n'  

/* compression */
#define NO_COMPRESSION 		' ' 
#define UNIX_COMPRESSION 	'u' 

/* encoding */
#define NO_ENCODING		' '  
#define HEX_ENCODING	'h'  /* Swartz 4/3 encoding */
#define IBM_HEXCODING	'i'	 /* same as h but uses characters acceptable for IBM mainframes */
#define UUENCODE		'u'  

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

void readWAISPacketHeader _AP((char* msgBuffer,WAISMessage *header_struct));
long getWAISPacketLength _AP((WAISMessage* header));
void writeWAISPacketHeader _AP((char* header,long dataLen,long type,
				char* server,long compression,	
				long encoding,long version));

#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* ndef WMESSAGE_H */
