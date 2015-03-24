/* WIDE AREA INFORMATION SERVER SOFTWARE	
   No guarantees or restrictions.  See the readme file for the full standard 
   disclaimer.
   5.29.90	Harry Morris, morris@think.com
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


/* this file is a server process for a unix machine that takes input from 
   standard in or from a socket and searches the local search engine on the 
   unix box.
   originally written by harry morris.
*/

#define BUFSZ 100000     /* size of our comm buffer */
