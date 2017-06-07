#include <lpc214x.h>

/* Arm data types */
typedef unsigned char 		uint8;
typedef unsigned short int 	uint16;
typedef unsigned int 		uint32;
typedef char 		int8;
typedef short int 	int16;
typedef int 		int32;
typedef unsigned char	BOOL;
/*
Data Type 	Actual ‘C’data type 	Bytes allocated.
uint8 		unsigned char 			1
int8 		char 					1
uint16 		unsigned short 			2
int16 		short 					2
uint32 		unsigned long 			4
int32 		long 					4
float32 	32 bit float 			4
float64 	double 					8
*/

#define	TIMER_RESET()	T0TCR=0X02
#define	TIMER_ENABLE()	T0TCR=0X01
#define	TIMER_DISABLE()	T0TCR=0X00
void init_timer(void);
void init_clock(void);
void delayMicros (uint32 time);

int main(void){

	IO1DIR = (1<<24);   // Configure P1.24 as Output
	while(1){
		IO1CLR = (1<<24); // CLEAR (0) 1.24 to turn LED ON
		delayMicros(5);
		IO1SET = (1<<24); // SET (1) P1.24 to turn LEDs OFF
		delayMicros(5);
	}
}

void init_clock (void)
{
	PLL0CON = 0x01; //Enable PLL
	PLL0CFG = 0x24; //Multiplier and divider setup
	PLL0FEED = 0xAA; //Feed sequence
	PLL0FEED = 0x55;
	while(!(PLL0STAT & 0x00000400)); //is locked?
	PLL0CON = 0x03; //Connect PLL after PLL is locked
	PLL0FEED = 0xAA; //Feed sequence
	PLL0FEED = 0x55;
	VPBDIV = 0x01; // PCLK is same as CCLK i.e.60 MHz
}

void init_timer (void)
{
	PCONP |= 0x00000006;
	T0PR = 59;
	T0TCR = 0X02;
}

void delayMicros (uint32 time)
{
	TIMER_RESET();
	TIMER_ENABLE();
	while(T0TC < time){}
	TIMER_RESET();
	TIMER_DISABLE();
}
