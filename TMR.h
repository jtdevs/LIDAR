#ifndef TIMER_H
#define TIMER_H

#define	TIMER_RESET() T0TCR=0X02
#define	TIMER_ENABLE() T0TCR=0X01
#define	TIMER_DISABLE() T0TCR=0X00
extern void init_timer(void);
extern void init_clock(void);
extern void delayMicros(uint32 time);

#endif
