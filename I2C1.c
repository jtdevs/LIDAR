#include "I2C1.h"

uint16 i;
uint16 j;
uint8 target_address;
uint8 buffer[401] = {0};
uint16 data_count;
uint16 expected_data;
BOOL I2C1_busy = FALSE;
BOOL I2C1_error = FALSE;

void __irq I2C_Status1(void)
{
	switch(I2C1STAT)
	{
//MASTER TRANSMITTER
		case 0x08: // START condition has been transmitted
			I2C1DAT = target_address;
			I2C1CONCLR = 0x18; // clear STO and SI
			i = 0;
			break;
		case 0x10: // A repeated START condtion has been transmitted
			I2C1DAT = target_address;
			I2C1CONCLR = 0x18; // Clear STO and SI
			i = 0;
			break;
		case 0x18: // SLA+W has been transmitted, ACK received
			I2C1DAT = buffer[i];
			I2C1CONCLR = 0x38; // clear STA, STO and SI
			break;
		case 0x20: // SLA+W has been transmitted, NACK received
			I2C1CONCLR = 0x28; // clear STA and SI
			I2C1CONSET = 0x10; // set STO
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
		case 0x28: // Data byte in I2DAT transmitted. ACK received
			i++;
			if(i < data_count)
			{
				I2C1DAT = buffer[i];
				I2C1CONCLR = 0x38; // clear STA, STO and SI
			}
			else
			{
				I2C1CONCLR = 0x28; // clear STA and SI
				I2C1CONSET = 0x10; // set STO
				I2C1_busy = FALSE;
			}
			break;
		case 0x30: // Data byte in I2DAT transmitted. NACK received
			I2C1CONCLR = 0x28; // clear STA and SI
			I2C1CONSET = 0x10; // set STO
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
		case 0x38: // Arbitration lost in NACK bit, SLA+R/W or Data bytes
			I2C1CONCLR = 0x38; // clear STA, STO and SI
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
//MASTER RECEIVER
		case 0x40: // SLA+R has been transmitted. ACK has been received.
			data_count = 0;
			I2C1CONCLR = 0x38; // clear STA, STO and SI
			if(expected_data == 1)
				I2C1CONCLR = 0x04; // Clr AA
			else
				I2C1CONSET = 0x04; // Set AA
			break;
		case 0x48: // SLA+R has been transmitted. NACK has been received.
			I2C1CONCLR = 0x28; // clear STA and SI
			I2C1CONSET = 0x10; // set STO
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
		case 0x50: // Data byte has been received. ACK has been returned.
			buffer[data_count] = I2C1DAT;
			data_count++;
			if(data_count+1 == expected_data)	//	Don't acknowledge the last data bit
				I2C1CONCLR = 0x3C; // clear STA, STO SI and AA
			else
			{
				I2C1CONCLR = 0x38; // clear STA, STO and SI
				I2C1CONSET = 0x04; // Set AA
			}
			break;
		case 0x58: // Data byte has been received. NACK has been returned.
			if(data_count+1 == expected_data)// If this is the last bit it's okay to return a NACK
			{
				buffer[data_count] = I2C1DAT;
				data_count++;
			}
			I2C1CONCLR = 0x28; // clear STA and SI
			I2C1CONSET = 0x10; // set STO
			if(data_count < expected_data)
				I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
//SLAVE RECEIVER
		case 0x60: // Addressed with own SLA+W. ACK returned
			I2C1CONCLR = 0x18; // Clear STO and SI
			I2C1CONSET = 0x04; // Set AA
			data_count = 0;
			break;
		case 0x68: // Arbitration lost during SLA+R/W as a Master. Own SLA+W received. ACK returned.
			I2C1CONCLR = 0x1C; // Clear STO, SI and AA
			I2C1_error = TRUE;
			break;
		case 0x70: // Addressed with GC addr. ACK returned
			I2C1CONCLR = 0x18; // Clear STO and SI
			I2C1CONSET = 0x04; // Set AA
			data_count = 0;
			break;
		case 0x78: // Arbitration lost during SLA+R/W as a Master. GC received. ACK returned.
			I2C1CONCLR = 0x1C; // Clear STO, SI and AA
			I2C1_error = TRUE;
			break;
		case 0x80: // Previously addressed with own SLV addr. Data received. ACK returned.
			buffer[data_count] = I2C1DAT;
			data_count++;
			I2C1CONCLR = 0x18; // Clear STO and SI
			I2C1CONSET = 0x04; // Set AA
			break;
		case 0x88: // Previously addressed with own SLV addr. Data received. NACK returned.
			I2C1CONCLR = 0x38; // clear STA, STO and SI
			I2C1CONSET = 0x04; // Set AA
			I2C1_error = TRUE;
			break;
		case 0x90: // Previously addressed with GC addr. Data received. ACK returned.
			buffer[data_count] = I2C1DAT;
			data_count++;
			I2C1CONCLR = 0x18; // Clear STO and SI
			I2C1CONSET = 0x04; // Set AA
			break;
		case 0x98: // Previously addressed with GC addr. Data received. NACK returned.
			I2C1CONCLR = 0x38; // clear STA, STO and SI
			I2C1CONSET = 0x04; // Set AA
			I2C1_error = TRUE;
			break;
		case 0xA0: // STOP condition or repeated START condition received.
			I2C1CONCLR = 0x3C; // Clear STA, STO, SI and AA
			I2C1_busy = FALSE;
			break;
//SLAVE TRANSMITTER
		case 0xA8: // Own SLA+R received. ACK returned
			i = 0;
			I2C1DAT = buffer[i];
			if(data_count == 1)
				I2C1CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C1CONCLR = 0x18; // Clear STO and SI
				I2C1CONSET = 0x04; // Set AA
			}
			break;
		case 0xB0: // Arbitration lost in SLA+R/W as master. Own SLA+R received. ACK returned
			i = 0;
			I2C1DAT = buffer[i];
			if(data_count == 1) // if its the last byte
				I2C1CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C1CONCLR = 0x18; // Clear STO and SI
				I2C1CONSET = 0x04; // Set AA
			}
			I2C1_error = TRUE;
			break;
		case 0xB8: // Data transmitted. ACK received.
			i++;
			I2C1DAT = buffer[i];
			if(data_count == i+1) // if its the last byte
				I2C1CONCLR = 0x1C; // Clear STO, SI and AA
			else
			{
				I2C1CONCLR = 0x18; // Clear STO and SI
				I2C1CONSET = 0x04; // Set AA
			}
			break;
		case 0xC0: // Data transmitted. NACK received.
			if(i < data_count) // If it's the last byte, then it's ok
				I2C1_error = TRUE;
			I2C1CONCLR = 0x38; // Clear STA, STO and SI
			I2C1CONSET = 0x04; // Set AA
			I2C1_busy = FALSE;
			break;
		case 0xC8: // Last byte transmitted. ACK received.
			I2C1CONCLR = 0x38; // Clear STA, STO and SI
			I2C1CONSET = 0x04; // Set AA
			I2C1_busy = FALSE;
			break;
