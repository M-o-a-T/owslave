#ifndef IO_REGS_H
#define IO_REGS_H

/* IO port mapping, used in port.h and unused.c
 *
 * addr format:
 * 	Bits 7-4 identifes the port (A=0, B=1, ...)
 * 	Bits 3-1 bits are bit position in the port.
 *
 * _P_REGS uses bit 7-4 to create pin,ddr,port pointers.
 */

#ifdef __AVR_ATmega8__
/* Mega8 layout:
 * 	0x2D, 0x2E, 0x2F (irrelevant, there is no port A)
 * 	0x30 PIND, 0x31 DDRD, 0x32 PORTD
 * 	0x33 PINC, 0x34 DDRC, 0x35 PORTC
 * 	0x36 PINB, 0x37 DDRB, 0x38 PORTB
 */
#define _P_REGS(addr) \
	uint8_t _i=3*(4-(adr>>3)); \
	uint8_t *pin __attribute__((unused)) = (uint8_t *)(0x2D+_i); \
	uint8_t *ddr __attribute__((unused)) = (uint8_t *)(0x2D+_i+1); \
	uint8_t *port __attribute__((unused)) = (uint8_t *)(0x2D+_i+2);


#else


/* Mega48/88/168 and others have
 * 	0x20, 0x21, 0x22 Reserved (there is no port A)
 * 	0x23 PINB, 0x24 DDRB, 0x25 PORTB
 * 	0x26 PINC, 0x27 DDRC, 0x28 PORTC
 * 	0x29 PIND, 0x2A DDRD, 0x2B PORTD
 */
#define _P_REGS(addr) \
	uint8_t _i=3*(adr>>3); \
	uint8_t *pin __attribute__((unused)) = (uint8_t *)(0x20+_i); \
	uint8_t *ddr __attribute__((unused)) = (uint8_t *)(0x20+_i+1); \
	uint8_t *port __attribute__((unused)) = (uint8_t *)(0x20+_i+2);
#endif


#endif // IO_REGS_H
