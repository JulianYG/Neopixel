#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "math.h"
#include <stdlib.h>
#define NEO BIT7
#define TXD BIT2
#define RXD BIT1


int atoi(const char * str);
int str_cmp(char *s1, char *s2);
void set(int pixel,int g,int r,int b);
void blackk(void);	// can set the argument as one input function; so that can black out whenever what function is running.
void fade_off(void);
void goo(void);
void fade_on(void);
void blink(void);
void noblink(void);
void flow_rainbow(void);
void write_ws2811(uint8_t *data, unsigned length, uint8_t pinmask);
void uart_config(void);
void command_config(int *array);
static uint8_t z[5 * 3];
signed char command[];	//problem is here: didn't define the length of array!!
int char_cnt;
unsigned x = 0;	//address the same as command[5] but doesn't matter much

/*	If type in "AC", it will clean everything and reset command input.
 *	Answers to Questions:
 *
 *	a. In general, serial communication means sending data one bit at a time, 
 *  as opposed to parallel communication, which sends several bits at one time to 
 *	some paralleled channels. UART protocol is asynchronous, which means data can 
 *	be transmitted intermittently, instead of following the stream clocked by external source.
 *	This suggests that I do not need a particular clock speed when communicating via UART.
 */

void main(void)
{
 	WDTCTL = WDTPW + WDTHOLD; 					// No watchdog reset
	BCSCTL1 = CALBC1_16MHZ;						// Run at 16 MHz
	DCOCTL  = CALDCO_16MHZ;

	P1OUT = 0;
	P1DIR |= NEO;

	uart_config();

	int *comma;

	comma = malloc(2*sizeof(int));	// to protect base address overlap

	for(;;){
		command_config(comma);
	}
}

