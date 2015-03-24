/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   This is part of the shell user-interface for the WAIS software.  Do with it
   as you please.

   jonathan@Think.COM

 * $Log: globals.h,v $
 * Revision 2.0.1.1  1995/09/15 09:53:17  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:05:05  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.4  1994/08/08  07:33:06  pfeifer
 * Moved wais_log_file_name and waislogfile to cutil.[ch]
 *
 * Revision 1.3  1994/08/05  08:55:03  pfeifer
 * dumped numtosave since it's unused
 *
 * Revision 1.2  1994/08/05  07:29:58  pfeifer
 * Release beta 04
 *
 * Revision 1.1  93/06/23  20:02:32  warnock
 * Initial revision
 * 
 * Revision 1.1  1993/02/16  15:09:27  freewais
 * Initial revision
 *
 */

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


#ifndef H_GLOBALS
#define H_GLOBALS

#ifdef MAIN
#define ext
#else
#define ext extern
#endif

ext char* command_name;

ext int NumQuestions;

ext Question the_Question;

ext SList Sources;
ext int NumSources;

/*
ext char* wais_log_file_name;
ext FILE* waislogfile;
*/
#endif
