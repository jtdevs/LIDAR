#include "LPC214x.H"

int main(void)
{
	IO0DIR &= ~((1 << 21));
	IO1DIR |= (1 << 24);
	while(1)
	{
		if(IO0PIN & (1 << 21))
			IO1SET |= (1 << 24);
		else
			IO1CLR |= (1 << 24);
	}
}
