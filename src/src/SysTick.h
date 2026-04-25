#ifndef __SYSTICK_H
#define __SYSTICK_H

struct timer {
    int cnt;
};

void SysTickInit(void);
void SysTick_Handler(void);
void timer_init(struct timer *t);
unsigned int timer_expired(struct timer *t, int dlyms);

#endif
