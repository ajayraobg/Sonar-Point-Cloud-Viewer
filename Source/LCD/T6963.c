
/********************************************
 * Filename: t6963.c                        *
 *                                          *
 * Description: Toshiba LCD Driver Functions*
 *                                          *
 * by Richard Taylor                        *
 *                                          *
 * Date: 09/02/2003                         *
 *******************************************/
/* includes */
#include "MKL25Z4.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "gpio_defs.h"
#include "timers.h"

#if (LCD_CONTROLLER == CTLR_T6963)
#include "T6963.h"

unsigned char FrameBuffer[LCD_GRAPHICS_WIDTH_BYTES][LCD_GRAPHICS_HEIGHT]; 

void delay_us(unsigned a) { // Probably not tuned for KL25Z! 
 	volatile int i;
	for (i=0; i<a; i++) {
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}
		
}

void delay_ms(unsigned a) { // Probably not tuned for KL25Z! 
 	volatile long i;
	for (i=0; i<a*100; i++)
		;
}

#if 0
void GrLCD_setup_test(void) {
    unsigned char n, address_h, address_l, x, y, color, step;
    unsigned int address, ex;

    /* Clear Graphic RAM */
    GrLCD_clear_graphics();
    /* Clear Text RAM */
    GrLCD_clear_text();

    /* Set Graphics Mode On and Text Mode Off */
    GrLCD_write_command(LCD_DISPLAY_MODES_GRAPHICS_ON);
    GrLCD_clear_graphics();

    // GrLCD_write_text('A', 0, 0); // 64 ASCII

		while (1) {
//			/* Fill display with text */
//			n = '@';
//			for (y=0; y<LCD_TEXT_HEIGHT; y++)
//				for (x=0; x<LCD_TEXT_WIDTH; x++) {
//					GrLCD_write_text(n, x, y);
//					n++;
//					if (n > 0x7f)
//							n = 0;
//				}

//			GrLCD_clear_text();
			color = 1;
			for (step = 16; step > 1; step--) {
				color = 1;
				do {
					for (x=0; x < LCD_GRAPHICS_WIDTH; x += step) {
						DrawLine(x, 0, LCD_GRAPHICS_WIDTH-x, LCD_GRAPHICS_HEIGHT-1, color); 
						GrLCD_refresh();
					}	
					for (y=0; y < LCD_GRAPHICS_HEIGHT; y += step) {
						DrawLine(LCD_GRAPHICS_WIDTH-1, y, 0, LCD_GRAPHICS_HEIGHT-y, color);  
						GrLCD_refresh();
					}	
				} while (color--);
				GrLCD_fill_screen_with_byte(0);
			}
				
			// code for testing selective segment draw/erase optimization
			n = 0;
			GrLCD_draw_seg_digit(n, 0, 0, 1);
			GrLCD_draw_seg_digit(n, W_SEG+7, 0, 1);
			GrLCD_draw_seg_digit(n, 2*(W_SEG+8)-1, 0, 1);	
			for (n=1; n<11; n++) {
				GrLCD_update_seg_digit(n, n-1, 0, 0);
				GrLCD_update_seg_digit(n, n-1, W_SEG+7, 0);
				GrLCD_update_seg_digit(n, n-1, 2*(W_SEG+8)-1, 0);	
				GrLCD_refresh();
				// delay_ms(300);
			}

			for (n=0; n<11; n++) {
				GrLCD_draw_seg_digit(n, 0, 0, 1);
				GrLCD_draw_seg_digit(n+1, W_SEG+7, 0, 1);
				GrLCD_draw_seg_digit(n+2, 2*(W_SEG+8)-1, 0, 1);	
				GrLCD_draw_seg_digit(n+3, 3*(W_SEG+8)-1, 0, 1);	
				GrLCD_draw_seg_digit(n+4, 4*(W_SEG+8)-1, 0, 1);	
				GrLCD_refresh();
			  //	delay_ms(14000);
				GrLCD_clear_graphics();
				GrLCD_refresh();
			}
		}
}
#endif

void Test_LCD_Plot_Speed(void) {
  unsigned char x, y;

  for (y=0; y<LCD_GRAPHICS_HEIGHT; y++)
    for (x=0; x<LCD_GRAPHICS_WIDTH; x++)
      GrLCD_set_pixel(x,y);
  for (y=0; y<LCD_GRAPHICS_HEIGHT; y++)
    for (x=0; x<LCD_GRAPHICS_WIDTH; x++)
      GrLCD_clear_pixel(x,y);
}

