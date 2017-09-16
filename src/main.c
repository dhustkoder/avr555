#include <stdbool.h>
#include <stdnoreturn.h>
#include <avr/io.h>
#include <util/delay.h>

/* *
 * 555 timer emulator for AVR by Rafael Moura (https://github.com/dhustkoder)
 * this emulator is based on 5v input for a 555 timer, with 1nF - 100nF capacitor on control pin
 * */

enum Pins {
	ANALOG_PIN_TR   = 0x00,
	ANALOG_PIN_TH   = 0x01<<MUX0,
	DIGITAL_PIN_DC  = 0x01<<PD2,
	DIGITAL_PIN_OUT = 0x01<<PD3
};

static uint16_t read_adc(const uint8_t analog_pin)
{
	ADMUX = (0x01<<REFS0)|analog_pin;
	ADCSRA = (0x01<<ADEN)|(0x01<<ADSC)|
	         (0x01<<ADPS2)|(0x01<<ADPS1)|(0x01<<ADPS0);

	while (ADCSRA&(0x01<<ADSC))
		;

	return ADC;
}

noreturn void main(void)
{
	DDRD |= DIGITAL_PIN_OUT|DIGITAL_PIN_DC;
	PORTD &= ~(DIGITAL_PIN_OUT|DIGITAL_PIN_DC);

	for (;;) {
		if (read_adc(ANALOG_PIN_TR) < 338)
			PORTD |= DIGITAL_PIN_OUT|DIGITAL_PIN_DC;

		if (read_adc(ANALOG_PIN_TH) > 686) {
			PORTD &= ~(DIGITAL_PIN_OUT|DIGITAL_PIN_DC);
			while (read_adc(ANALOG_PIN_TR) > 338)
				;
		}
	}
}
