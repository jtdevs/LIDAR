#include "I2C0.h"
#include "UART.h"
#include "TMR.h"

uint8 Status = 0x00;

uint16 i;
uint16 j;
uint8 I2C0_target_address;
uint8 I2C0_buffer[401] = {0};
uint16 I2C0_data_count;
uint16 I2C0_expected_data;
BOOL I2C0_busy = FALSE;
BOOL I2C0_error = FALSE;

void __irq I2C_Status0(void)
{
	switch(I2C0STAT)
	{
	//MASTER TRANSMITTER
		case 0x08: // START condition has been transmitted
			I2C0DAT = I2C0_target_address;
			I2C0CONCLR = 0x18; // clear STO and SI
			i = 0;
			break;
		case 0x10: // A repeated START condtion has been transmitted
			I2C0DAT = I2C0_target_address;
			I2C0CONCLR = 0x18; // Clear STO and SI
			i = 0;
			break;
		case 0x18: // SLA+W has been transmitted, ACK received
			I2C0DAT = I2C0_buffer[i];
			I2C0CONCLR = 0x38; // clear STA, STO and SI
			break;
		case 0x20: // SLA+W has been transmitted, NACK received
			I2C0CONCLR = 0x28; // clear STA and SI
			I2C0CONSET = 0x10; // set STO
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
		case 0x28: // Data byte in I2DAT transmitted. ACK received
			i++;
			if(i < I2C0_data_count)
			{
				I2C0DAT = I2C0_buffer[i];
				I2C0CONCLR = 0x38; // clear STA, STO and SI
			}
			else
			{
				I2C0CONCLR = 0x28; // clear STA and SI
				I2C0CONSET = 0x10; // set STO
				I2C0_busy = FALSE;
			}
			break;
		case 0x30: // Data byte in I2DAT transmitted. NACK received
			I2C0CONCLR = 0x28; // clear STA and SI
			I2C0CONSET = 0x10; // set STO
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
		case 0x38: // Arbitration lost in NACK bit, SLA+R/W or Data bytes
			I2C0CONCLR = 0x38; // clear STA, STO and SI
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
	//MASTER RECEIVER
		case 0x40: // SLA+R has been transmitted. ACK has been received.
			I2C0_data_count = 0;
			I2C0CONCLR = 0x38; // clear STA, STO and SI
			if(I2C0_expected_data == 1)
				I2C0CONCLR = 0x04; // Clr AA
			else
				I2C0CONSET = 0x04; // Set AA
			break;
		case 0x48: // SLA+R has been transmitted. NACK has been received.
			I2C0CONCLR = 0x28; // clear STA and SI
			I2C0CONSET = 0x10; // set STO
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
		case 0x50: // Data byte has been received. ACK has been returned.
			I2C0_buffer[I2C0_data_count] = I2C0DAT;
			I2C0_data_count++;
			if(I2C0_data_count+1 == I2C0_expected_data)	//	Don't acknowledge the last data bit
				I2C0CONCLR = 0x3C; // clear STA, STO SI and AA
			else
			{
				I2C0CONCLR = 0x38; // clear STA, STO and SI
				I2C0CONSET = 0x04; // Set AA
			}
			break;
		case 0x58: // Data byte has been received. NACK has been returned.
			if(I2C0_data_count+1 == I2C0_expected_data)// If this is the last bit it's okay to return a NACK
			{
				I2C0_buffer[I2C0_data_count] = I2C0DAT;
				I2C0_data_count++;
			}
			I2C0CONCLR = 0x28; // clear STA and SI
			I2C0CONSET = 0x10; // set STO
			if(I2C0_data_count < I2C0_expected_data)
				I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
	//SLAVE RECEIVER
		case 0x60: // Addressed with own SLA+W. ACK returned
			I2C0CONCLR = 0x18; // Clear STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_data_count = 0;
			break;
		case 0x68: // Arbitration lost during SLA+R/W as a Master. Own SLA+W received. ACK returned.
			I2C0CONCLR = 0x1C; // Clear STO, SI and AA
			I2C0_error = TRUE;
			break;
		case 0x70: // Addressed with GC addr. ACK returned
			I2C0CONCLR = 0x18; // Clear STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_data_count = 0;
			break;
		case 0x78: // Arbitration lost during SLA+R/W as a Master. GC received. ACK returned.
			I2C0CONCLR = 0x1C; // Clear STO, SI and AA
			I2C0_error = TRUE;
			break;
		case 0x80: // Previously addressed with own SLV addr. Data received. ACK returned.
			I2C0_buffer[I2C0_data_count] = I2C0DAT;
			I2C0_data_count++;
			I2C0CONCLR = 0x18; // Clear STO and SI
			I2C0CONSET = 0x04; // Set AA
			break;
		case 0x88: // Previously addressed with own SLV addr. Data received. NACK returned.
			I2C0CONCLR = 0x38; // clear STA, STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_error = TRUE;
			break;
		case 0x90: // Previously addressed with GC addr. Data received. ACK returned.
			I2C0_buffer[I2C0_data_count] = I2C0DAT;
			I2C0_data_count++;
			I2C0CONCLR = 0x18; // Clear STO and SI
			I2C0CONSET = 0x04; // Set AA
			break;
		case 0x98: // Previously addressed with GC addr. Data received. NACK returned.
			I2C0CONCLR = 0x38; // clear STA, STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_error = TRUE;
			break;
		case 0xA0: // STOP condition or repeated START condition received.
			I2C0CONCLR = 0x3C; // Clear STA, STO, SI and AA
			I2C0_busy = FALSE;
			break;
	//SLAVE TRANSMITTER
		case 0xA8: // Own SLA+R received. ACK returned
			i = 0;
			I2C0DAT = I2C0_buffer[i];
			if(I2C0_data_count == 1)
				I2C0CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C0CONCLR = 0x18; // Clear STO and SI
				I2C0CONSET = 0x04; // Set AA
			}
			break;
		case 0xB0: // Arbitration lost in SLA+R/W as master. Own SLA+R received. ACK returned
			i = 0;
			I2C0DAT = I2C0_buffer[i];
			if(I2C0_data_count == 1) // if its the last byte
				I2C0CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C0CONCLR = 0x18; // Clear STO and SI
				I2C0CONSET = 0x04; // Set AA
			}
			I2C0_error = TRUE;
			break;
		case 0xB8: // Data transmitted. ACK received.
			i++;
			I2C0DAT = I2C0_buffer[i];
			if(I2C0_data_count == i+1) // if its the last byte
				I2C0CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C0CONCLR = 0x18; // Clear STO and SI
				I2C0CONSET = 0x04; // Set AA
			}
			break;
		case 0xC0: // Data transmitted. NACK received.
			if(i < I2C0_data_count) // If it's the last byte, then it's ok
				I2C0_error = TRUE;
			I2C0CONCLR = 0x38; // Clear STA, STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_busy = FALSE;
			break;
		case 0xC8: // Last byte transmitted. ACK received.
			I2C0CONCLR = 0x38; // Clear STA, STO and SI
			I2C0CONSET = 0x04; // Set AA
			I2C0_busy = FALSE;
			break;
	//MISCELLANEOUS STATES
		case 0xF8: // Nothing to do, just don't go to default. SI not set yet.
			break;
		case 0x00: // Bus error during serial transfer.
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			I2C0CONSET = 0x10; // Set STO
			I2C0CONCLR = 0x08; // Clear SI
			break;
		default:
			I2C0CONCLR = 0x08; // Clear SI
			I2C0_error = TRUE;
			I2C0_busy = FALSE;
			break;
	}
	VICVectAddr = 0x00; // Acknowledge Interrupt
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
BOOL sensorReset()
{
	return I2C0_write(0x00,0x00);
} 
BOOL setZero()
{
	return I2C0_write(0x13,256-OFFSET);
}
BOOL getDistance(uint8 *LSB, uint8 *MSB)
{
	uint8 busy = 0x01;
	uint8* pbusy = &busy;
	if(!I2C0_write(0x00,0x004)){
		return FALSE;
	}
	while(busy & 0x01){
		if(!I2C0_read(0x01, pbusy)){
			return FALSE;
		}
	}
	if(!I2C0_read(0xf, MSB)){
		return FALSE;
	}
	if(!I2C0_read(0x10, LSB)){
		return FALSE;
	}
	return TRUE;
}

BOOL I2C0_read(uint8 reg_addr, uint8 *data)
{
	I2C0_target_address = 0xC4;
	I2C0_data_count = 1;
	I2C0_buffer[0] = reg_addr;
	I2C0_error = FALSE;
	I2C0_busy = TRUE;
	I2C0_StartTx();
	while(I2C0_busy);
	if(I2C0_error)
		return FALSE;
	I2C0_target_address = 0xC5;
	I2C0_expected_data = 1;
	I2C0_error = FALSE;
	I2C0_busy = TRUE;
	I2C0_StartTx();
	while(I2C0_busy);
	if(I2C0_error)
		return FALSE;
	*data = I2C0_buffer[0];
	return TRUE;
}

BOOL I2C0_write(uint8 reg_addr, uint8 data)
{
	I2C0_target_address = 0xC4; // Load LIDARaddr+w
	I2C0_data_count = 2;
	I2C0_buffer[0] = reg_addr;
	I2C0_buffer[1] = data;
	I2C0_error = FALSE;
	I2C0_busy = TRUE;
	I2C0_StartTx();
	while(I2C0_busy);
	if(I2C0_error)
		return FALSE;
	else
		return TRUE;
}
