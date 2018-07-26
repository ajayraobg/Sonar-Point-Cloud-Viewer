#ifndef T6963_H
#define T6963_H

#include "LCD_driver.h"
/********************************************
 * Filename: t6963.h                        *
 *                                          *
 * Description: Header file for t6963.c     *
 *                                          *
 *                                          *
 * Date: 06/02/2003                         *
 *******************************************/
 
#if ((LCD_CONTROLLER)==(CTLR_T6963))
 
#define BITS_PER_PIXEL				(1)
 
#define FAKE_READ 						1
#define FAKE_READ_DELAY_US		0 // Need at least 20 for text to work
#define WRITE_DELAY_US				3

#define USE_LOCAL_FRAME_BUFFER 1
 
#define LCD_WIDTH (160)
#define LCD_HEIGHT (128)
 
/* LCD Parameters */
#define LCD_NUMBER_OF_SCREENS   0x01   

// To Do - fix bug when character width is 8 and FS is 1
#define LCD_CHARACTER_WIDTH     8    /* Is character 8x8 or 6x8 (0x08 / 0x06) - see Font Select signal */
#define LCD_CHARACTER_HEIGHT    8    /* Is character 8x8 or 6x8 (0x08 / 0x06) */

#define LCD_GRAPHICS_WIDTH      LCD_WIDTH   /* Width of display (0xA0 = 160 pixels, 0xF0 = 240 pixels) */
#define LCD_GRAPHICS_HEIGHT     LCD_HEIGHT    /* Height of display (0x80 = 128 pixels) */
#define LCD_GRAPHICS_WIDTH_BYTES   (LCD_GRAPHICS_WIDTH/8)    /* A graphic character is 8 bits wide (same as 8x8 char) */
#define LCD_GRAPHICS_SIZE   (LCD_GRAPHICS_WIDTH_BYTES * LCD_GRAPHICS_HEIGHT)  /* Size of graphics RAM - was 0x0800*/

#define LCD_TEXT_WIDTH          (LCD_GRAPHICS_WIDTH/LCD_CHARACTER_WIDTH)    /* Text Width = Graphics Width / Character Width */
#define LCD_TEXT_HEIGHT         (LCD_GRAPHICS_HEIGHT/LCD_CHARACTER_HEIGHT)    	/* Text Height = Graphics Height / Character Height */

#define LCD_INVERT 0

#if LCD_INVERT
#define LCD_ON_BYTE (0)
#define LCD_OFF_BYTE (0xff)
#define LCD_ON_PIXEL (0)
#define LCD_OFF_PIXEL (1)
#else
#define LCD_ON_BYTE (0xff)
#define LCD_OFF_BYTE (0)
#define LCD_ON_PIXEL (1)
#define LCD_OFF_PIXEL (0)
#endif


/* Define the Memory Map */

// CFAG160128EYYHTZ
#define LCD_TEXT_HOME       0x0000  
#define LCD_TEXT_SIZE       0x0C00  /* Size of text RAM */
#define LCD_GRAPHICS_HOME   0x0C00  

#define LCD_CG_RAM_HOME     0x1800  /* start of CG RAM */
#define LCD_CG_RAM_SIZE     0x0800  /* length */


// #define LCD_TEXT_HOME       0x0A00  /* Graphics Area + Text Attribute Size (same size as text size) */

#define LCD_TEXT_WINDOW_START 6

/*  Memory Map for 160x128 pixel display */
/*  This display is made up of two screens */
/*  Both 160x64 pixels */

/*  Screen 1 */

/*  0x0000  ----------------------------- */
/*          | Graphic RAM Area          | */
/*          | 0x0000 to 0x07FF          | */
/*          | (256x64 pixels)           | */
/*  0x0800  ----------------------------- */
/*          | Text Attribute Area       | */
/*  0x0A00  ----------------------------- */
/*          | Text RAM Area             | */
/*          | 512 Bytes                 | */
/*          | (256x64 pixels)           | */
/*  0x0C00  ----------------------------- */

/*  Screen 2 (Automatically derived from Screen 1) */

/*  0x8000  ----------------------------- */
/*          | Graphic RAM Area          | */
/*          | 0x0000 to 0x07FF          | */
/*          | (256x64 pixels)           | */
/*  0x8800  ----------------------------- */
/*          | Text Attribute Area       | */
/*  0x8A00  ----------------------------- */
/*          | Text RAM Area             | */
/*          | 512 Bytes                 | */
/*          | (256x64 pixels)           | */
/*  0x8C00  ----------------------------- */

