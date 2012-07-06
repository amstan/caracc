#include <msp430.h>
#include "bitop.h"

#define X 0
#define Y 1
#define Z 2

#include "onboard_acc.h"
#include "bma180.h"
#include "gyro.h"

#include <stdint.h>
#include "ringbuffer.h"
#include "usci_serial.h"

ringbuffer_ui8_16 usci_buffer = { 0, 0, { 0 } };
Serial<ringbuffer_ui8_16> usci0 = { usci_buffer };
void __attribute__((interrupt (USCI_A0_VECTOR))) USCI0RX_ISR() {
	usci_buffer.push_back(UCA0RXBUF);
}

void output_leds(unsigned char leds) {
	change_bits(P3OUT,0b11110000,leds&0b11110000);
	change_bits(PJOUT,0b00001111,leds&0b00001111);
}

void __delay_ms(unsigned int ms) {
	for(;ms!=0;ms--) {
		__delay_cycles(1000);
	}
}

void io_init(void) {
	//Configure led ports
	P3DIR=0b11110000;
	PJDIR=0b00001111;
	
	//configure buttons
	#define S1 0
	#define S2 1
	clear_bits(P4DIR,(1<<S1)|(1<<S2)); //change to input
	set_bits(P4REN,(1<<S1)|(1<<S2)); //enable resistors
	set_bits(P4OUT,(1<<S1)|(1<<S2)); //pullup
	#define switch_pressed(switch) (!test_bit(P4IN,switch))
	
	//spi pins
	set_bits(P1DIR,(1<<CS_ACC)|(1<<CS_GYRO)|(1<<CLOCK)|(1<<DOUT));
	clear_bit(P1DIR,DIN);
}

void clock_8MHz(void) {
	// XT1 Setup 
	PJSEL0 |= BIT4 + BIT5; 
	
	CSCTL0_H = 0xA5;
	CSCTL1 |= DCOFSEL0 + DCOFSEL1;     // Set max. DCO setting
	CSCTL2 = SELA_0 + SELS_3 + SELM_3; // set ACLK = XT1; MCLK = DCO
	CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0; // set all dividers 
	CSCTL4 |= XT1DRIVE_0; 
	CSCTL4 &= ~XT1OFF;
	
	do
	{
		CSCTL5 &= ~XT1OFFG; // Clear XT1 fault flag
		SFRIFG1 &= ~OFIFG; 
	}while (SFRIFG1&OFIFG); // Test oscillator fault flag
}

void chip_init(void) {
	// Stop WDT
	WDTCTL = WDTPW+WDTHOLD;
	
	clock_8MHz();
}

void printint(int i) {
	if(i<0) {
		i*=-1;
		usci0.xmit('-');
	}
	for(unsigned int digit=10000;digit!=0;digit/=10) {
		usci0.xmit('0'+((i/digit)%10));
	}
}

int main(void) {
	chip_init();
	io_init();
	usci0.init();
	output_leds(255);
	
	onboard_acc_init();
	
	usci0.xmit("Booting!\n");
	
	
	while(BMA180_init(0b0010,0b0010)==-1) {
		usci0.xmit("Error starting acc communications, whoami register:");
		printint(spi_read(BMA180_ID));
		usci0.xmit('\n');
	}
	
	while(1) {
		unsigned int x,y,z;
		
		getAccValues(x,y,z);
		
		output_leds(x/256);
		
		printint(x);
		usci0.xmit("\t");
		printint(y);
		usci0.xmit("\t");
		printint(z);
		//usci0.xmit("\t");
		
		//printint(onboard_acc_read(X));
		usci0.xmit("\n");
	}
}