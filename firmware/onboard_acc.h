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