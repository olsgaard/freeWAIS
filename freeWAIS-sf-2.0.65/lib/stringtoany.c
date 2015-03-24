/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.	
  
 *
 * Change log:
 * $Log: stringtoany.c,v $
 * Revision 2.0.1.1  1995/09/15 09:52:29  pfeifer
 * patch7: Fixed CNIDR copyright line.
 *
 * Revision 2.0  1995/09/08  08:03:45  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.1  1994/08/05  07:13:50  pfeifer
 * Initial revision
 *
 * Revision 1.1  1993/02/16  15:08:58  freewais
 * Initial revision
 *
 * Revision 1.3  92/02/27  09:26:14  jonathan
 * seperated =* since some compilers get confused (ULTRIX).
 * 
 * Revision 1.2  92/02/16  18:23:52  jonathan
 * Utility to make an any out of a string
 * 
*/

/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */


main(argc,argv)
int argc;
char **argv;
{
  char c;
  printf("(:any :size %d :bytes #( ", strlen(argv[1]));
  while(c = *argv[1]++) printf("%d ", c);
  printf(") )\n");
  exit(0);
}
