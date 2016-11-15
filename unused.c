#include "unused.h"
#include "io_regs.h"

#ifdef HAVE_UNUSED

void unused_port(uint8_t adr, uint8_t unused_pins) {
	uint8_t i;
	_P_REGS(addr);
	// Set all high-z (0)
	*ddr &= ~unused_pins;
	// Enable all pullup (1)
	*port|= unused_pins;

	// Must give some time for pins to settle. According to mega88A datasheet p78, fig 14.4
	// one nop should be enough. That was not the case on test board, with ~1cm of trace to
	// a pin header.
	for(i=0;i<10; i++){
		asm("nop");
	}

	// Read PIN and ensure they are high
	// If a pin is low, even though we enabled pullups,
	// we want to turn off the pull-up to avoid wasting energy
	i = ~(*pin) & unused_pins;
	*port &= (~unused_pins) | ~i;
}

void init_unused(void) {
#define UNUSED_PIOS(adr, unused_pins) \
	do{unused_port(adr, unused_pins);}while(0);

#include "_unused.h"

}

#endif