void LCD_Init(void)
{
	int i,j;
	// Clear local frame buffer
	for (j=0; j<LCD_GRAPHICS_HEIGHT; j++) {
		for (i=0; i<LCD_GRAPHICS_WIDTH_BYTES; i++) {
			FrameBuffer[i][j] = 0;
		}
	}
	ENABLE_LCD_PORT_CLOCKS
	
	/* Select GPIO for port C bits */
	PIN_CONTROL_PORT->PCR[PIN_CD_SHIFT] = PORT_PCR_MUX(1);
	PIN_CONTROL_PORT->PCR[PIN_WR_SHIFT] = PORT_PCR_MUX(1);
	PIN_CONTROL_PORT->PCR[PIN_RD_SHIFT] = PORT_PCR_MUX(1);
	PIN_CONTROL_PORT->PCR[PIN_CE_SHIFT] = PORT_PCR_MUX(1);
	PIN_CONTROL_PORT->PCR[PIN_RST_SHIFT] = PORT_PCR_MUX(1);

	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+1] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+2] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+3] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+4] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+5] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+6] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+7] = PORT_PCR_MUX(1);
	
   /* Set Control Lines to outputs and High */
	PIN_CONTROL_PT->PDDR |= PINS_CONTROL;
	PIN_CONTROL_PT->PDOR |= PINS_CONTROL;
  // LCD_CONTROL_DIR =   LCD_WR | LCD_RD | LCD_CE | LCD_CD;

  
  /* Set data bus to inputs */
	SET_LCD_DATA_DIR_IN;
  
  /* Hold Reset Line for 1ms */
  CLEAR_BITMASK(PIN_CONTROL_PT, PIN_RST);
  delay_us(2000);
  SET_BITMASK(PIN_CONTROL_PT, PIN_RST);

  /* Set Char Gen Up */
  GrLCD_write_command(LCD_CG_ROM_MODE_OR);

  /* Set Graphic Home Address */
  GrLCD_write_data(LCD_GRAPHICS_HOME & 0xff);
  GrLCD_write_data(LCD_GRAPHICS_HOME >> 8);
  GrLCD_write_command(LCD_GRAPHIC_HOME_ADDRESS_SET);
  
  /* Set Graphic Area */
  GrLCD_write_data(LCD_GRAPHICS_WIDTH_BYTES); 
  GrLCD_write_data(0x00);
  GrLCD_write_command(LCD_GRAPHIC_AREA_SET);
  
  /* Set Text Home Address */
  GrLCD_write_data(LCD_TEXT_HOME & 0xff);
  GrLCD_write_data(LCD_TEXT_HOME >> 8);
  GrLCD_write_command(LCD_TEXT_HOME_ADDRESS_SET);
  
  /* Set Text Area */
  GrLCD_write_data(LCD_TEXT_WIDTH); 
  GrLCD_write_data(0x0);
  GrLCD_write_command(LCD_TEXT_AREA_SET);

  /* Set offset area for CG RAM*/
  GrLCD_write_data(LCD_CG_RAM_HOME>>11);
  GrLCD_write_data(0x0);
  GrLCD_write_command(LCD_OFFSET_REGISTER_SET);
  
  /* Set Text Mode and Graphics Mode On */
  GrLCD_write_command(LCD_DISPLAY_MODES_GRAPHICS_ON | LCD_DISPLAY_MODES_TEXT_ON);

  /* Clear Graphic RAM */
	GrLCD_clear_graphics();
  /* Clear Character generator tables -- really need to load them */
	//  GrLCD_clear_CGRAM();
  /* Clear Text RAM */
  GrLCD_clear_text();

	GrLCD_write_command(LCD_DISPLAY_MODES_GRAPHICS_ON);

	
}

/********************************************
 * Function name: GrLCD_write_data            *
 * Description:   Write Data to LCD         *
 *******************************************/
void GrLCD_write_data(unsigned char data)
{
    /* While BUSY1 and BUSY2 are not 1 */
    while ( (GrLCD_read_status() & (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2)) 
				!= (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2))
				;

    /* Clear C/D# */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Output */
		SET_LCD_DATA_DIR_OUT;
		
		/* write data */
		SET_LCD_DATA_OUT(data);
		
    /* Clear CE and WR, set RD */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CE);
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_WR);

    delay_us(WRITE_DELAY_US);

    /* Set CE and RD*/
    SET_BITMASK(PIN_CONTROL_PT, PIN_CE);
    SET_BITMASK(PIN_CONTROL_PT, PIN_WR);
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Input */
		SET_LCD_DATA_DIR_IN;
}

