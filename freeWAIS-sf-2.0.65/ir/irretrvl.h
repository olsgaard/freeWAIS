/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef IRRETRVL_H
#define IRRETRVL_H

#include "cdialect.h"
#include "docid.h"
#include "wprot.h"

/* error codes for getDocumentText() */
#define GDT_NoError				0
#define GDT_UnsupportedChunkType		1
#define GDT_BadDocID			       	2
#define GDT_MissingDocID			3
#define GDT_BadRange				4
#define GDT_MissingDatabase			5
#define GDT_BadDatabase				6

WAISDocumentText* getData _AP((DocObj* doc, long* errorCode,
			       char* index_directory));

WAISDocumentText* getDocumentText _AP((DocObj* doc, long* errorCode,
				       char* index_directory));

boolean parseDocID _AP((DocObj* doc,char* filename,long* start_character,
			long* end_character,long* errorCode));

#endif /* ndef IRRETRVL_H */

