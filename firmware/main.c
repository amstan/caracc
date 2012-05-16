#include <msp430.h>
#include "bitop.h"

#define X 0
#define Y 1
#define Z 2

#include "onboard_acc.h"
#include "bma180.h"

void output_leds(unsigned char leds) {
	change_bits(P3OUT,0b11110000,leds&0b11110000);
	change_bits(PJOUT,0b00001111,leds&0b00001111);
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

#define WRITE 0b00000000
#define READ  0b10000000

#define CS 0
#define CLOCK 1
#define DOUT 2
#define DIN 7

///Enables spi communication, 1 for start, 0 for stop
unsigned char spi_enable(unsigned char enable) {
	change_bit(P1OUT,CS,!enable);
}

void clockpulse(unsigned char rising) {
	change_bit(P1OUT,CLOCK,!rising);
	__delay_cycles(2000);
	
	//Switch here!
	
	change_bit(P1OUT,CLOCK,rising);
	__delay_cycles(2000);
}

void spi_send(unsigned char data) {
	for(signed char bit=7;bit>=0;bit--) {
		change_bit(P1OUT,DOUT,test_bit(data,bit));
		clockpulse(1);
	}
}

unsigned char spi_recieve(void) {
	unsigned char data;
	for(signed char bit=7;bit>=0;bit--) {
		clockpulse(1);
		change_bit(data,bit,test_bit(P1IN,DIN));
	}
	return data;
}

void spi_write(unsigned char reg, unsigned char data) {
	spi_enable(1);
	
	__delay_cycles(20000);
	//send register to write
	spi_send(WRITE|reg);
	//spi_recieve();
	
	__delay_cycles(20000);
	
	//send data to write in register
	spi_send(WRITE|data);
	//spi_recieve();
	__delay_cycles(20000);
	
	spi_enable(0);
}

unsigned char spi_read(unsigned char reg) {
	//Read something
	spi_enable(1);
	
	//send register to read
	spi_send(READ|reg);
	
	//read the data
	unsigned char data=spi_recieve();
	spi_enable(0);
	return data;
}

/**init_BMA180
 * @arg range a 3-bit value between 0x00 and 0x06 will set the range as described in the BMA180 datasheet (pg. 27)
 * @arg bw a 4-bit value between 0x00 and 0x09.  Again described on pg. 27
 * @returns -1 on error, 0 on success
 */
unsigned char BMA180_init(unsigned char range, unsigned char bw) {
	char temp, temp1;
	
	//Apparently the id is always supposed to be 3, i get 1.
	if(spi_read(BMA180_ID)!=3)
		return -1;
	
	// Have to set ee_w to write any other registers
	temp = spi_read(BMA180_CTRLREG0);
	set_bits(temp,0x10);
	spi_write(BMA180_CTRLREG0, temp);
	
	//Set bw
	bw<<=4;
	temp = spi_read(BMA180_BWTCS);
	change_bits(temp,BMA180_BWMASK,bw);
	spi_write(BMA180_BWTCS, temp);
	
	//Set range
	range<<=BMA180_RANGESHIFT;
	temp = spi_read(BMA180_OLSB1);
	change_bits(temp,BMA180_RANGEMASK,range);
	spi_write(BMA180_OLSB1, temp);
	
	return 0;
}

// init_BMA180
// Input: range is a 3-bit value between 0x00 and 0x06 will set the range as described in the BMA180 datasheet (pg. 27)
// bw is a 4-bit value between 0x00 and 0x09.  Again described on pg. 27
// Output: -1 on error, 0 on success
int init_BMA180(unsigned char range, unsigned char bw)
{
	char temp, temp1;
	
	// if connected correctly, ID register should be 3
	if(spi_read(BMA180_ID) != 3)
		return -1;
		
	//-------------------------------------------------------------------------------------
	// Set ee_w bit
	temp = spi_read(BMA180_CTRLREG0);
	temp |= 0x10;
	spi_write(BMA180_CTRLREG0, temp);	// Have to set ee_w to write any other registers
	//-------------------------------------------------------------------------------------
	// Set BW
	temp = spi_read(BMA180_BWTCS);
	temp1 = bw;
	temp1 = temp1<<4;
	temp &= (~BMA180_BWMASK);
	temp |= temp1;
	spi_write(BMA180_BWTCS, temp);		// Keep tcs<3:0> in BWTCS, but write new BW
	//-------------------------------------------------------------------------------------
	// Set Range
	temp = spi_read(BMA180_OLSB1);
	temp1 = range;
	temp1 = (temp1<<BMA180_RANGESHIFT);
	temp &= (~BMA180_RANGEMASK);
	temp |= temp1;
	spi_write(BMA180_OLSB1, temp); //Write new range data, keep other bits the same
	//-------------------------------------------------------------------------------------
	
	return 0;
}

void main(void)
{
	io_init();
	__delay_cycles(20000);
	
	set_bits(P1DIR,(1<<CS)|(1<<CLOCK)|(1<<DOUT));
	clear_bit(P1DIR,DIN);
	
	if(init_BMA180(0b010, 0b0100)==-1)
		while(1) {
			output_leds(0xFF);
			__delay_cycles(20000);
			output_leds(spi_read(BMA180_ID));
			__delay_cycles(20000);
		}
	
	while(1) {
		output_leds(spi_read(BMA180_ACCXLSB));
		__delay_cycles(2000);
	}
}