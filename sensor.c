#include "LPC214x.H"
#include "DATATYPES.h"
#include "UART.h"
#include "I2C0.h"
#include "TMR.h"
int main(void)
{
	uint8 MSB = 10;
	uint8 LSB = 10;
	init_clock();
	init_timer();
	I2C0_init(); // Address 0x60. No GC response
	InitUART0(FALSE);
	while(sensorReset());
	delayMicros(10000);
	while(!setZero());
	U0Write(MSB);
	U0Write(LSB);
	while(TRUE)
	{
		getDistance(&LSB, &MSB);
		U0Write(MSB);
		U0Write(LSB);
		delayMicros(5000);
	}
}
