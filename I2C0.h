#ifndef I2C_H
#define I2C_H

#include "LPC214x.H"
#include "DATATYPES.h"

#define I2C0_Send_Start() I2C0CONSET = 0x20;
#define I2C0_Send_Stop() I2C0CONSET = 0x10;
extern BOOL I2C0_Status(uint8 status_code);
extern void I2C0_init(void);
extern BOOL I2C0_write(uint8 reg_addr, uint8 data, BOOL debug);
extern BOOL I2C0_read(uint8 reg_addr, uint8 *data, BOOL debug);

#endif
