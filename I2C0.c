#include "I2C.h"
#include "UART.h"
#include "TMR.h"

uint8 Status0 = 0x00;
BOOL Status_Flag0 = FALSE;

uint8 j;

void __irq I2C_Status0(void)
{
	Status_Flag0 = TRUE; // Update status flag
	Status0 = I2C0STAT; // Read Status byte
	I2C0CONCLR = 0x28; // Clear STA and SI bits
	VICVectAddr = 0x00; // Acknowledge Interrupt
}

BOOL I2C0_Status(uint8 status_code)
{
	//wait for the interrupt routine to act
	while(Status_Flag0 == FALSE);
	//reset the flag
	Status_Flag0 = FALSE;
	//check if the status is the one expected.
	if(Status0 != status_code)
	  return FALSE;
	else
	  return TRUE;
}

void I2C0_init()
{
	PCONP |= 0x00000080; // Enable power for I2C0 peripheral
	PINSEL0 &= 0xFFFFFF0F; // Clear P0.2 and P0.3 first
	PINSEL0 |= 0x00000050; //Set P0.2 -> SCL P0.3 -> SDA
	I2C0CONCLR = 0x6C; // Clear all I2CON bits
	I2C0SCLH = 0x12C; // 300 PCLK cycles high
	I2C0SCLL = 0x12C; // 300 PCLK cycles high
	I2C0CONSET = 0x40; // Set I2EN bit

	/*  ----------------------- Init VIC for I2C0 ------------------------- */
	VICIntSelect &= 0xFFFFFDFF; // Setting I2C0 as IRQ(Vectored)
	VICVectCntl0 = 0x20 | 9; // Assigning Highest Priority Slot to I2C0 and enabling this slot
	VICVectAddr0 = (uint32)I2C_Status0; // Storing vector address of I2C0
	VICIntEnable = (1<<9); //Enable I2C0 interrupt
}

BOOL I2C0_write(uint8 reg_addr, uint8 data, BOOL debug)
{
	I2C0_Send_Start();
	if(!I2C0_Status(0x08)) // START transmitted and ready for addr
	{
		if(debug)
			U0Write_text("ERROR: Start not received at func BOOL write(uint8 reg_addr, uint8 data)\n\r");
		return FALSE;
	}
	I2C0DAT = 0xC4; // Load addr+w
	if(!I2C0_Status(0x18)) // ACK received and ready for reg_addr
	{
		if(debug)
			U0Write_text("ERROR: Sens_Addr not received at func BOOL write(uint8 reg_addr, uint8 data)\n\r");
		return FALSE;
	}
	I2C0DAT = reg_addr; // Load reg_addr
	if(!I2C0_Status(0x28)) // ACK received and ready for data
	{
		if(debug)
			U0Write_text("ERROR: Reg_Addr note received at func BOOL write(uint8 reg_addr, uint8 data)\n\r");
		return FALSE;
	}
	I2C0DAT = data; // Load data
	if(!I2C0_Status(0x28)) // ACK received and ready next step
	{
		//I2C0_Send_Stop();
		if(debug)
			U0Write_text("ERROR: Data not received at func BOOL write(uint8 reg_addr, uint8 data)\n\r");
		return FALSE;
	}
	I2C0_Send_Stop();
	return TRUE;
}

BOOL I2C0_read(uint8 reg_addr, uint8 *data, BOOL debug)
{
	I2C0_Send_Start();
	if(!I2C0_Status(0x08)) // ACK received and ready for addr
	{
		if(debug)
			U0Write_text("ERROR: Start not received at func BOOL read(uint8 reg_addr, uint8 *data) W\n\r");
		return FALSE;
	}
	I2C0DAT = 0xC4; // Load addr+w
	if(!I2C0_Status(0x18)) // ACK received and ready for reg_addr
	{
		if(debug)
			U0Write_text("ERROR: Sens_Addr not received at func BOOL read(uint8 reg_addr, uint8 *data) W\n\r");
		return FALSE;
	}
	I2C0DAT = reg_addr; // Load reg_addr
	if(!I2C0_Status(0x28)) // ACK received and ready for data
	{
		if(debug)
			U0Write_text("ERROR: Reg_Addr note received at func BOOL read(uint8 reg_addr, uint8 *data) W\n\r");
		return FALSE;
	}
	I2C0_Send_Stop();
	I2C0_Send_Start();
	if(!I2C0_Status(0x08)) // ACK received and ready for addr
	{
		if(debug)
			U0Write_text("ERROR: Start not received at func BOOL read(uint8 reg_addr, uint8 *data) R\n\r");
		return FALSE;
	}
	I2C0DAT = 0xC5; // Load addr+r
	if(!I2C0_Status(0x40)) // ACK received and ready to receive data
	{
		if(debug)
			U0Write_text("ERROR: Sens_Addr not received at func read(uint8 reg_addr, uint8 *data) R\n\r");
		return FALSE;
	}
	I2C0CONCLR=0x04;
	if(!I2C0_Status(0x58)) // NACK received and ready to receive data
	{
		I2C0_Send_Stop();
		if(debug)
			U0Write_text("ERROR: Data not sent at func read(uint8 reg_addr, uint8 *data) R\n\r");
		return FALSE;
	}
	*data = I2C0DAT;
	I2C0_Send_Stop();
	return TRUE;
}
