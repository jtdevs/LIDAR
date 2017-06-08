#ifndef I2C0_H
#define I2C0_H
#endif

#include "LPC214x.H"
#include "DATATYPES.h"

#define OFFSET 11
#define I2C0_StartRx() I2C0CONSET = 0x04;
#define I2C0_StartTx() I2C0CONSET = 0x20;
BOOL I2C0_read(uint8 reg_addr, uint8 *data);
BOOL I2C0_write(uint8 reg_addr, uint8 data);
extern BOOL sensorReset(void);
extern BOOL setZero(void);
extern void I2C0_init(void);
extern BOOL getDistance(uint8 *LSB, uint8 *MSB);
extern BOOL I2C0_busy;
extern BOOL I2C0_error;
