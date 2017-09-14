#include <stdnoreturn.h>
#include "uart.h"




noreturn void main(void)
{
	uart_init();

	for (;;)
		puts("Hello AVR555");
}
