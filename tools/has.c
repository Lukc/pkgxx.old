#include <string.h>

int main(int argc, char **argv) {
	char *flag = *++argv;
	if (flag == NULL)
		return 2;
	while (*++argv != NULL)
		if (!strcmp(flag, *argv))
			return 0;
	return 1;
}