/********************************************
 * Function name: GrLCD_read_data             *
 * Description:   Read Data From LCD        *
 *******************************************/
unsigned char GrLCD_read_data(void)
{
    unsigned char data;
	
#if FAKE_READ
		delay_us(FAKE_READ_DELAY_US);
		data = 0xff;
#else
    /* While BUSY1 and BUSY2 are not 1 */
    while ( (GrLCD_read_status() & (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2)) 
					!= (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2))
				;

    /* Clear C/D# */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Input */
		SET_LCD_DATA_DIR_IN;
		

    /* Clear CE and RD, set WR */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CE);
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_RD);

    delay_us(3);
    /* Read Data Bus */
    data = GET_LCD_DATA_IN;

    /* Set CE and RD*/
    SET_BITMASK(PIN_CONTROL_PT, PIN_CE);
    SET_BITMASK(PIN_CONTROL_PT, PIN_RD);
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);
#endif
    return data;
}

/********************************************
 * Function name: GrLCD_write_command         *
 * Description:   Write Command to LCD      *
 *******************************************/
void GrLCD_write_command(unsigned char data)
{
    unsigned char status;

    /* While BUSY1 and BUSY2 are not 1 */
    while ( (GrLCD_read_status() & (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2)) 
				!= (LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2))
				;

    /* Set C/D# */
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Output */
		SET_LCD_DATA_OUT(data);
		SET_LCD_DATA_DIR_OUT;
    
    /* Clear CE and WR, set RD */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CE);
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_WR);

    delay_us(WRITE_DELAY_US);
    
    /* Set CE and RD*/
    SET_BITMASK(PIN_CONTROL_PT, PIN_CE);
    SET_BITMASK(PIN_CONTROL_PT, PIN_WR);
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Input */
    SET_LCD_DATA_DIR_IN;
}

/********************************************
 * Function name: GrLCD_read_status           *
 * Description:   Read Status From LCD      *
 *******************************************/
unsigned char GrLCD_read_status(void)
{
    unsigned char data;

#if FAKE_READ
	delay_us(FAKE_READ_DELAY_US);
	data = LCD_STATUS_BUSY1 | LCD_STATUS_BUSY2;
#else
	   /* Set C/D# */
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);

    /* Set Data Lines to Input */
		SET_LCD_DATA_DIR_IN;
	
    /* Clear CE and RD */
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_CE);
    CLEAR_BITMASK(PIN_CONTROL_PT, PIN_RD);

    delay_us(1);

    /* Read Data Bus */
    data = GET_LCD_DATA_IN;

    /* Set All Bits */
    SET_BITMASK(PIN_CONTROL_PT, PIN_CE);
    SET_BITMASK(PIN_CONTROL_PT, PIN_RD);
    SET_BITMASK(PIN_CONTROL_PT, PIN_CD);
#endif
    return data;
}

/********************************************
 * Function name: GrLCD_clear_graphics        *
 * Description:   Wipe Graphics RAM         *
 *******************************************/
void GrLCD_clear_graphics(void)
{
    unsigned char address_l, address_h;
    unsigned int address, address_limit;

    /* Set Address Pointer */
    address = LCD_GRAPHICS_HOME;
    address_l = address & 0xff;
    address_h = address >> 8;
    GrLCD_write_data(address_l);
    GrLCD_write_data(address_h);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

    address_limit = (LCD_GRAPHICS_HOME + LCD_GRAPHICS_SIZE);

    while (address < address_limit) // + 640
    {
				GrLCD_write_data(LCD_ON_BYTE);
				GrLCD_write_command(LCD_DATA_WRITE_NO_INCREMENT);
			
				GrLCD_write_data(LCD_OFF_BYTE);
				GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
				address = address + 1;	
    }
		
    if (LCD_NUMBER_OF_SCREENS == 0x02)
    {
        /* Set Address Pointer */
        address = LCD_GRAPHICS_HOME + 0x8000;
        address_l = address & 0xff;
        address_h = address >> 8;
        GrLCD_write_data(address_l);
        GrLCD_write_data(address_h);
        GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

        address_limit = (LCD_GRAPHICS_HOME + LCD_GRAPHICS_SIZE +0x8000);
        while (address < address_limit)
        {
            GrLCD_write_data(LCD_OFF_BYTE);
            GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
            address = address + 1;
        }
    }

}

