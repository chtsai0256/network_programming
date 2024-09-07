#include <stdio.h>

typedef struct test {
	int a;
	struct test *next;
} test_t;

int main()
{
	test_t *p = NULL;

	printf("sizeof poniter = %ld\n", sizeof(test_t*));
	
}
