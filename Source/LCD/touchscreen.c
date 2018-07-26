/* Code for resistive touchscreen. */

#include <stdint.h>
#include <stdio.h>
#include "MKL25Z4.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "ST7789.h"
#include "T6963.h"
#include "font.h"

#include "gpio_defs.h"
#include "timers.h"

extern void Delay(uint32_t);

/* Is set to one if touchscreen been calibrated. */
uint8_t LCD_TS_Calibrated = 1;
uint32_t LCD_TS_X_Scale=209, LCD_TS_X_Offset=6648;
uint32_t LCD_TS_Y_Scale=159, LCD_TS_Y_Offset=6689;

PT_T TS_Min;
PT_T TS_Max;

void Init_ADC(void) {
	
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; 
	ADC0->CFG1 = 0x9C; // 16 bit
	ADC0->SC2 = 0;
}


/* Initialize touchscreen hardware (ADC). */
void LCD_TS_Init(void) {
	// Configure ADC
	Init_ADC();
}


static uint16_t xl=0, yu=0;

/* Read touch screen. Returns 1 if touched, and updates position. Else returns 0 leaving 
position unchanged. */
uint32_t LCD_TS_Read(PT_T * position) {
	uint32_t x, y;
	uint32_t b;

	// Determine if screen was pressed.
	// Set YU digital output at ground, 
	LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] &= ~PORT_PCR_MUX_MASK;
	LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] |= PORT_PCR_MUX(1);
	LCD_TS_YU_PT->PDDR |= MASK(LCD_TS_YU_BIT);
	LCD_TS_YU_PT->PCOR = MASK(LCD_TS_YU_BIT); // Clear YU to 0

	// Set YD + XL to open
	// Set YD as ADC input
	LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] &= ~PORT_PCR_MUX_MASK;
	LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] |= PORT_PCR_MUX(0);

	// Set XL as digital input
	LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] &= ~PORT_PCR_MUX_MASK;
	LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] |= PORT_PCR_MUX(1);
	LCD_TS_XL_PT->PDDR &= ~MASK(LCD_TS_XL_BIT); // Input
	
	// Set XR as digital input with pull-up
	LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] &= ~PORT_PCR_MUX_MASK;
	LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] |= PORT_PCR_MUX(1);
	LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] |= PORT_PCR_PE_MASK; // Enable pull-up
	LCD_TS_XR_PT->PDDR &= ~MASK(LCD_TS_XR_BIT); // Input
		
	// Wait for the inputs to settle
	Delay(TS_DELAY);
	// Read XR input via digital
	// if XR is 0, then screen is pressed
	b = (LCD_TS_XR_PT->PDIR) & MASK(LCD_TS_XR_BIT);
	
	if (b>0) {
		// Screen not pressed
		return 0;
	} else {
		// Read X Position
		// Configure inputs to ADC
		LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] |= PORT_PCR_MUX(0);
		LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] |= PORT_PCR_MUX(0);

		
		// Configure outputs to GPIO
		LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] |= PORT_PCR_MUX(1);
		LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] |= PORT_PCR_MUX(1);
		LCD_TS_XL_PT->PDDR |= MASK(LCD_TS_XL_BIT); 
		LCD_TS_XR_PT->PDDR |= MASK(LCD_TS_XR_BIT);
		LCD_TS_XR_PT->PSOR = MASK(LCD_TS_XR_BIT); // Set XR to 1
		LCD_TS_XL_PT->PCOR = MASK(LCD_TS_XL_BIT); // Clear XL to 0
		// Wait for inputs to settle
		Delay(TS_DELAY);
		
		// Read inputs
		ADC0->SC1[0] = LCD_TS_YU_CHANNEL; // start conversion on channel YU
		while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
			;
		yu = ADC0->R[0];
		x = yu;

		// Read Y Position
		// Configure inputs to ADC
		LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_XL_PORT->PCR[LCD_TS_XL_BIT] |= PORT_PCR_MUX(0);
		LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] |= PORT_PCR_MUX(0);
		// Disable pull-up - just to be sure
		LCD_TS_XR_PORT->PCR[LCD_TS_XR_BIT] &= ~PORT_PCR_PE_MASK; 
		
		// Configure outputs to GPIO
		LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_YU_PORT->PCR[LCD_TS_YU_BIT] |= PORT_PCR_MUX(1);
		LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] &= ~PORT_PCR_MUX_MASK;
		LCD_TS_YD_PORT->PCR[LCD_TS_YD_BIT] |= PORT_PCR_MUX(1);
		LCD_TS_YU_PT->PDDR |= MASK(LCD_TS_YU_BIT);
		LCD_TS_YD_PT->PDDR |= MASK(LCD_TS_YD_BIT);
		LCD_TS_YD_PT->PSOR = MASK(LCD_TS_YD_BIT); // Set YD to 1
		LCD_TS_YU_PT->PCOR = MASK(LCD_TS_YU_BIT); // Clear YU to 0
		// Wait for the inputs to settle
		Delay(TS_DELAY);

		// Read inputs
		ADC0->SC1[0] = LCD_TS_XL_CHANNEL; // start conversion on channel XL
		while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
			;
		xl = ADC0->R[0];
		y = xl;

		// Apply calibration factors to raw position information
		if (LCD_TS_Calibrated) {
			if (x<LCD_TS_X_Offset) {
				position->X = 0;
			} else {
				position->X = (x - LCD_TS_X_Offset)/LCD_TS_X_Scale;
			}
			if (y<LCD_TS_Y_Offset) {
				position->Y = 0;
			} else {
				position->Y = (y - LCD_TS_Y_Offset)/LCD_TS_Y_Scale;
			}
		} else {
			position->X = x;
			position->Y = y;
		}
		return 1;
	}
}

