/*
 *  Copyright © 2010-2015, Matthias Urlichs <matthias@urlichs.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License (included; see the file LICENSE)
 *  for more details.
 */

/* Based on work published at http://www.mikrocontroller.net/topic/44100 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define MAIN
#include "features.h"
#include "onewire.h"
#include "uart.h"
#include "console.h"
#include "timer.h"
#include "dev_data.h"
#include "debug.h"
#include "moat.h"
#include "jmp.h"
#include "status.h"
#include "unused.h"

uint8_t mcusr __attribute__ ((section (".noinit")));
#ifdef HAVE_IRQ_CATCHER
extern uint8_t reboot_irq;
#endif
void setup_watchdog(void) __attribute__((naked,section(".init3")));
void setup_watchdog(void)
{
	mcusr = MCUSR;
#if 0 // def N_STATUS
	if (reboot_irq) {
		mcusr = S_boot_irq | reboot_irq;
		reboot_irq = 0;
	}
#endif
	MCUSR = 0;

#ifdef HAVE_WATCHDOG
	wdt_reset();
#ifdef HAVE_IRQ_CATCHER
	// intense debugging
	wdt_enable(0x05);
#else
	wdt_enable(0x09);
#endif
#else
	wdt_disable();
#endif
}

// Initialise the hardware
static inline void
init_mcu(void)
{
#ifdef ADCSRA
	ADCSRA = 0;
#endif
#ifdef __AVR_ATtiny13__
	CLKPR = 0x80;    // Prepare to ...
	CLKPR = 0x00;    // ... set to 9.6 MHz

#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	CLKPR = 0x80;    // Prepare to ...
	CLKPR = 0x00;    // ... set to 8.0 MHz

#elif defined(__AVR_ATtiny84__)
	CLKPR = 0x80;    // Prepare to ...
	CLKPR = 0x00;    // ... set to 8.0 MHz

#elif defined (__AVR_ATmega8__)
	// Clock is set via fuse
#elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined(__AVR_ATmega328__)
	// Clock is set via fuse

#else
#error Basic config for your CPU undefined
#endif
}
 
static inline void
init_all(void)
{
	eeprom_init();
	console_init();
	onewire_init();
	timer_init();
	init_state();
	init_unused();
}

static inline void
poll_all(void)
{
#if defined(UART_DEBUG) && defined(N_CONSOLE)
	uint16_t c;
#endif
	timer_poll();
	uart_poll();
	onewire_poll();
#if defined(UART_DEBUG) && defined(N_CONSOLE)
	c = uart_getc();
	if(c <= 0xFF)
		console_putc(c);
#endif
}

static uint16_t bootseq __attribute__((section(".noinit")));

// Main program
int
main(void)
{
#ifdef HAVE_DBG_PIN
	DBGPINPORT &= ~(1 << DBGPIN);
	DBGPINDDR |= (1 << DBGPIN);
#endif
#ifdef HAVE_DBG_PORT
	DBGPORT = 0;
	DBGDDR |= 0x1F;
#endif

	init_mcu();
	uart_init(UART_BAUD_SELECT(BAUDRATE,F_CPU));
	uart_puts_P("\nreboot ");
	uart_puthex_word(mcusr);
	uart_putc(' ');
	uart_puthex_word(++bootseq);
	uart_putc(' ');

	init_all();

	// now go
	DBG(0x33);
	sei();
	DBG(0x21);
	setjmp_q(_go_out);
	DBG(0x23);
	/* clobbered variables (and constants) beyond this point */
	while(1) {
#if defined(HAVE_WATCHDOG) && (!defined(ONEWIRE_MOAT) || !defined(CONDITIONAL_SEARCH))
		wdt_reset();
#endif
#ifdef IS_BOOTLOADER
		onewire_poll();
#else
		poll_all();
		mainloop();
#endif
	}
}

