#include <msp430.h>
#include "bitop.h"

#define X 0
#define Y 1
#define Z 2

void output_leds(unsigned char leds) {
	change_bits(P3OUT,0b11110000,leds&0b11110000);
	change_bits(PJOUT,0b00001111,leds&0b00001111);
}

void onboard_acc_init(void) {
	//configure pins for adc
	P3SEL0 = 0b00000111;
	P3SEL1 = 0b00000111;
	
	//setup adc
	ADC10CTL0 |= ADC10SHT_2 + ADC10ON;        // ADC10ON, S&H=16 ADC clks
	ADC10CTL1 |= ADC10SHP;                    // ADCCLK = MODOSC; sampling timer
	ADC10CTL2 |= ADC10RES;                    // 10-bit conversion results
	
	//power accelerometer
	set_bit(P2DIR,7);
	set_bit(P2OUT,7);
	
	// Allow the accelerometer to power up before sampling any data 
	__delay_cycles(200000);
}

unsigned int onboard_acc_read(unsigned char channel) {
	ADC10CTL0 &= ~ADC10ENC;                   //Stop conversion
	change_bits(ADC10MCTL0, 0b00001111, 12+channel); // ADC input select;
	//output_leds(ADC10MCTL0);
	ADC10CTL0 |= ADC10ENC + ADC10SC;          // Sampling and conversion start
	while (ADC10CTL1 & BUSY);
	return ADC10MEM0;
}

void io_init(void) {
	WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
	
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
}

void main(void)
{
	io_init();
	onboard_acc_init();
	
	//Calibrate
	#define CALIBRATION_COUNT 50
	unsigned int calibration[3]={0,0,0};
	for(unsigned int i=0;i<CALIBRATION_COUNT;i++) {
		output_leds(i);
		for(unsigned int channel=0;channel<3;channel++) {
			__delay_cycles(2000);
			calibration[channel]+=onboard_acc_read(channel);
		}
	}
	for(unsigned int channel=0;channel<3;channel++) {
		calibration[channel]/=CALIBRATION_COUNT;
	}
	
	while(1) {
		unsigned char leds=0;
		for(unsigned int channel=0;channel<3;channel++) {
			unsigned int rawvalue=onboard_acc_read(channel);
			int value=rawvalue-calibration[channel];
			change_bit(leds,channel,value>0);
		}
		output_leds(leds);
	}
}