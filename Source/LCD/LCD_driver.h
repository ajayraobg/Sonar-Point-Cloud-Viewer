#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>

#define CTLR_ILI9341  (1)
#define CTLR_ST7789   (2)
#define CTLR_T6963  	(3)

// #define LCD_CONTROLLER (CTLR_ILI9341) 
#define LCD_CONTROLLER (CTLR_ST7789) 
// #define LCD_CONTROLLER (CTLR_T6963) 

#define SHIELD_VERSION (9)

// Backlight
#define LCD_BL_TPM (TPM1)
#define LCD_BL_TPM_CHANNEL (0)
#define LCD_BL_TPM_FREQ (20000)
#define LCD_BL_PERIOD ((SystemCoreClock)/(2*(LCD_BL_TPM_FREQ)))

// Touchscreen Hardware Interface
#if (SHIELD_VERSION == 9)
#define LCD_TS_YD_CHANNEL (14)
#define LCD_TS_YU_CHANNEL (3)
#define LCD_TS_XL_CHANNEL (15) 
#define LCD_TS_XR_CHANNEL (7)

#define LCD_TS_YD_PORT (PORTC)
#define LCD_TS_XL_PORT (PORTC)
#define LCD_TS_YU_PORT (PORTE)
#define LCD_TS_XR_PORT (PORTE)

#define LCD_TS_YD_PT (PTC)
#define LCD_TS_XL_PT (PTC)
#define LCD_TS_YU_PT (PTE)
#define LCD_TS_XR_PT (PTE)

#define LCD_TS_YD_BIT (0)
#define LCD_TS_XL_BIT (1)
#define LCD_TS_YU_BIT (22)
#define LCD_TS_XR_BIT (23)

#elif (SHIELD_VERSION < 9)

#define LCD_TS_YD_CHANNEL (0)
#define LCD_TS_YU_CHANNEL (3)
#define LCD_TS_XL_CHANNEL (4) 
#define LCD_TS_XR_CHANNEL (7)

#define LCD_TS_YD_PORT (PORTE)
#define LCD_TS_XL_PORT (PORTE)
#define LCD_TS_YU_PORT (PORTE)
#define LCD_TS_XR_PORT (PORTE)

#define LCD_TS_YD_PT (PTE)
#define LCD_TS_XL_PT (PTE)
#define LCD_TS_YU_PT (PTE)
#define LCD_TS_XR_PT (PTE)

#define LCD_TS_YD_BIT (20)
#define LCD_TS_XL_BIT (21)
#define LCD_TS_YU_BIT (22)
#define LCD_TS_XR_BIT (23)

#else

#error "Must specify SHIELD_VERSION in LCD_driver.h"
#endif


// Touchscreen Configuration
#define TS_DELAY (1)
#define TS_CALIB_SAMPLES (10)

/**************************************************************/
#define	GPIO_ResetBit(pos)	(FPTC->PCOR = MASK(pos))
#define	GPIO_SetBit(pos) 		(FPTC->PSOR = MASK(pos))
#define GPIO_Write(cmd) 		FPTC->PDOR &= ~LCD_DATA_MASK; \
														FPTC->PDOR |= (cmd & 0xff) << LCD_DB8_POS;
/**************************************************************/

#define LCD_CTRL_INIT_SEQ_END 0
#define LCD_CTRL_INIT_SEQ_CMD 1
#define LCD_CTRL_INIT_SEQ_DAT 2

typedef struct {
	uint8_t Type;  // 0: end, 1: command, 2: data
	uint8_t Value;
} LCD_CTLR_INIT_SEQ_T; // sequence of commands and data for initializing LCD controller

#endif