/* Connections from LCD to MCU port bits: 
 DB0 through DB7 are contiguous, starting with LSB at bit position PIN_DATA_SHIFT
 
  For example:
   - DB0 = PTC0
   - DB1 = PTC1
   - DB2 = PTC2
   - DB3 = PTC3
   - DB4 = PTC4
   - DB5 = PTC5
   - DB6 = PTC6
   - DB7 = PTC7
	 
   - C/D  = PTC8                                                              
	 - /WR  = PTC9
   - /RD  = PTC10
	 - /CE  = PTC11
	 - /RST = PTC12
																																				*/

#if 1 // New connections
#define PIN_DATA_PORT					PORTC
#define PIN_DATA_PT						PTC
#define PIN_DATA_SHIFT				( 3 )

#define PIN_CONTROL_PORT			PORTC
#define PIN_CONTROL_PT				PTC
#define PIN_CD_SHIFT					( 12 )
#define PIN_CD           			( 1 << PIN_CD_SHIFT)

#define PIN_WR_SHIFT						( 13 )
#define PIN_WR                 ( 1 << PIN_WR_SHIFT)

#define PIN_RD_SHIFT						( 16 )
#define PIN_RD                 ( 1 << PIN_RD_SHIFT)

#define PIN_CE_SHIFT						( 11 )
#define PIN_CE                 ( 1 << PIN_CE_SHIFT)

#define PIN_RST_SHIFT						( 17 )
#define PIN_RST                 ( 1 << PIN_RST_SHIFT)

#else // Old connections

#define PIN_DATA_PORT					PORTC
#define PIN_DATA_PT						PTC
#define PIN_DATA_SHIFT				( 0 )

#define PIN_CONTROL_PORT			PORTC
#define PIN_CONTROL_PT				PTC
#define PIN_CD_SHIFT					( 8 )
#define PIN_CD           			( 1 << PIN_CD_SHIFT)

#define PIN_WR_SHIFT						( 9 )
#define PIN_WR                 ( 1 << PIN_WR_SHIFT)

#define PIN_RD_SHIFT						( 10 )
#define PIN_RD                 ( 1 << PIN_RD_SHIFT)

#define PIN_CE_SHIFT						( 11 )
#define PIN_CE                 ( 1 << PIN_CE_SHIFT)

#define PIN_RST_SHIFT						( 12 )
#define PIN_RST                 ( 1 << PIN_RST_SHIFT)

#endif

#define PINS_CONTROL						(PIN_CD | PIN_WR | PIN_RD | PIN_CE | PIN_RST)
#define PINS_DATA								(0xff << PIN_DATA_SHIFT)

/* Enable Clock for peripheral driving LCD pins                               */
#define ENABLE_LCD_PORT_CLOCKS   	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	

#define SET_LCD_DATA_OUT(x)       PIN_DATA_PT->PDOR = (PIN_DATA_PT->PDOR & ~PINS_DATA) | ((x) << PIN_DATA_SHIFT);
#define GET_LCD_DATA_IN           (((PIN_DATA_PT->PDIR & PINS_DATA) >> PIN_DATA_SHIFT) & 0xFF)

/* Setting all pins to output mode                                            */
#define SET_LCD_ALL_DIR_OUT       { PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR | PINS_DATA; \
																PIN_E_PT->PDDR = PIN_E_PT->PDDR | PIN_E; \
																PIN_RW_PT->PDDR = PIN_RW_PT->PDDR | PIN_RW; \
																PIN_RS_PT->PDDR = PIN_RS_PT->PDDR | PIN_RS; }

/* Setting DATA pins to input mode                                            */
#define SET_LCD_DATA_DIR_IN       PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR & ~PINS_DATA;

/* Setting DATA pins to output mode                                           */
#define SET_LCD_DATA_DIR_OUT      PIN_DATA_PT->PDDR = PIN_DATA_PT->PDDR | PINS_DATA;

/*--- */

#define LCD_BUSY_FLAG_MASK				(0x80)

/* Control Word Definitions */
#define LCD_CURSOR_POINTER_SET          0x21 // 00100001b
#define LCD_OFFSET_REGISTER_SET         0x22 // 00100010b
#define LCD_ADDRESS_POINTER_SET         0x24 // 00100100b

#define LCD_TEXT_HOME_ADDRESS_SET       0x40 // 01000000b
#define LCD_TEXT_AREA_SET               0x41 // 01000001b
#define LCD_GRAPHIC_HOME_ADDRESS_SET    0x42 // 01000010b
#define LCD_GRAPHIC_AREA_SET            0x43 // 01000011b

