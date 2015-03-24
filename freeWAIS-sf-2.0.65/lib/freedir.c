
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

/* free list malloc'd by scandir */
void
freedir(list)
  char **list;
{
    char **s = list;

    while (*s) { 
	free(*s++);
    }
    free((char *)list);
}
