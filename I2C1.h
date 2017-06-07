#ifndef I2C1_H
#define I2C1_H
#endif

#include "LPC214x.H"
#include "DATATYPES.h"

#define I2C1_StartRx() I2C1CONSET = 0x04;
#define I2C1_StartTx() I2C1CONSET = 0x20;
extern void I2C1_init(uint8 addr);
extern BOOL I2C1_byteTx(uint8 address, uint8 byte);
extern BOOL I2C1_byteStreamTx(uint8 address, uint8 *byte_array, uint8 size);
extern BOOL I2C1_byteRx(uint8 *byte);
extern BOOL I2C1_byteStreamRx(uint8 *byte_array);

uint8 target_address;
uint8 buffer[401];
uint16 data_count;
uint16 expected_data;
extern BOOL I2C1_busy;
extern BOOL I2C1_error;

/*
Variables:

I2C_busy 		-> 	This flag will tell you when all data has been transmitted or received,
[READ/WRITE]		or the operation was aborted due to failure.

I2C_error		->	This flag will tell you if the operation ended sucessfully or not. Also
[READ ONLY]			if you count the amount of times it changes from 0 to 1 you can keep track
					of the amount of non fatal errors that may occur. Thus giving you information
					about the quality of the lines.

data_count		->	This variable will hold the amount of data packages to be sent or has been received
[READ/WRITE]		through the port. Keep in mind that the packages are 1[byte] each one, so to send 
					data larger than this, you should separate it in 1[byte] chunks.
					This variable should be a natural number (e.g. to send 8 data packages use a
					value of 8, not 7 i.e. don't take the 0 element into account). Its value should
					not be greater than 400. If an Rx operation is in process its value increments 
					every time you receive a package.

expected_data	->	Usefull only when on Master Receiver mode. As the name goes, it's the amount of data
					expected from the slave.

buffer[401]		->	This array holds the data to be sent or that has been received.
[READ/WRITE]

target_address	->	This variable holds the unique address of the target. If is set to 0x00, then it's
[WRITE ONLY]		the general call address and every device with GC enabled should receive the message.
					The address is 8bits: 7bitADDR+GC_bit.
*/
