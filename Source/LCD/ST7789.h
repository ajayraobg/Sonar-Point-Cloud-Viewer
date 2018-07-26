#ifndef ST7789_H
#define ST7789_H

#include "LCD_driver.h"

#if (((LCD_CONTROLLER)==(CTLR_ILI9341)) || ((LCD_CONTROLLER)==(CTLR_ST7789)))

extern const LCD_CTLR_INIT_SEQ_T Init_Seq_ST7789[];
extern const LCD_CTLR_INIT_SEQ_T Init_Seq_ILI9341[];

#define LCD_WIDTH (240)
#define LCD_HEIGHT (320)

#define BITS_PER_PIXEL				(24)


// TFT LCD Hardware Interface
// signals -- all on port C
#define LCD_DB8_POS (3)
#define LCD_DB15_POS (10)
#define LCD_D_NC_POS (12)
#define LCD_NWR_POS (13)
#define LCD_NRD_POS (16)
#define LCD_NRST_POS (17)
#define LCD_DATA_MASK (((unsigned )0x0ff) << LCD_DB8_POS)

#endif // LCD Controller

#endif // ST7789_H

