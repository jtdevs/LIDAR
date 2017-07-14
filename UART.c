#include "LPC214x.H"
#include "DATATYPES.h"
#include "UART.h"

uint8 UART_buff = 0;
void __irq U0IRQ(void){
	UART_buff = U0RBR;
	VICVectAddr = 0x0;
}
void InitUART0 (BOOL fast_mode)
{
	PINSEL0 |= 0x5; // Set P0.0 -> Tx, P0.1 -> Rx
	U0LCR = 0x83; // (10000011) 8 bits, no Parity, 1 Stop bit, DLAB = 1
	if(fast_mode) // 115200bps
	{
		U0DLL = 0x0D; // 13
		U0DLM = 0X00; // 0
		U0FDR = (0X02 << 4) | 0x03; // MULVAL = 2 -> 7:4, DIVADDVAL = 3 -> 3:0
	}
	else // 9600bps
	{
		U0DLL = 0x6E; // 110
		U0DLM = 0x01; // 1
		U0FDR = (0xE << 4) | 0x01; // MULVAL = 15 -> 7:4, DIVADDVAL = 1 -> 3:0
	}
	U0LCR &= 0x03; // Set DLAB=0 to lock MULVAL and DIVADDVAL

	U0IER = 0x01; //Enable RDR interrupt
	/*  ----------------------- Init VIC for UART0 ------------------------- */
	VICIntSelect &= 0xFFFFFFBF; // Setting UART0 as IRQ(Vectored)
	VICVectCntl2 = 0x20 | 6; // Assigning Second Highest Priority Slot to I2C1 and enabling this slot
	VICVectAddr2 = (uint32)U0IRQ; // Storing vector address of I2C1
	VICIntEnable = (1<<6); //Enable I2C1 interrupt NOTE: Writting 0's to this register has no effect
}

#define RDR (1 << 0) // Receiver Data Ready
unsigned char U0Read (void)
{
	while (!(U0LSR & RDR)); // Wait till any data arrives into Rx FIFO
	return U0RBR;
}

#define THRE (1 << 5) // Transmit Holding Register Empty
void U0Write (char data)
{
    while ( !(U0LSR & THRE ) ); // Wait till the THR is empty
    U0THR = data;// Now we can write to the Tx FIFO
}

void U0Write_text (char * msg)
{
	while (*msg)
	{
		U0Write(*msg);
		msg++;
	}
}
