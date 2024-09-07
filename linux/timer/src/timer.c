#include <stdlib.h>
#include "config.h"
#include <stdio.h>
#include <string.h>

mtimer_t *timerList=NULL;

static void set_time(mtimer_t *timer, int value) {
	timer->ticks = getTicks()+value;
}

mtimer_t * create_timer(){
        mtimer_t *temp = (mtimer_t *)malloc(sizeof(mtimer_t));
        bzero(temp, sizeof(mtimer_t));
		temp->ticks = 10;
		printf("temp addr=%p\n", temp);
        return(temp);
}

void decrease_time() {
	if (timerList) timerList->ticks--;
}

void start_timer(mtimer_t *timer, int value) { //insert to timerlist
     mtimer_t *ptr = timerList;
     set_time(timer, value);

     if (ptr == NULL)
	     timerList = timer;
     else {
	mtimer_t *prev = NULL;
	while(ptr !=NULL && (ptr->ticks < timer->ticks)) {
		timer->ticks = timer->ticks - ptr->ticks;
		prev = ptr;
		ptr = ptr->next;
	}
	if (prev == NULL) {//insert to first node
	        timerList->ticks -= timer->ticks;
		    timer->next = timerList;
	       	timerList = timer; 	
	}else{
		//timer->ticks -= prev->ticks;
		timer->next = prev->next;
		prev->next = timer;
		if (ptr!=NULL) ptr->ticks -= timer->ticks;
	}
     }
}

void checkTimerList()
{
	mtimer_t *ptr = timerList;
	mtimer_t *timer;

	while(ptr!=NULL && ptr->ticks <= 0) { //timeout
        	timer = timerList;
		timerList = timerList->next;
		timer->callback();	
		free(timer);
	} 
}

void print()
{
	mtimer_t *ptr=timerList;

	while(ptr!=NULL) {
		printf("%p, tick=%d, callback=%p\n", ptr, ptr->ticks, ptr->callback);
		ptr = ptr->next;
	}
}

