#include "LPC214x.H"
#include "LIDAR.h"
#include "DATATYPES.h"
#include "I2C.h"
#include "I2C1.h"
#include "TMR.h"
#include "UART.h"

int main(void)
{
	//variables
	uint8 LSBmap[401];//assume the worst case and allocate space for 1/16th steps
	uint8 MSBmap[401];
	uint8 input;
	uint8 *pinput;
	BOOL debug;
	BOOL ms;
	uint8 step_divider = 1;
	//system control inputs
	init_inputs();
	debug = dbg_stat();
	//debug = FALSE;
	//ms = FALSE;
	ms = ms_stat();
	//User feedback led
	Init_LED();
	LED_ON();
	//timing related
	init_clock(); //set the clock to 60MHz
	init_timer(); // Initialize Timer
	//Communications
	InitUART0(FALSE); // Initialize UART0
	I2C0_init(); // Initialize I2C0
	I2C1_init(0xA1);
	//LIDAR and motor initialization
	motor_init();
	sensorReset(debug);
	delayMicros(100000);
	setZero(debug);
	LED_OFF();

	if(ms == TRUE)
	{
		LED_ON();
		delayMicros(500000);//half a second
		LED_OFF();
		delayMicros(500000);//half a second
		LED_ON();
		delayMicros(500000);//half a second
		LED_OFF();
		while(TRUE)
		{
			input = U0Read();
			switch(input)
			{
				case 0x00:
					change_step(1);
					step_divider = 1;
					break;

				case 0x01:
					change_step(2);
					step_divider = 2;
					break;

				case 0x02:
					change_step(4);
					step_divider = 4;
					break;

				case 0x03:
					LED_ON();
					delayMicros(500000);//half a second
					LED_OFF();
					break;

				case 0x04:
					updateRange(LSBmap, MSBmap, debug, step_divider);
					break;

				case 0x05:
					IO1SET |= (1 << EN_PIN); //Disable motor
					break;

				case 0x06:
					IO1CLR |= (1 << EN_PIN); //Enable motor
					delayMicros(500);
					motor_step(TRUE); // Turn Right
					break;

				case 0x07:
					IO1CLR |= (1 << EN_PIN); //Enable motor
					delayMicros(500);
					motor_step(FALSE); // Turn Left
					break;
				case 0x08:
					//send a scan request, wait for the incoming data
			}
		}
	}
	else
	{
		LED_ON();
		delayMicros(500000);//half a second
		LED_OFF();
		delayMicros(500000);//half a second
		LED_ON();
		delayMicros(500000);//half a second
		LED_OFF();
		delayMicros(500000);//half a second
		LED_ON();
		delayMicros(500000);//half a second
		LED_OFF();
		while(TRUE)
		{
			I2C1_byteRx(pinput);
			switch(input)
			{
				//change step, echo scan, continuous scan
				case 0x00:
					change_step(1);
					step_divider = 1;
					break;
				case 0x01:
					change_step(2);
					step_divider = 2;
					break;
				case 0x02:
					change_step(4);
					step_divider = 4;
					break;
				case 0x03:
					LED_ON();
					delayMicros(500000);//half a second
					LED_OFF();
					break;
				case 0x04:
					updateRange(LSBmap, MSBmap, debug, step_divider);
					break;
				case 0x05:
					IO1SET |= (1 << EN_PIN); //Disable motor
					break;
				case 0x06:
					IO1CLR |= (1 << EN_PIN); //Enable motor
					delayMicros(500);
					motor_step(TRUE); // Turn Right
					break;
				case 0x07:
					IO1CLR |= (1 << EN_PIN); //Enable motor
					delayMicros(500);
					motor_step(FALSE); // Turn Left
					break;
				case 0x08:
					//take a measurement and send data
					break;
				default:
					break;
			}
		}
	}
}
//Para efectos de prueba se dejaran las direcciones constantes por ahora.
//******testear el correcto funcionamiento de todos los pines de interes.