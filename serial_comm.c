#include <msp430.h>
#define TXD BIT2
#define RXD BIT1
/*
 * main.c
 */
void main(void) {

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1SEL |= TXD + RXD;
    P1SEL2 |= TXD + RXD;	// select special function UART
    P1OUT &= 0x00;		// shut down unused pins and consume power

    BCSCTL1 = CALBC1_1MHZ;	// set DCO
    DCOCTL = CALDCO_1MHZ;

    UCA0CTL1 |= UCSSEL_2;	// select SMCLK
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;	// set baud rate 9600

    UCA0CTL1 &= ~UCSWRST;	// initialize

    IFG2 &= UCA0TXIFG;
    IE2 =  UCA0RXIE;	// enable interrupt
    __enable_interrupt();
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void RX_ISR(void){

	while (!(IFG2 & UCA0TXIFG));{

		UCA0TXBUF = UCA0RXBUF;

		__delay_cycles(30);
	}
}
