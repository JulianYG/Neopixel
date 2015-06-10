#include <msp430.h>
#include "stdint.h"

void write_ws2811(uint8_t *data, unsigned length, uint8_t pinmask);

static uint8_t z[5 * 3];

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD; 					// No watchdog reset
	BCSCTL1 = CALBC1_16MHZ;						// Run at 16 MHz
	DCOCTL  = CALDCO_16MHZ;

	P1OUT = 0;
	P1DIR = BIT7;

	unsigned x;

	unsigned int green = 0;
	unsigned int red = 0;
	unsigned int blue = 255;

	for(;;) {

		for(x = 0; x < 5 * 3;) {
			
			z[x++] = green;
			z[x++] = red;
			z[x++] = blue;

			write_ws2811(z, sizeof(z), BIT7);
		}
	}
}
