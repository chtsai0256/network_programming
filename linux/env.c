#include <stdlib.h>
#include <stdio.h>

int main()
{
	if (!setenv("USER", "lx138",0))
		printf("error\n");
	printf("%s\n", getenv("USER"));

}
