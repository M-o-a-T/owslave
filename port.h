#ifndef PORT_H
#define PORT_H

#include "dev_data.h"
#include "features.h"

#if defined(N_PORT)

typedef struct {
	uint8_t adr;
	uint8_t flags;
} port_t;
extern port_t ports[];

#include "io_regs.h" // defines _P_REGS

#define _P_VARS(_port) \
	uint8_t flg __attribute__((unused)) = _port->flags; \
	uint8_t adr = _port->adr; \
	_P_REGS(addr); \
	adr = 1<<(adr & 0x07);

#define _P_GET(_reg) (!!(*_reg & adr))
#define _P_SET(_reg,_val) do { \
	if(_val)                   \
		*_reg |= adr;         \
	else                       \
		*_reg &=~adr;         \
	} while(0);

// the first two bits are used for port_out_t, i.e. PO_* constants. Hardcoded.
#define PFLG_ALERT   (1<<2)  // alert when port changes externally
#define PFLG_ALT     (1<<3)  // switch H/Z and L/pull-up; default: H/L and Z/pull-up
#define PFLG_ALT2    (1<<4)  // switch H/pullup and L/Z
#define PFLG_POLL    (1<<5)  // change has been reported
#define PFLG_CHANGED (1<<6)  // pin change
#define PFLG_CURRENT (1<<7)  // last-seen value. Hardcoded.

typedef enum {
	PI_OFF=0, PI_ON=1
} port_in_t;
typedef enum { // do not change: bit values are user in port.c
	PO_OFF=0, PO_ON=1, PO_Z=2, PO_PULLUP=3
} port_out_t;

// actual port-pin state
static inline port_in_t port_get_in(port_t *portp) {
	_P_VARS(portp)
	return _P_GET(pin);
}

// read intended port state from registers
static inline port_out_t port_get_out(port_t *portp) {
	_P_VARS(portp)
	return _P_GET(port) | (!_P_GET(ddr)<<1) ;
}

// set intended port state
static inline void port_set_out(port_t *portp, port_out_t state) {
	_P_VARS(portp)
	_P_SET(port, state&1);
	_P_SET(ddr,!(state&2));
	portp->flags = (portp->flags&~PFLG_CURRENT) | (state<<7);
}

// update flags based on current port state
void port_check(port_t *pp);

// Set port to 0/1 according to mode (PFLG_ALT*). This is harder than it seems.
void port_set(port_t *portp, char val);


static inline char port_changed(port_t *portp) {
	return portp->flags & PFLG_CHANGED;
}

/* Number of highest port that has a change +1  */
extern uint8_t port_changed_cache;

/* Note whether a port has changed */
static inline char port_has_changed(port_t *portp) {
	uint8_t flg = portp->flags;
	if (flg & PFLG_CHANGED) {
		flg |= PFLG_POLL;
		flg &=~ PFLG_CHANGED;
		portp->flags = flg;
		return 1;
	}
	return 0;
}

static inline void port_pre_send (port_t *portp) {
	(void)port_has_changed(portp);
}

static inline void port_post_send (port_t *portp) {
	uint8_t flg = portp->flags;
	if(flg & PFLG_POLL) {
		if (flg & PFLG_CHANGED)
			flg &=~PFLG_CHANGED;
		else
			flg &=~PFLG_POLL;
		portp->flags = flg;
	}
}

#else // no i/o

#endif // any inputs or outputs at all
#endif // port_h
