#include <MKL25Z4.H>

void Delay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly*10000; t>0; t--)
		;
}

void ShortDelay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly; t>0; t--)
		;
}