/********************************************
 * Function name: GrLCD_clear_text            *
 * Description:   Wipe Text RAM             *
 *******************************************/
void GrLCD_clear_text(void)
{
  unsigned char address_l, address_h;
  unsigned int address, address_limit;
  /* Set Address Pointer */
  address = LCD_TEXT_HOME;
  address_l = address & 0xff;
  address_h = address >> 8;
  GrLCD_write_data(address_l);
  GrLCD_write_data(address_h);
  GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
  
  address_limit =  (LCD_TEXT_HOME + LCD_TEXT_SIZE);
  while (address < address_limit)
    {
			GrLCD_write_data('X');
			GrLCD_write_command(LCD_DATA_WRITE_NO_INCREMENT);
      GrLCD_write_data(0x00);
      GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
      address = address + 1;
    }
  
  if (LCD_NUMBER_OF_SCREENS == 0x02)
    {
      /* Set Address Pointer */
      address = LCD_TEXT_HOME + 0x8000;
      address_l = address & 0xff;
      address_h = address >> 8;
      GrLCD_write_data(address_l);
      GrLCD_write_data(address_h);
      GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
      
      address_limit =  (LCD_TEXT_HOME + LCD_TEXT_SIZE + 0x8000);
      while (address < address_limit)
        {
					GrLCD_write_data(0x00);
					GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
					address = address + 1;
        }
    }
}

void GrLCD_clear_CGRAM(void) {
  unsigned char address_l, address_h, i;
  unsigned int address, address_limit;
  /* Set Address Pointer */
  address = LCD_CG_RAM_HOME;
  address_l = address & 0xff;
  address_h = address >> 8;
  GrLCD_write_data(address_l);
  GrLCD_write_data(address_h);
  GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
  
  address_limit =  (LCD_CG_RAM_HOME + LCD_CG_RAM_SIZE);
  i = 0;
  while (address < address_limit) {
    GrLCD_write_data(i++);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
    address = address + 1;
  }
}

/********************************************
 * Function name: GrLCD_write_text            *
 * Description:   Write Character to X, Y   *
 *                0 <= X <= LCD Text Width  *
 *                0 <= Y <= LCD Text Height *
 *******************************************/
void GrLCD_write_text(unsigned char character, unsigned char x, unsigned char y)
{
    unsigned int address;


    if ((y > 7) && (LCD_NUMBER_OF_SCREENS == 2)) /* If we are on the second screen and it exists */
    {
      address = ((y-8) * LCD_TEXT_WIDTH) + x + LCD_TEXT_HOME + 0x8000;
    } else 
      address = (y * LCD_TEXT_WIDTH) + x + LCD_TEXT_HOME;

    character = character - 0x20;   /* Adjust standard ASCII to T6963 ASCII */

    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
    GrLCD_write_data(character);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
}
/********************************************
 * Function name: GrLCD_write_string        *
 * Description:   Write string starting at X, Y   *
 *                0 <= X <= LCD Text Width  *
 *                0 <= Y <= LCD Text Height *
 *                Truncate if off the screen*
 *******************************************/
void GrLCD_write_string(unsigned char * str, unsigned char x, unsigned char y)
{
  while (*str && (x<LCD_TEXT_WIDTH)) {
    GrLCD_write_text(*str++, x++, y);
  }
}

void LCD_Plot_Pixel(PT_T * p, COLOR_T * color) {
uint32_t x, y;
	x = p->X;
	y = p->Y;
#if USE_LOCAL_FRAME_BUFFER
	if (color->G > 0)
		FrameBuffer[x/8][y] |= 1 << (7 - (x&7));
	else
		FrameBuffer[x/8][y] &= ~(1 << (7 - (x&7)));

#else
		unsigned char data;
    unsigned int address, shift;

    address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;

    if (y > 63 && LCD_NUMBER_OF_SCREENS == 2) /* If we are on the second screen and it exists */
    {
        y = y - 64;
        address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;
        address = address + 0x8000;
    }

    data = 1 << (7 - (x % 8));

    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

#if 1  // 0 = overwrite
    /* Read existing display */
    GrLCD_write_command(LCD_DATA_READ_NO_INCREMENT);
    if (color)
      data = data | GrLCD_read_data();
    else
      data = (~data) & GrLCD_read_data();
#endif
		
    /* Write modified data */
    GrLCD_write_data(data);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
#endif
}


