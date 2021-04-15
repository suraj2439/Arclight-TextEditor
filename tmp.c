#include <stdio.h>
#include <limits.h>

int main() {
	FILE *fd = fopen("tmp.txt", "w+");
	for(int i = 0; i < INT_MAX/40; i++)
		fprintf(fd, "%d\n", i);
	fclose(fd);
	return 0;
}
