#ifndef TIMER_H_
#define TIMER_H_

#define PIT_COMMREG 0x43
#define PIT_DATAREG0 0x40

extern unsigned short read_PIT_count();

void initTimer();

void tick(void);

unsigned long int time(void);

void sleep(unsigned int i);

#endif
