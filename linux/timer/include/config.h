#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct timer {
	int ticks;
	void (*callback)(void);
	struct timer *next;
} mtimer_t;

#endif
