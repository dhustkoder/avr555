#include <stdnoreturn.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

/* *
 * 555 timer emulator for AVR by Rafael Moura (https://github.com/dhustkoder)
 * this emulator is based on 5v input for a 555 timer, with 1nF - 100nF capacitor on control pin
 * */

enum Pins {
	ANALOG_PIN_TRIGGER = 0x00,           /* input */
	ANALOG_PIN_THRESHOLD = 0x01<<MUX0,   /* input */
	DIGITAL_PIN_OUTPUT = 0x01<<PD3,      /* output */
	DIGITAL_PIN_RESET = 0x01<<PD2,       /* input  */
	DIGITAL_PIN_DISCHARGE = 0x01<<PD4    /* output */
};


static void avr555_init(void)
{
	/* digital setup */
	DDRD |= DIGITAL_PIN_OUTPUT|DIGITAL_PIN_DISCHARGE;
	DDRD &= ~DIGITAL_PIN_RESET;
	PORTD |= DIGITAL_PIN_DISCHARGE;
}

static uint16_t read_adc(const uint8_t analog_pin)
{
	ADMUX = (0x01<<REFS0)|analog_pin;

	ADCSRA = (0x01<<ADEN)|(0x01<<ADSC)|
	  (0x01<<ADPS2)|(0x01<<ADPS1)|(0x01<<ADPS0);

	while (ADCSRA&(0x01<<ADSC))
		_delay_us(1);

	return ADC;
}

static bool trigger_comparator_output(void)
{
	const uint16_t vplus = 341;
	const uint16_t vminus = read_adc(ANALOG_PIN_TRIGGER);
	return vplus > vminus;
}

static bool threshold_comparator_output(void)
{
	const uint16_t vminus = 682;
	const uint16_t vplus = read_adc(ANALOG_PIN_THRESHOLD);
	return vplus > vminus;
}


noreturn void main(void)
{
	avr555_init();
	uart_init();

	for (;;) {
		const bool flipflop_out = !trigger_comparator_output() &&
			                   threshold_comparator_output();
		const bool vout = !flipflop_out;

		if (flipflop_out)
			PORTD &= ~DIGITAL_PIN_DISCHARGE;
		else
			PORTD |= DIGITAL_PIN_DISCHARGE;

		if (vout)
			PORTD |= DIGITAL_PIN_OUTPUT;
		else
			PORTD &= ~DIGITAL_PIN_OUTPUT;
	}

}
