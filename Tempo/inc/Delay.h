#ifndef __DELAY_H
#define __DELAY_H

extern void InitDelay();
extern void DelayMs(uint32_t Ms);
extern void DelayUs(uint16_t Us);
extern void SysTyck_Handler(void);

#endif /* end __TIMER_H */
