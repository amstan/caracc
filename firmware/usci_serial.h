#ifndef HW_SERIAL_H
#define HW_SERIAL_H

/**
 * Serial - simple access to USCI UART hardware
 *          code implements interrupt driven input
 *          and poll driven output.
 *
 * License: Do with this code what you want. However, don't blame
 * me if you connect it to a heart pump and it stops.  This source
 * is provided as is with no warranties. It probably has bugs!!
 * You have been warned!
 *
 * Author: Rick Kimball
 * email: rick@kimballsoftware.com
 * Version: 1.00 Initial version 05-12-2011
 */

template<typename T_STORAGE>
struct Serial {
	T_STORAGE &_recv_buffer;
	
	/**
	* init - setup the USCI UART hardware for 9600-8-N-1
	*        P1.1 = RX PIN, P1.2 = TX PIN
	*/
	inline void init() {
		// Configure UART pins
		P2SEL1 |= BIT0 + BIT1;
		P2SEL0 &= ~(BIT0 + BIT1);
		// Configure UART 0
		UCA0CTL1 |= UCSWRST; 
		UCA0CTL1 = UCSSEL_1;                      // Set ACLK = 32768 as UCBRCLK
		UCA0BR0 = 3;                              // 9600 baud
		UCA0BR1 = 0; 
		UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
												// UCBRSx value = 0x53 (See UG)
		UCA0CTL1 &= ~UCSWRST;                     // release from reset
		UCA0IE |= UCRXIE;                         // Enable RX interrupt
	}
	
	inline bool empty() {
		return _recv_buffer.empty();
	}
	
	inline int recv() {
		while(empty());
		return _recv_buffer.pop_front();
	}
	
	void xmit(uint8_t c) {
		while (!(UCA0IFG&UCTXIFG)); // USCI_A0 TX buffer ready?
		UCA0TXBUF =  (uint8_t) c;   // TX -> RXed character
	}
	
	void xmit(const char *s) {
		while (*s) {
			xmit((uint8_t) *s);
			++s;
		}
	}
};

#endif /* HW_SERIAL_H */