//MISCELLANEOUS STATES
		case 0xF8: // Nothing to do, just don't go to default. SI not set yet.
			break;
		case 0x00: // Bus error during serial transfer.
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			I2C1CONSET = 0x10; // Set STO
			I2C1CONCLR = 0x08; // Clear SI
			break;
		default:
			I2C1CONCLR = 0x08; // Clear SI
			I2C1_error = TRUE;
			I2C1_busy = FALSE;
			break;
	}
	VICVectAddr = 0x00; // Acknowledge Interrupt
}

void I2C1_init(uint8 addr)
{
	PCONP |= 0x80000; // Enable power for I2C1 peripheral
	PINSEL0 &= 0xCF3FFFFF; // Clear P0.11 and P0.14 first
	PINSEL0 |= 0x30C00000; //Set P0.11 -> SCL1 , P0.14 -> SDA1
	PINSEL2 &= 0xFFFFFFF3; //disable trace and debug functionalities NOT SURE
	I2C1CONCLR = 0x6C; // Clear all I2CON bits
	I2C1SCLH = 0x12C; // 300 PCLK cycles high
	I2C1SCLL = 0x12C; // 300 PCLK cycles high
	I2C1CONSET = 0x40; // Set I2EN bit
	I2C1CONCLR = 0x3C;
	I2C1ADR = addr;
	/*  ----------------------- Init VIC for I2C1 ------------------------- */
	VICIntSelect &= 0xFFF7FFFF; // Setting I2C1 as IRQ(Vectored)
	VICVectCntl1 = 0x20 | 19; // Assigning Second Highest Priority Slot to I2C1 and enabling this slot
	VICVectAddr1 = (uint32)I2C_Status1; // Storing vector address of I2C1
	VICIntEnable = (1<<19); //Enable I2C1 interrupt NOTE: Writting 0's to this register has no effect
}

BOOL I2C1_byteTx(uint8 address, uint8 byte)
{
	target_address = address;
	data_count = 1;
	buffer[0] = byte;
	I2C1_error = FALSE;
	I2C1_busy = TRUE;
	I2C1_StartTx();
	while(I2C1_busy);
	if(I2C1_error)
		return FALSE;
	else
		return TRUE;
}

BOOL I2C1_byteStreamTx(uint8 address, uint8 *byte_array, uint8 size)
{
	target_address = address;
	data_count = size;
	for(j=0; j<size; j++)
		buffer[j]=byte_array[j];
	I2C1_error = FALSE;
	I2C1_busy = TRUE;
	I2C1_StartTx();
	while(I2C1_busy);
	if(I2C1_error)
		return FALSE;
	else
		return TRUE;
}

BOOL I2C1_byteRx(uint8 *byte)
{
	I2C1_error = FALSE;
	I2C1_busy = TRUE;
	I2C1_StartRx();
	while(I2C1_busy);
	*byte = buffer[0];
	if(I2C1_error)
		return FALSE;
	else
		return TRUE;
}

BOOL I2C1_byteStreamRx(uint8 *byte_array)
{
	I2C1_error = FALSE;
	I2C1_busy = TRUE;
	I2C1_StartRx();
	while(I2C1_busy);
	for(j=0; j<data_count; j++)
		byte_array[j] = buffer[j];
	if(I2C1_error)
		return FALSE;
	else
		return TRUE;
}