/********************************************
 * Function name: GrLCD_set_pixel             *
 * Description:   Set a single Pixel on     *
 *                0 <= X <= LCD Width       *
 *                0 <= Y <= LCD Height      *
 *******************************************/
void GrLCD_set_pixel(unsigned char x, unsigned char y)
{
	#if USE_LOCAL_FRAME_BUFFER
		FrameBuffer[x/8][y] |= 1 << (7 - (x&7));
	#else
    unsigned char data;
    unsigned int address, shift;

    address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;

    if (y > 63 && LCD_NUMBER_OF_SCREENS == 2) /* If we are on the second screen and it exists */
    {
        y = y - 64;
        address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;
        address = address + 0x8000;
    }

    data = 1 << (7 - (x % 8));

    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

    /* Read existing display */
    GrLCD_write_command(LCD_DATA_READ_NO_INCREMENT);
    data = data | GrLCD_read_data();

    /* Write modified data */
    GrLCD_write_data(data);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
	#endif
}


/********************************************
 * Function name: GrLCD_fill_screen_with_byte *
 * Description:   fill screen!
 *******************************************/
void LCD_Fill_Buffer(COLOR_T * color) {
  unsigned char x, y, fill_byte;
	if (color->G > 0)
		fill_byte = 0xff;
	else
		fill_byte = 0;
	
  for (y=0; y<LCD_GRAPHICS_HEIGHT; y++)
    for (x=0; x<LCD_GRAPHICS_WIDTH; x+=8)
      GrLCD_fill_byte(fill_byte, x, y);
}

void LCD_Plot_Packed_Pixels(uint8_t fill_byte, PT_T * pos) {
		FrameBuffer[pos->X/8][pos->Y] = fill_byte;
}

void LCD_Plot_Packed_Pixels_Unaligned(uint8_t fill_byte, uint8_t r_shift, PT_T * pos) {
	uint8_t col = pos->X/8;
	volatile uint8_t t1, t2;

	FrameBuffer[col][pos->Y] &= ~((uint8_t) 0xff >> r_shift);
	t1 = fill_byte >> r_shift;
	FrameBuffer[col][pos->Y] |= t1;
	//	LCD_Refresh();
	
	FrameBuffer[col+1][pos->Y] &= ((uint8_t) 0xff << (8-r_shift));
	t2 = fill_byte << (8-r_shift);
	FrameBuffer[col+1][pos->Y] |= t2;
	//	LCD_Refresh();
}


/********************************************
 * Function name: GrLCD_fill_byte             *
 * Description:   Set a byte of pixels      *
 *                x: start pixel            *
 *                0 <= X <= LCD Width       *
 *                0 <= Y <= LCD Height      *
 *******************************************/
void GrLCD_fill_byte(unsigned char fill_byte, unsigned char x, unsigned char y)
{
#if USE_LOCAL_FRAME_BUFFER
		FrameBuffer[x/8][y] = fill_byte;
#else
    unsigned char data;
    unsigned int address;

    address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;

    if (y > 63 && LCD_NUMBER_OF_SCREENS == 2) /* If we are on the second screen and it exists */
    {
        y = y - 64;
        address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;
        address = address + 0x8000;
    }

    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

    /* Write  data */
    GrLCD_write_data(fill_byte);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
#endif
}


/********************************************
 * Function name: GrLCD_display_bitmap      *
 * Description:   p: Pointer to first byte  *
 *******************************************/
void GrLCD_display_bitmap(unsigned char * p)
{
  unsigned int address, n;

    /* Write  data for first half of screen */
    address = LCD_GRAPHICS_HOME;    
    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

    for (n = 0; n<64*LCD_GRAPHICS_WIDTH_BYTES  ; n++) {
      GrLCD_write_data(*p++);
      GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
    }

    /* write data for second half of screen */
    address = LCD_GRAPHICS_HOME + 0x8000;
    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
    for (n = 0; n<64*LCD_GRAPHICS_WIDTH_BYTES  ; n++) {
      GrLCD_write_data(*p++);
      GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
    }
}

