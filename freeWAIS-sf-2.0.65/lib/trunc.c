
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
  int len, i, c;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s length\n", argv[0]);
    exit(-1);
  }

  len = atoi(argv[1]);
  i = 0;
  while (EOF != (c = getchar())) {
    if (c == '\n') {
      putchar(c);
      fflush(stdout);
      i = 0;
    }
    else if (i++ < len)
      putchar(c);
  }
}
