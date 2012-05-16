#include "bitop.h"
#include <avr/io.h>
#include <avr/delay.h>

void main(void) {
	DDRB=0b00000001;
	while(1) {
		_delay_ms(1000);
		toggle_bit(PORTB,0);
	}
}