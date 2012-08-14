#ifndef SPI_H_
#define SPI_H_

#define WRITE 0b00000000
#define READ  0b10000000

#define CS_ACC 5
#define CS_GYRO 7
#define CLOCK 4
#define DOUT 3
#define DIN 0
#define DIN0 0
#define DIN1 1
#define DIN2 2
#define DIN3 3

#define X 0
#define Y 1
#define Z 2

///Represents current chip to talk to
unsigned char cs = CS_ACC;

///Enables spi communication, 1 for start, 0 for stop
void spi_enable(unsigned char enable) {
	if(enable) {
		clear_bit(P1OUT,cs);
	} else {
		set_bit(P1OUT,CS_ACC);
		set_bit(P1OUT,CS_GYRO);
	}
	change_bit(P1OUT,cs,!enable);
}

void clockpulse(unsigned char rising) {
	change_bit(P1OUT,CLOCK,!rising);
	
	//Switch here!
	
	change_bit(P1OUT,CLOCK,rising);
}

void spi_send(unsigned char data) {
	for(signed char bit=7;bit>=0;bit--) {
		change_bit(P1OUT,DOUT,test_bit(data,bit));
		clockpulse(1);
	}
}

unsigned char spi_recieve(void) {
	unsigned char data=0;
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
	
	__delay_cycles(20000);
	
	//send data to write in register
	spi_send(WRITE|data);
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

#endif