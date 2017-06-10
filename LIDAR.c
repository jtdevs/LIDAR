#include "LIDAR.h"
#include "I2C0.h"
#include "I2C1.h"
#include "TMR.h"
#include "UART.h"

/* ------------------------------------------------ LiDAR related ------------------------------------------------ */
BOOL read_error = FALSE;
BOOL direction = TRUE;
uint8 error_counter = 0;
uint16 k;

void sensorInit()
{
	read_error = FALSE;
	error_counter = 0;
	while(!read_error)
	{
		if(error_counter > 10)
			IO1CLR = (1 << 24);
		read_error = sensorReset();
		error_counter++;
	}
	read_error = FALSE;
	error_counter = 0;
	delayMicros(100000);
	while(!read_error)
	{
		if(error_counter > 10)
			IO1CLR = (1 << 24);
		read_error = setZero();
		error_counter++;
	}
	delayMicros(100000);
	read_error = FALSE;
	error_counter = 0;
}

void sendMap(uint8 *LSBmap, uint8 *MSBmap, uint8 step_div, BOOL protocol)
{
	if(protocol)
	{
		for(k = 0; k <(step_div*100)+1; k++)
		{
			U0Write(MSBmap[k]);
			U0Write(LSBmap[k]);
		}
	}
	else
	{
		for(k = 0; k <(step_div*100)+1; k++)
		{
			I2C1_byteTx(0x02, MSBmap[k]);
			I2C1_byteTx(0x02, LSBmap[k]);
		}
	}
}

BOOL external_request(uint8 *LSBmap, uint8 *MSBmap, uint8 step_divider, uint8 addr)
{
	uint8 count = 0;
	I2C1_byteTx(addr<<1, 0x04);//TO_DO: just try 10 times and give up
	while(count < (step_divider*100)+1)
	{
		I2C1_byteRx(&MSBmap[count]);
		I2C1_byteRx(&LSBmap[count]);
		count++;
	}
	return TRUE;
}

void updateRange(uint8 *LSBmap, uint8 *MSBmap, uint8 step_div){
	IO1CLR |= (1 << EN_PIN); //Enable motor
	delayMicros(500);// wait for the FET's to energyze
	if(direction)
	{
		//the zero pin is pulled up when open
		while(zero_stat())
		{
			delayMicros(1000);
			motor_step(FALSE);//counter clockwise
		}
		delayMicros(1000);
		motor_step(TRUE);
		read_error = FALSE;
		error_counter = 0;
		while(!read_error)
		{
			if(error_counter > 10)
				IO1CLR = (1 << 24);
			read_error = getDistance(&LSBmap[0], &MSBmap[0]);
			error_counter++;
		}
		error_counter = 0;
		read_error = FALSE;
		IO1SET = (1 << 24);
		for(k = 1; k <(step_div*100)+1; k++)
		{
			motor_step(direction);
			while(!read_error)
			{
				if(error_counter > 10)
					IO1CLR = (1 << 24);
				read_error = getDistance(&LSBmap[k], &MSBmap[k]);
				error_counter++;
			}
			error_counter = 0;
			read_error = FALSE;
			IO1SET = (1 << 24);
		}
	}
	else{
		for(k = (step_div*100); k > 0; k--)
		{
			while(!read_error)
			{
				if(error_counter > 10)
					IO1CLR = (1 << 24);
				read_error = getDistance(&LSBmap[k], &MSBmap[k]);
				error_counter++;
			}
			error_counter = 0;
			read_error = FALSE;
			IO1SET = (1 << 24);
			motor_step(direction);
		}
		while(!read_error)
		{
			if(error_counter > 10)
				IO1CLR = (1 << 24);
			read_error = getDistance(&LSBmap[0], &MSBmap[0]);
			error_counter++;
		}
		error_counter = 0;
		read_error = FALSE;
		IO1SET = (1 << 24);
	}
	direction = !direction;
	IO1SET |= (1 << EN_PIN); //Disable motor
}

void motor_init()
{
	//Set all control pins as GPIO
	PINSEL0 &= 0xF3F333FF;
	PINSEL2 &= 0xFFFFFFFB;//this sets pins 1.16...1.25 as GPIO (bit 3 = 0), but at the expense of LPC214x TRACE functionality.
	//Set all the control pins as OUTPUT
	IO0DIR |= (1 << DIR_PIN) | (1 << STEP_PIN) | (1 << RST_PIN) | (1 << SLP_PIN);
	IO1DIR |= (1 << EN_PIN) | (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);
	//RST and SLP pin must be held HIGH for normal operation
	IO0SET |= (1 << RST_PIN) | (1 << SLP_PIN);
	//EN pin must be held LOW for normal operation. Set full step as default -> MS1 = MS2 = MS3 = LOW
	IO1CLR |= (1 << EN_PIN) | (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);
	IO1SET |= (1 << EN_PIN); //Disable motor
}

void change_step(uint8 microstep)
{
	IO0CLR |= (1 << RST_PIN);
	switch(microstep)
	{
		case 1:
			IO1CLR |= (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);
			break;
			
		case 2:
			IO1CLR |= (1 << MS2_PIN) | (1 << MS3_PIN);
			IO1SET |= (1 << MS1_PIN);
			break;

		case 4:
			IO1CLR |= (1 << MS1_PIN) | (1 << MS3_PIN);
			IO1SET |= (1 << MS2_PIN);
			break;

		case 8:
			IO1CLR |= (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);
			break;

		case 16:
			IO1CLR |= (1 << MS3_PIN);
			IO1SET |= (1 << MS1_PIN) | (1 << MS2_PIN) ;
			break;

		default:
			IO1SET |= (1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN);
			break;
	}
	IO0SET |= (1 << RST_PIN);
}

void motor_step(BOOL dir)
{
	if(dir)
		IO0SET |= (1 << DIR_PIN);
	else
		IO0CLR |= (1 << DIR_PIN);
	IO0CLR |= (1 << STEP_PIN);
	delayMicros(500);
	IO0SET |= (1 << STEP_PIN);
	delayMicros(500);
	IO0CLR |= (1 << STEP_PIN);
	delayMicros(500);
}

void init_inputs()
{
	PINSEL1 &= 0xFFFFF3FF;
	PINSEL2 &= 0xFFFFFFFB;
	IO0DIR &= ~((1 << ZERO_PIN));
	IO1DIR &= ~((1 << DBG_PIN || 1 << MS_PIN)); //select them as inputs
}

BOOL zero_stat()
{
	if(IO0PIN & (1 << ZERO_PIN))
		return TRUE;
	else
		return FALSE;
}

BOOL dbg_stat()
{

	if(IO1PIN & (1 << DBG_PIN))
		return TRUE;
	else
		return FALSE;
}

BOOL ms_stat()
{
	if(IO1PIN & (1 << MS_PIN))
		return TRUE;
	else
		return FALSE;
}
