#include "avr.h"
float wait_value = 0.00005;

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void
avr_wait_music(unsigned short msec)
{
	TCCR0 = 2; //page 82
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * wait_value);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}