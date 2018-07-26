#ifndef PIT_H
#define PIT_H
#include "MKL25Z4.h"

void PIT_Init(unsigned period);
void PIT_Start(void);
void PIT_Stop(void);

#endif
