#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

void LCD_TS_Init(void);
uint32_t LCD_TS_Read(PT_T * position);
void LCD_TS_Blocking_Read(PT_T * position);
void LCD_TS_Calibrate(void);
void LCD_TS_Test(void);

#endif

