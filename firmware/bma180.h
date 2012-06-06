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
unsigned char BMA180_init(unsigned char range, unsigned char bw) {
	unsigned char reg;
	cs=CS_ACC;
	
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

#endif
