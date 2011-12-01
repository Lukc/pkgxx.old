#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Execution times on the time of first write:
 * zsh: 35.133498191833496s
 * c:   16.015569448471069s
 * So, this implementation is just two times faster, fork()
 * time included!
 */

int main(int argc, char **argv) {
	if (argv[1] == NULL || strcmp(argv[1],"") == 0)
		/* 
		 * Well, the sh implementation would not have said anything,
		 * but the C one is a little more strict. To be precise,
		 * it would segfault and cry if no option was given, which
		 * is not something we might want to manage externally.
		 */
		return 3;
	else if (
		strcmp(argv[1],"true") == 0 || 
		strcmp(argv[1],"TRUE") == 0 ||
		strcmp(argv[1],"yes") == 0 || 
		strcmp(argv[1],"YES") == 0 ||
		strcmp(argv[1],"y") == 0 ||
		strcmp(argv[1],"Y") == 0 ||
		strcmp(argv[1],"1") == 0
	)
		return 0;
	else if (
		strcmp(argv[1],"false") == 0 ||
		strcmp(argv[1],"FALSE") == 0 ||
		strcmp(argv[1],"no") == 0 ||
		strcmp(argv[1],"NO") == 0 ||
		strcmp(argv[1],"n") == 0 ||
		strcmp(argv[1],"N") == 0 ||
		strcmp(argv[1],"0") == 0
	)
		return 1;
	return 2;
}

