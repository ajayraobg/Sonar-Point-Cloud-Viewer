#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

typedef struct {
	uint32_t X, Y;
} PT_T;

typedef struct {
	uint8_t R, G, B; // note: using 5-6-5 color mode for LCD. 
									 // Values are left aligned here
} COLOR_T;

void Graphics_Test(void);
void DrawLine(PT_T * p1, PT_T * p2, COLOR_T * color);

#define STEP 8

#endif // GRAPHICS_H
