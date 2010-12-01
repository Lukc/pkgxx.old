#include "stdio.h"
#include "string.h"

int main(int argc, char **argv) {
	int i = 2; /* 0 is the executable name, 1 is the flag */
	char *flag = argv[1];
	if (flag == NULL) {
		return 2;
	}
	while (argv[i] != NULL) {
		if (strcmp(flag,argv[i]) == 0) {
			return 0;
		}
		i++;
	}
	return 1;
}

