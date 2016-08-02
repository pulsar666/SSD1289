#include "lpc_types.h"
void TIMER0_IRQHandler (void);
uint32_t getPClock (uint32_t);
uint32_t converUSecToVal (uint32_t, uint32_t);
void Timer0_init(uint32_t);
void delay_ms(unsigned int);
