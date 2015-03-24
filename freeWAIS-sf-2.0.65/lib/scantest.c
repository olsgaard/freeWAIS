
/* Copyright (c) CNIDR (see ../doc/CNIDR/COPYRIGHT) */

#include "../ir/cdialect.h"
#include <stdio.h>
/* #include <sys/types.h>*/
#include <sys/stat.h>

extern int alphasort();

static int
filesonly(e)
	Direntry_t *e;
{
	struct stat sb;

	return(stat(e->d_name, &sb) >= 0 && (sb.st_mode & S_IFMT) == S_IFREG);
}

main(ac, av)
	int ac;
	char *av[];
{
	register int i;
	register int j;
	Direntry_t **list;

	if (ac != 2) {
		fprintf(stderr, "usage: %s dirname\n", av[0]);
		exit(1);
	}
	if (chdir(av[1]) < 0) {
		perror(av[1]);
		exit(1);
	}
	if ((i = scandir(".", &list, filesonly, alphasort)) < 0) {
		perror("Error reading directory");
		exit(1);
	}
	for (j = 0; j < i; j++)
		printf("%s\n", list[j]->d_name);
	for (j = 0; j < i; j++)
		free((char *)list[j]);
	free((char *)list);
	exit(0);
}
