#ifndef BMA180_H_
#define BMA180_H_

#include "spi.h"

//Address defines for BMA180

//ID and Version Registers
#define BMA180_ID 0x00
#define BMA180_Version 0x01
#define BMA180_ACCXLSB 0x02
#define BMA180_ACCXMSB 0x03
#define BMA180_ACCYLSB 0x04
#define BMA180_ACCYMSB 0x05
#define BMA180_ACCZLSB 0x06
#define BMA180_ACCZMSB 0x07
#define BMA180_TEMPERATURE 0x08
#define BMA180_STATREG1 0x09
#define BMA180_STATREG2 0x0A
#define BMA180_STATREG3 0x0B
#define BMA180_STATREG4 0x0C
#define BMA180_CTRLREG0 0x0D
#define BMA180_CTRLREG1 0x0E
#define BMA180_CTRLREG2 0x0F

#define BMA180_BWTCS 0x20
#define BMA180_CTRLREG3 0x21

#define BMA180_HILOWNFO 0x25
#define BMA180_LOWDUR 0x26

#define BMA180_LOWTH 0x29

#define BMA180_tco_y 0x2F
#define BMA180_tco_z 0x30

#define BMA180_OLSB1 0x35

//Range setting
#define BMA180_RANGESHIFT 1
#define BMA180_RANGEMASK 0x0E
#define BMA180_BWMASK 0xF0
#define BMA180_BWSHIFT 4

/**init_BMA180
 * @arg range a 3-bit value between 0x00 and 0x06 will set the range as described in the BMA180 datasheet (pg. 27)
 * @arg bw a 4-bit value between 0x00 and 0x09.  Again described on pg. 27
 * @returns -1 on error, 0 on success
 */
int BMA180_init(unsigned char range, unsigned char bw) {
	cs=CS_ACC;
	
	unsigned char reg;
	
	//Apparently the id is always supposed to be 3
	if(spi_read(BMA180_ID)!=3)
		return -1;
	
	// Have to set ee_w to write any other registers
	reg = spi_read(BMA180_CTRLREG0);
	set_bits(reg,0x10);
	spi_write(BMA180_CTRLREG0, reg);
	
	//Set bw
	bw<<=4;
	reg = spi_read(BMA180_BWTCS);
	change_bits(reg,BMA180_BWMASK,bw);
	spi_write(BMA180_BWTCS, reg);
	
	//Set range
	range<<=BMA180_RANGESHIFT;
	reg = spi_read(BMA180_OLSB1);
	change_bits(reg,BMA180_RANGEMASK,range);
	spi_write(BMA180_OLSB1, reg);
	
	//disable i2c
	spi_write(0x27,0b00000001);
	
	//shadow register enabled
	spi_write(0x33,0b00000000);
	
	return 0;
}

void getAccValues(signed int acc[3]) {
	cs=CS_ACC;
	
	spi_enable(1);
	
	spi_send(READ|BMA180_ACCXLSB);
	
	for(unsigned char d=0;d<3;d++){
		acc[d] = spi_recieve();
		acc[d] |= spi_recieve()<<8;
	}
	
	spi_enable(0);
}

///Number of accelerometer devices
#define ACCNUMBER 4

void spi_recieve_multi(unsigned int data[3]) {
	for(unsigned int i=0;i<ACCNUMBER;i++) {
		data[i]=0;
	}
	for(signed char bit=7;bit>=0;bit--) {
		clockpulse(1);
		
		change_bit(data[0],bit,test_bit(P1IN,DIN0));
		change_bit(data[1],bit,test_bit(P1IN,DIN1));
		change_bit(data[2],bit,test_bit(P1IN,DIN2));
		//change_bit(data[3],bit,test_bit(P1IN,DIN));
	}
}

void getAccsValues(signed int acc[ACCNUMBER][3]) {
	unsigned int data[ACCNUMBER];
	
	cs=CS_ACC;
	spi_enable(1);
	spi_send(READ|BMA180_ACCXLSB);
	
	for(unsigned char d=0;d<3;d++){
		spi_recieve_multi(data);
		for(unsigned int i=0;i<ACCNUMBER;i++) {
			acc[i][d] = data[i];
		}
		
		spi_recieve_multi(data);
		for(unsigned int i=0;i<ACCNUMBER;i++) {
			acc[i][d] |= data[i]<<8;
		}
	}
	
	spi_enable(0);
}

#endif
