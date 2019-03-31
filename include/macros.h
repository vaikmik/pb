#ifndef MACROS_H
#define MACROS_H

#include <string.h>

#define FALSE                   (0)
#define TRUE                    (1)

#define ZERO_STRUCT(x)          memset(&(x), 0x00, sizeof x)
#define ZERO_STRUCTP(x)         if((x))do{memset((x), 0x00, sizeof *(x));}while(0)

#ifdef PB_DEBUG
#define ASSERT(x)               if(!(x))do{*(volatile int *)0=0;}while(0)
#else
#define ASSERT(x)
#endif

#endif
