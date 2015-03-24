/* WIDE AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.  
  
   3.26.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/*----------------------------------------------------------------------*/
/* This code implements a simple Z39.50+WAIS server, which consults a 
   local database useing Brewster's search engine.  The main routine is
   interpret_buffer() which reads the contents of a receive buffer, and 
   writes results back to a send buffer.
 */
/*----------------------------------------------------------------------*/

#ifndef IR_H
#define IR_H

#include "cdialect.h"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */

long interpret_buffer _AP((char* receiveBuffer,long receiveBufLen,
			   char* sendBuffer,long sendBufLen,
			   long* maxBufferSize,
			   long waisProtocolVersion,
			   char *index_directory));

#define SERVSECURITYFILE        "SERV_SEC"
#define DATASECURITYFILE        "DATA_SEC"


#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* ndef IR_H */




