#ifndef LIDARLITE_H
#define LIDARLITE_H

#include "LPC214x.H"
#include "DATATYPES.h"

//IMPORTANT: These are on GPIO port 0
#define DIR_PIN 5
#define STEP_PIN 7
#define SLP_PIN 9
#define RST_PIN 13
#define ZERO_PIN 21

//IMPORTANT: These are on GPIO port 1
#define EN_PIN 22
#define MS1_PIN 20
#define MS2_PIN 18
#define MS3_PIN 16
#define DBG_PIN 23
#define MS_PIN 25

#define Init_LED() IO1DIR |= (1 << 24);
#define LED_OFF() IO1SET |= (1 << 24);
#define LED_ON() IO1CLR |= (1 << 24);

extern void sensorInit(void);
extern void sendMap(uint8 *LSBmap, uint8 *MSBmap, uint8 step_div, BOOL protocol);
extern BOOL external_request(uint8 *LSBmap, uint8 *MSBmap, uint8 step_divider, uint8 addr);
extern void updateRange(uint8 *LSBmap, uint8 *MSBmap, uint8 step_div);
extern void motor_init(void);
extern void motor_step(BOOL dir);
extern void change_step(uint8 microstep);
void init_inputs(void);
BOOL zero_stat(void);
BOOL dbg_stat(void);
BOOL ms_stat(void);
#endif