void LCD_TS_Blocking_Read(PT_T * position) {
	PT_T temp;
	// Wait for screen press
	while (LCD_TS_Read(&temp) == 0)
		;
	// Wait for screen release
	while (LCD_TS_Read(&temp) == 1) {
		position->X = temp.X;
		position->Y = temp.Y;
	}
	return;
}

#if 0
/* Test touchscreen by printing touch coordinates and drawing lines where touched. */
void LCD_TS_Test(void) {
	PT_T p, bp, pp;
	COLOR_T c;
	char buffer[32];
	
	LCD_Erase();

	pp.X = 0;
	pp.Y = 0;
	
	bp.X = COL_TO_X(0);
	bp.Y = ROW_TO_Y(0);
	c.R = 255;
	c.G = 100;
	c.B = 100;
	
	while (1) {
		if (LCD_TS_Read(&p)) {
			bp.X = 0;
			sprintf(buffer, "X:%5d Y:%5d", p.X, p.Y);
			LCD_Text_PrintStr(&bp, buffer);

#if 0
			LCD_Plot_Pixel(&p, &c);
			p.X++;
			LCD_Plot_Pixel(&p, &c);
			p.X-=2;
			LCD_Plot_Pixel(&p, &c);
			p.X++;
			
			p.Y++;
			LCD_Plot_Pixel(&p, &c);
			p.Y-=2;
			LCD_Plot_Pixel(&p, &c);
#else
			if ((pp.X == 0) && (pp.Y == 0)) {
				pp = p;
			}
			LCD_Draw_Line(&p, &pp, &c);
			pp = p;
#endif
		} else {
			pp.X = 0;
			pp.Y = 0;
	
			bp.X = 0;
			LCD_Text_PrintStr(&bp, "Not Pressed    ");
		}
	}
}
#endif

/* Calibrate touchscreen. */
void LCD_TS_Calibrate(void) {
	PT_T p, bp, p_bound;
	uint32_t i;
	
	bp.X = COL_TO_X(0);
	bp.Y = ROW_TO_Y(3);

	LCD_Text_PrintStr(&bp, "Calibrate TS");
	bp.X = COL_TO_X(0);
	bp.Y = ROW_TO_Y(4);
	LCD_Text_PrintStr(&bp, "by pressing each +");
	
	bp.X = COL_TO_X(0);
	bp.Y = ROW_TO_Y(0);
	LCD_Text_PrintStr(&bp, "+");
	
	p_bound.X = 0xffff;
	p_bound.Y = 0xffff;
	for (i=0; i<TS_CALIB_SAMPLES; i++){
		while (!LCD_TS_Read(&p))
			;
		if (p.X < p_bound.X)
			p_bound.X = p.X;
		if (p.Y < p_bound.Y)
			p_bound.Y = p.Y;
	}
	
	LCD_TS_X_Offset = p_bound.X;
	LCD_TS_Y_Offset = p_bound.Y;
	
	TS_Min = p_bound;
	
	// Wait for release
	while (LCD_TS_Read(&p))
		;
	
	LCD_Erase();

	/*
	bp.X = COL_TO_X(LCD_MAX_COLS-1);
	bp.Y = ROW_TO_Y(LCD_MAX_ROWS-1);
	*/
	bp.X = LCD_WIDTH - CHAR_WIDTH - 1;
	bp.Y = LCD_HEIGHT - CHAR_HEIGHT - 1;
	LCD_Text_PrintStr(&bp, "+");

	p_bound.X = 0;
	p_bound.Y = 0;
	for (i=0; i<TS_CALIB_SAMPLES; i++){
		while (!LCD_TS_Read(&p))
			;
		if (p.X > p_bound.X)
			p_bound.X = p.X;
		if (p.Y > p_bound.Y)
			p_bound.Y = p.Y;
	}
	
	TS_Max = p_bound;
	
	LCD_TS_X_Scale = (p_bound.X-LCD_TS_X_Offset)/LCD_WIDTH;
	LCD_TS_Y_Scale = (p_bound.Y-LCD_TS_Y_Offset)/LCD_HEIGHT;

	// Wait for release
	while (LCD_TS_Read(&p))
		;
	
	LCD_TS_Calibrated = 1;

}
