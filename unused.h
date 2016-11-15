#ifndef UNUSED_H
#define UNUSED_H

#include "features.h"

#ifdef HAVE_UNUSED
void init_unused(void);
#else
#define init_unused() do{} while(0)
#endif

#endif