void LCD_Refresh(void) {
	unsigned int address, n, x, y;

	address = LCD_GRAPHICS_HOME;		
	GrLCD_write_data(address & 0xff);
	GrLCD_write_data(address >> 0x08);
	GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
#if 0 // 18 ms per refresh
	for (y = 0; y < 128; y++) { 
		for (x = 0; x < LCD_GRAPHICS_WIDTH_BYTES; x++) {
			GrLCD_write_data(FrameBuffer[x][y]);
			GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
		}
	}
#else // 9 ms per refresh
	GrLCD_write_command(LCD_DATA_AUTO_WRITE_SET);
	for (y = 0; y < 128; y++) {
		for (x = 0; x < LCD_GRAPHICS_WIDTH_BYTES; x++) {
			GrLCD_write_data(FrameBuffer[x][y]);
		}
	}
	GrLCD_write_command(LCD_DATA_AUTO_RESET);
#endif
}

/********************************************
 * Function name: GrLCD_clear_pixel           *
 * Description:   Clear a single Pixel      *
 *                0 <= X <= LCD Width       *
 *                0 <= Y <= LCD Height      *
 *******************************************/
void GrLCD_clear_pixel(unsigned char x, unsigned char y)
{

#if USE_LOCAL_FRAME_BUFFER
		FrameBuffer[x/8][y] &= ~(1 << (7 - (x&7)));
#else
    unsigned char data;
    unsigned int address;
  	
    address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;

    if (y > 63 && LCD_NUMBER_OF_SCREENS == 2) /* If we are on the second screen and it exists */
    {
        y = y - 64;
        address = (y * LCD_GRAPHICS_WIDTH_BYTES) + (x / 8) + LCD_GRAPHICS_HOME;
        address = address + 0x8000;
    }

    data = 1 << (7 - (x % 8));

    GrLCD_write_data(address & 0xff);
    GrLCD_write_data(address >> 0x08);
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);

    /* Read existing display */
    GrLCD_write_command(LCD_DATA_READ_NO_INCREMENT);
    data = (~data) & GrLCD_read_data();

    /* Write modified data */
    GrLCD_write_data(data);
    GrLCD_write_command(LCD_DATA_WRITE_AUTO_INCREMENT);
#endif
}

#if 0 
void GrLCD_DrawRectangle(unsigned char x1, unsigned char y1,
			 unsigned char x2, unsigned char y2, unsigned char c){
  unsigned char x, y, t, b;
  x1 = MIN(x1, LCD_GRAPHICS_WIDTH);
  x2 = MIN(x2, LCD_GRAPHICS_WIDTH);
  y1 = MIN(y1, LCD_GRAPHICS_HEIGHT);
  y2 = MIN(y2, LCD_GRAPHICS_HEIGHT);

  if (x1 > x2) {
    t = x1;
    x1 = x2;
    x2 = t;    
  }

  if (y1 > y2) {
    t = y1;
    y1 = y2;
    y2 = t;    
  }

	b = (c ^ LCD_INVERT) ? 0xff : 0x00;
 	for (y=y1; y<=y2; y++) {
 		// Prologue
   	for (x=x1; (x<=x2) && (x & 0x07); x++) {
			GrLCD_plot_pixel(x, y, c ^ LCD_INVERT);
		}
		// Body
		for ( ; (x <= x2-7); x += 8) {
			GrLCD_fill_byte(b, x, y);
		}
		// Epilogue
		for ( ; x < x2; x++) {
			GrLCD_plot_pixel(x, y, c ^ LCD_INVERT);
		}
 	}
}
#endif
	
void GrLCD_set_page(unsigned int adx) {
	   /* Set Address Pointer To GRAPHIC Home */
    GrLCD_write_data(adx & 0x00ff); /* LSB */
    GrLCD_write_data(adx >> 8); /* MSB */
    GrLCD_write_command(LCD_ADDRESS_POINTER_SET);
}

void GrLCD_page_test(void) {
	unsigned int adx;
	
	for (adx = LCD_GRAPHICS_HOME; adx < 0x4000; adx += 0x0100) {
		GrLCD_set_page(adx);
		/* GrLCD_DrawRectangle(10, 10, 50, 50, 1); */
		delay_ms(1);
	}
}

 void LCD_Fill_Rectangle(PT_T * p1, PT_T * p2, COLOR_T * color){
 }
 
 uint32_t LCD_Start_Rectangle(PT_T * p1, PT_T * p2) {
	 return 0;
 }
 
 void LCD_Write_Rectangle_Pixel(COLOR_T * color, unsigned int count) {
 }


#endif // LCD controller