#define LCD_CG_ROM_MODE_OR              0x80 // 10000000b
#define LCD_CG_ROM_MODE_EXOR            0x81 // 10000001b
#define LCD_CG_ROM_MODE_AND             0x83 // 10000011b
#define LCD_CG_ROM_MODE_TEXT            0x84 // 10000100b
#define LCD_CG_RAM_MODE_OR              0x88 // 10001000b
#define LCD_CG_RAM_MODE_EXOR            0x89 // 10001001b
#define LCD_CG_RAM_MODE_AND             0x8b // 10001011b
#define LCD_CG_RAM_MODE_TEXT            0x8c // 10001100b

/* 1001 0000 is all off, OR together for ON modes */
#define LCD_DISPLAY_MODES_ALL_OFF       0x90 // 10010000b
#define LCD_DISPLAY_MODES_GRAPHICS_ON   0x98 // 10011000b 
#define LCD_DISPLAY_MODES_TEXT_ON       0x94 // 10010100b 
#define LCD_DISPLAY_MODES_CURSOR_ON     0x92 // 10010010b
#define LCD_DISPLAY_MODES_CURSOR_BLINK  0x91 // 10010001b

/* Cursor Pattern Select */
#define LCD_CURSOR_PATTERN_UNDERLINE    0xa0 // 10100000b
#define LCD_CURSOR_PATTERN_BLOCK        0xa7 // 10100111b

/* Send Auto_XX Command, then block of data, then Auto_reset */
#define LCD_DATA_AUTO_WRITE_SET         0xb0 // 10110000b
#define LCD_DATA_AUTO_READ_SET          0xb1 // 10110001b
#define LCD_DATA_AUTO_RESET             0xb2 // 10110010b

/* Send R/W Then one byte Data */
#define LCD_DATA_WRITE_AUTO_INCREMENT   0xc0 // 11000000b
#define LCD_DATA_READ_AUTO_INCREMENT    0xc1 // 11000001b
#define LCD_DATA_WRITE_NO_INCREMENT     0xc4 // 11000100b
#define LCD_DATA_READ_NO_INCREMENT      0xc5 // 11000101b

/* Status Register Bits */
#define LCD_STATUS_BUSY1    0x01
#define LCD_STATUS_BUSY2    0x02
#define LCD_STATUS_DARRDY   0x04
#define LCD_STATUS_DAWRDY   0x08

#define LCD_STATUS_CLR      0x20
#define LCD_STATUS_ERR      0x40
#define LCD_STATUS_BLINK    0x80

typedef struct {
	unsigned char x;
	unsigned char y;
} Point_T ;

/* Definitions */
#define ALL_INPUTS  0x00
#define ALL_OUTPUTS 0xff

extern unsigned char FrameBuffer[LCD_GRAPHICS_WIDTH_BYTES][LCD_GRAPHICS_HEIGHT];

// #define LCD_Plot_Packed_Pixels(bits, pos) GrLCD_fill_byte(bits, pos->X, pos->Y)

/* Function Declarations */
// TODO: Update these based on unified API 

void GrLCD_initialise(void);
void GrLCD_write_data(unsigned char data);
unsigned char GrLCD_read_data(void);
void GrLCD_write_command(unsigned char data);
unsigned char GrLCD_read_status(void);
void GrLCD_clear_graphics(void);
void GrLCD_clear_text(void);
void GrLCD_clear_CGRAM(void);
void GrLCD_refresh(void);

void GrLCD_write_text(unsigned char character, unsigned char x, unsigned char y);
void GrLCD_write_string(unsigned char * str, unsigned char x, unsigned char y);
void GrLCD_fill_byte(unsigned char fill_byte, unsigned char x, unsigned char y);
void GrLCD_plot_pixel(unsigned char x, unsigned char y, unsigned char color);

void GrLCD_set_pixel(unsigned char x, unsigned char y);
void GrLCD_clear_pixel(unsigned char x, unsigned char y);

unsigned char GrLCD_bit_to_byte(unsigned char bit);
void GrLCD_fill_screen_with_byte(unsigned char fill_byte);

void GrLCD_DrawRectangle(unsigned char x1, unsigned char y1,
			 unsigned char x2, unsigned char y2, unsigned char c);

void GrLCD_setup_test(void);
void Test_LCD_Plot_Speed(void);
void GrLCD_display_bitmap(unsigned char * p);
/** Display one row of a bitmap from flash.
CSNo: dataflash chip to read from
p: address of start byte
width: number of bytes in row
row_num: row on display to put pixels onto
*/
void GrLCD_display_bitmap_row(unsigned char CSNo, unsigned long p, unsigned width, unsigned int row_num);
void GrLCD_set_page(unsigned int adx);
void GrLCD_page_test(void);

#define CLEAR_BITMASK(a,b)  (a->PCOR = b)
#define SET_BITMASK(a,b) 	(a->PSOR = b)

#endif

#endif // T6963_H

