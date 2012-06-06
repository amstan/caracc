#ifndef SPI_H_
#define SPI_H_

#define WRITE 0b00000000
#define READ  0b10000000

#define CS 0
#define CLOCK 1
#define DOUT 2
#define DIN 7
#define INTERRUPT 6

///Enables spi communication, 1 for start, 0 for stop
unsigned char spi_enable(unsigned char enable) {
	change_bit(P1OUT,CS,!enable);
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

#endif