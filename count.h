#ifndef COUNT_H
#define COUNT_H

#include "dev_data.h"
#include "features.h"

#if defined(N_COUNT)

typedef struct {
	uint8_t port;
	unsigned char flags;
#define CF_ALERTING (1<<0)
#define CF_IS_ALERT (1<<6)
#define CF_IS_ON (1<<7)
	uint16_t count;
#define COUNT_SIZE 2
} count_t;

extern count_t counts[];

#ifdef CONDITIONAL_SEARCH
extern uint8_t count_changed_cache;

#else
#define count_alert() 0
#endif

#else // no i/o

#define count_alert() 0

#endif // any inputs or outputs at all
#endif // count_h