void uart_config(void){

	P1SEL |= TXD + RXD;
    P1SEL2 |= TXD + RXD;	// select special function UART

    UCA0CTL1 |= UCSSEL_2;	// select SMCLK
    UCA0BR0 = 131;
    UCA0BR1 = 6;
    UCA0MCTL = UCBRS0;	// set baud rate 9600

    UCA0CTL1 &= ~UCSWRST;	// initialize

    IFG2 &= UCA0TXIFG;
    IE2 =  UCA0RXIE;	// enable interrupt

	char_cnt = 0;

    __enable_interrupt();
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void RX_ISR(void){

	while (!(IFG2 & UCA0TXIFG));{

		command[char_cnt] = UCA0RXBUF;

		UCA0TXBUF = command[char_cnt];

		char_cnt = char_cnt + 1;

		__delay_cycles(30);
	}
}

void command_config(int *comma){

	int pointer;
	int pin;

	char str1[3];
	char str2[3];
	char str3[3];

	for (pointer = 0; pointer < char_cnt; pointer++){

		if (command[pointer] == 'A' && command[pointer + 1] == 'C'){

			for (pointer = 0; pointer <= 20; pointer++){

				command[pointer] = 0;
			}
			char_cnt = 0;
			break;
		}

		else if (command[pointer] == 'R' && command[pointer + 1] == 'S'){

			int v = 0;

			for (v = 0; v < 5 * 3; v++){

					z[v] = 0;
					z[v] = 0;
					z[v] = 0;
			}
			for (pointer = 0; pointer <= 20; pointer++){

				command[pointer] = 0;
			}
			char_cnt = 0;
			break;
		}
	}

	if (command[char_cnt - 1] == ';'){

	    char_cnt = char_cnt - 1;

	    if (char_cnt == 2){

	    	if (((command[0] == 'G') || (command[0] == 'g')) && ((command[1] == 'O') || command[1] == 'o')){

	    			for (pointer = 0; pointer <= char_cnt; pointer++){

	    				command[pointer] = 0;
	    			}
					char_cnt = 0;
	    			fade_on();
	    	}
	    }

	    else if (char_cnt == 5){

	    	if ((command[0] == 'B' || command[0] =='b') && (command[1] == 'L' || command[1] == 'l') && (command[4] == 'K' || command[4] =='k')){

	    		if ((command[2] == 'A' || command[2] == 'a') && (command[3] == 'C' || command[3] == 'c') ){

	    			for (pointer = 0; pointer <= char_cnt; pointer++){

	    				command[pointer] = 0;
	    			}

	    			char_cnt = 0;
	    			fade_off();
	    		}

	    		else if ((command[2] == 'I' || command[2] == 'i') && (command[3] == 'N' || command[3] == 'n')){

	    			for (pointer = 0; pointer <= char_cnt; pointer++){

	    				command[pointer] = 0;
	    			}

	    			char_cnt = 0;
	    			for(;;){
	    				blink();

	    				if (char_cnt > 1)
	    					break;
	    			}
	    		}
	    	}
	    }

	    else if (char_cnt == 7){

	    	if((command[0] == 'R' || command[0] == 'r') && (command[1] == 'A' || command[1] == 'a') && (command[2] == 'I' || command[2] == 'i')
	    			&& (command[3] == 'N' || command[3] == 'n') && (command[4] == 'B' || command[4] == 'b') && (command[5] == 'O' || command[5] == 'o')
	    			&& (command[6] == 'W' || command[6] == 'w')){

	    		for (pointer = 0; pointer <= char_cnt; pointer++){

	    			command[pointer] = 0;
	    		}

	    		char_cnt = 0;
	    		flow_rainbow();
	    	}

	    	else if((command[0] == 'N' || command[0] == 'n') && (command[1] == 'O' || command[1] == 'o') && (command[2] == 'B' || command[2] == 'b')
	    			&& (command[3] == 'L' || command[3] == 'l') && (command[4] == 'I' || command[4] == 'i') && (command[5] == 'N' || command[5] == 'n')
	    			&& (command[6] == 'K' || command[6] == 'k')){

	    		for (pointer = 0; pointer <= char_cnt; pointer++){

	    			command[pointer] = 0;
	    		}

	    		char_cnt = 0;

	    		for(;;){
	    			noblink();

	    			if (char_cnt > 1)
	    				break;
	    		}
	    	}
	    }

	    else if ((command[0] == 'S' || command[0] == 's') && (command[1] == 'E' || command[1] == 'e') && (command[2] == 'T' || command[2] == 't')
	    		&& (command[3] == '(') && command[char_cnt - 1] == ')'){

	    	int pix,green,blue,red;

	    	if( char_cnt > 10 && char_cnt < 19){

	    		pix = command[4] - 48;	//ASCII code for 0 starts from 48

	    		int q = 0;

	    		for (pin = 6; pin < char_cnt; pin++){

	    			if (command[pin] == ','){

	    				comma[q] = pin;
	    				q = q + 1;
	    			}
	    		}

	    		int comma1 = comma[0];
	    		int comma2 = comma[1];

	    		int u;
	    		for (u = 0; u < 3; u++){

	    			str1[u] = 0;
	    		}
	    		for (u = 0; u < 3; u++){

	    			str2[u] = 0;
	    		}
	    		for (u = 0; u < 3; u++){

	    			str3[u] = 0;
	    		}

	    		for (u = 6; u < comma1; u++){

	    			str1[u - 6] = command[u];
	    		}

	    		for (u = comma1 + 1; u < comma2; u++){

	    			str2[u - (comma1 + 1)] = command[u];
	    		}

	    		for (u = comma2 + 1; u < char_cnt - 1; u++){

	    			str3[u - (comma2 + 1)] = command[u];
	    		}

	    		green = atoi(str1);
	    		red = atoi(str2);
	    		blue = atoi(str3);
	    		set(pix,green,red,blue);

	    		for (pointer = 0; pointer <= char_cnt; pointer++){
	    			command[pointer] = 0;
	    		}
	    		char_cnt = 0;
	    		for (;;){
	    			write_ws2811(z, sizeof(z), NEO);
	    			if (char_cnt > 0)
	    				break;
	    		}
	    	}
	    }
	}
}

void set(int pixel,int g,int r,int b){

	int p;

	if (pixel > 0 && pixel < 6){

		for (p = (pixel - 1) * 3; p < pixel * 3;){

			z[p++] = g;
			z[p++] = r;
			z[p++] = b;
		}
	}

	else if (pixel == 0){

		for (p = 0; p < 5 * 3;){

			z[p++] = g;
			z[p++] = r;
			z[p++] = b;
		}
	}

	write_ws2811(z, sizeof(z), NEO);
}

void blackk(void){	// prepared for fade-off blink;

	int cnt = 0;
	uint8_t k[5*3];

	int i = 0;

	for (i = 0; i < 5 * 3; i++){

			k[i] = z[i];
			k[i] = z[i];
			k[i] = z[i];
	}

	for(;;){

		write_ws2811(k, sizeof(k), NEO);
		__delay_cycles(75000);

		for (i = 0; i < 5 * 3; i++){

			if(k[i] > 0){
				k[i]--;}
			if(k[i] > 0){
				k[i]--;}
			if(k[i] > 0){
				k[i]--;}

			cnt = cnt + 1;
		}

		if (cnt > 3825)
			break;
	}

	write_ws2811(k, sizeof(k), NEO);	//k
}

void fade_off(void){

	uint8_t k[5*3];

	int i = 0;

	for (i = 0; i < 5 * 3; i++){

			k[i] = z[i];
			k[i] = z[i];
			k[i] = z[i];
	}

	for(;;){

		write_ws2811(k, sizeof(k), NEO);
		__delay_cycles(650000);

		for (i = 0; i < 5 * 3; i++){

			if(k[i] > 0){
				k[i]--;}
			if(k[i] > 0){
				k[i]--;}
			if(k[i] > 0){
				k[i]--;}
		}

		if (char_cnt > 0)
			break;
	}
}

void goo(void){		// prepared for fade-on blink;

	int count = 0;

	uint8_t p[5*3];

	int i = 0;

	for (i = 0; i < 5 * 3; i++){

			p[i] = 0;
			p[i] = 0;
			p[i] = 0;
	}

	for(;;){

		write_ws2811(p, sizeof(p), NEO);
		__delay_cycles(75000);

		for (i = 0; i < 5 * 3; i++){

			if(p[i] < z[i]){  //change back to z
					p[i]++;}
			if(p[i] < z[i]){
					p[i]++;}
			if(p[i] < z[i]){
					p[i]++;}
			count = count + 1;
		}

		if (count > 3825)	// checking if two 2D-arrays are the same is too complicated with another function
			break;
	}

	write_ws2811(p,sizeof(p),NEO);
}

void fade_on(void){

	uint8_t p[5*3];

	int i = 0;

	for (i = 0; i < 5 * 3; i++){

			p[i] = 0;
			p[i] = 0;
			p[i] = 0;
	}

	for(;;){

		write_ws2811(p, sizeof(p), NEO);
		__delay_cycles(650000);

		for (i = 0; i < 5 * 3; i++){

			if(p[i] < z[i]){  //change back to z
					p[i]++;}
			if(p[i] < z[i]){
					p[i]++;}
			if(p[i] < z[i]){
					p[i]++;}
		}

		if (char_cnt > 0){
			break;
		}
	}
}

void blink(void){

		blackk();

		__delay_cycles(2500000);

		goo();

		__delay_cycles(2500000);
}

void noblink(void){

		write_ws2811(z, sizeof(z), NEO);
}

void flow_rainbow(void){

	unsigned int R = 15;
	unsigned int G = 15;
	unsigned int B = 15;

	for(;;) {


		if (char_cnt > 0)
			break;

		unsigned t;

		for(t = ((x+0)%5) * 3; t < ((x+0)%5+1) * 3; ) {

			z[t++] = 0*G;
			z[t++] = 15*R;
			z[t++] = 0*B;	//red
		}

		for(t = ((x+1)%5) *3; t < ((x+1)%5+1) * 3; ) {

			z[t++] = 10*G;
			z[t++] = 15*R;
			z[t++] = 0*B;	//yellow-orange
		}

		for(t = ((x+2)%5) *3; t < ((x+2)%5+1) * 3; ) {

			z[t++] = 15*G;
			z[t++] = 0*R;
			z[t++] = 0*B;	//green
		}

		for(t = ((x+3)%5) *3; t < ((x+3)%5+1) * 3; ) {

			z[t++] = 0*G;
			z[t++] = 0*R;
			z[t++] = 15*B;	//blue
		}

		for(t = ((x+4)%5) *3; t < ((x+4)%5+1) * 3; ) {

			z[t++] = 0*G;
			z[t++] = 15*R;
			z[t++] = 15*B;	//purple
		}

	x = x + 1;

	__delay_cycles(2000000);

	write_ws2811(z, sizeof(z), NEO);
	}
}
