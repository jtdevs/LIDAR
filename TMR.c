#include "LPC214x.H"
#include "DATATYPES.h"
#include "TMR.h"

void init_clock (void)
{
	PLL0CON = 0x01; // Enable PLL
	PLL0CFG = 0x24; // Multiplier and divider setup
	PLL0FEED = 0xAA; // Feed sequence
	PLL0FEED = 0x55;
	while(!(PLL0STAT & 0x00000400)); // Is locked?
	PLL0CON = 0x03; // Connect PLL after PLL is locked
	PLL0FEED = 0xAA; // Feed sequence
	PLL0FEED = 0x55;
	VPBDIV = 0x01; // PCLK is same as CCLK i.e.60 MHz
}

void init_timer (void)
{
	PCONP |= 0x00000006;
	T0PR = 59;
	TIMER_RESET();
}

void delayMicros (uint32 time)
{
	TIMER_RESET();
	TIMER_ENABLE();
	while(T0TC < time){}
	TIMER_RESET();
	TIMER_DISABLE();
}
