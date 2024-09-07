#include <stdio.h>
#include "config.h"

mtimer_t * create_timer();

unsigned int ticks;


void t5_func(void){
	printf("Hello, I am 5.\n");
}

void t10_func(void){
	printf("I am 10.\n");
}

void t20_func(void){
	printf("I am 20.\n");
}


int main()
{
	mtimer_t *t10 = create_timer();// (mtimer_t *)malloc(sizeof(mtimer_t));
	//mtimer_t *t5 = (mtimer_t *)malloc(sizeof(mtimer_t));;
	//mtimer_t *t20 = (mtimer_t *)malloc(sizeof(mtimer_t));;
	
	t10->ticks = 10;
	printf("%p\n", t10);
	ticks = 0;
	t10->callback = t10_func;
	//t5->callback = t5_func;
	//t20->callback = t20_func;

	start_timer(t10, 10);
	//start_timer(t5, 17);
	print();
	//start_timer(t20,15);
	print();
	start();	
}
