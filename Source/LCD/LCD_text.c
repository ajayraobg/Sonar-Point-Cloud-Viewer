#include "font.h"
#include "LCD.h"
#include "LCD_driver.h"

#include "ST7789.h"
#include "T6963.h"

#include "delay.h"

const uint8_t * font;
FONT_HEADER_T * font_header;
GLYPH_INDEX_T * glyph_index; 

COLOR_T fg, bg;

uint8_t G_LCD_char_width, G_LCD_char_height;

// const uint8_t * fonts[] = {Lucida_Console8x13, Lucida_Console12x19}; // Lucida_Console20x31};
const uint8_t * fonts[] = {0, Lucida_Console12x19}; // Lucida_Console20x31};
const uint8_t char_widths[] = {8, 12, 20};
const uint8_t char_heights[] = {13, 19, 31};

uint8_t Bit_Reverse_Byte(uint8_t v) {
// http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
	// v: input bits to be reversed
	int r = v; // r will be reversed bits of v; first get LSB of v
	int s = sizeof(v) * 8 - 1; // extra shift needed at end

	for (v >>= 1; v; v >>= 1)
	{   
		r <<= 1;
		r |= v & 1;
		s--;
	}
	r <<= s; // shift when v's highest bits are zero
	return r;
}

void LCD_Text_Set_Colors(COLOR_T * foreground, COLOR_T * background) {
	fg.R = foreground->R;
	fg.G = foreground->G;
	fg.B = foreground->B;
	bg.R = background->R;
	bg.G = background->G;
	bg.B = background->B;
}

void LCD_Erase(void) {
	LCD_Fill_Buffer(&bg);
}

void LCD_Text_Init(uint8_t font_num) {
	
#if 0  // Code for multiple fonts not working yet
	font = fonts[font_num];
	G_LCD_char_width = char_widths[font_num];
	G_LCD_char_height = char_heights[font_num];
#endif
	
//#if 0 // This code is not working
//	font = Lucida_Console20x31;
//	G_LCD_char_width = 20;
//	G_LCD_char_height = 31;
	
#if 1 // This code is working
	font = Lucida_Console12x19;
	G_LCD_char_width = 12;
	G_LCD_char_height = 19;
#else
	font = Lucida_Console8x13;
	G_LCD_char_width = 8;
	G_LCD_char_height = 13;
#endif
	
	font_header = (FONT_HEADER_T *) font;
	glyph_index = (GLYPH_INDEX_T *) (font + sizeof(FONT_HEADER_T));
	
	// Set default FG and BG colors
	fg.R = 255;
	fg.G = 255;
	fg.B = 0;

	bg.R = 0;
	bg.G = 0;
	bg.B = 0;
}

uint8_t LCD_Text_GetGlyphWidth(char ch) {
	uint8_t glyph_index_entry;
	
	glyph_index_entry = ch - font_header->FirstChar;
	return glyph_index[glyph_index_entry].Width;
}
void LCD_Text_PrintChar(PT_T * pos, char ch) {
	uint8_t glyph_index_entry;
	const uint8_t * glyph_data; // start of the data
#if BITS_PER_PIXEL == 1 // Copy bitmap byte directly
	PT_T cur_pos;
#endif
	PT_T end_pos;
	COLOR_T * pixel_color;
	uint8_t bitmap_byte;
	uint8_t glyph_width, x_bm;
	uint32_t offset;
	uint32_t row, col, num_pixels;
	
	glyph_index_entry = ch - font_header->FirstChar;
	glyph_width = glyph_index[glyph_index_entry].Width;
	offset = glyph_index[glyph_index_entry].Offset;
	glyph_data = &(font[offset]);
	
#if BITS_PER_PIXEL == 1 // Copy bitmap byte directly
	cur_pos.Y = pos->Y;
	
	for (row = 0; row < CHAR_HEIGHT; row++) {
		cur_pos.X = pos->X;
		x_bm = 0; // x position within glyph bitmap, can span bytes 
		do {
			bitmap_byte = *glyph_data;
			bitmap_byte = Bit_Reverse_Byte(bitmap_byte);
			num_pixels = MIN(8,glyph_width - x_bm);
			if ((cur_pos.X & 0x07) == 0) { // Glyph is byte-aligned, so ready to plot
				LCD_Plot_Packed_Pixels(bitmap_byte, (&cur_pos));
				// LCD_Refresh();
				x_bm += num_pixels;
			} else { // Split and align as needed
				LCD_Plot_Packed_Pixels_Unaligned(bitmap_byte, (cur_pos.X & 0x07), 
					(&cur_pos));
				// LCD_Refresh();
 				x_bm += num_pixels;
			}
			cur_pos.X += num_pixels;
			glyph_data++; 	// Advance to next byte of glyph data
		} while (x_bm < glyph_width);
		while (x_bm < CHAR_WIDTH) {
			// fill in rest of cell with background color for narrow glyphs
			LCD_Plot_Pixel(&cur_pos, &bg);
			// LCD_Refresh();
			x_bm++;
			cur_pos.X++;
		}
		cur_pos.Y++;
	}
	LCD_Refresh();
#else	// BPP != 1, so expand to given color
	end_pos.X = pos->X+CHAR_WIDTH-1;
	end_pos.Y = pos->Y+CHAR_HEIGHT-1;

	LCD_Start_Rectangle(pos, &end_pos); 

	for (row = 0; row < CHAR_HEIGHT; row++) {
		x_bm = 0; // x position within glyph bitmap, can span bytes 
		do {
			bitmap_byte = *glyph_data;
			
			// Special cases with run starting at LSB	
			// Up to 8 bit run
			if (bitmap_byte == 0x00) {
				num_pixels = MIN(8,glyph_width - x_bm);
				LCD_Write_Rectangle_Pixel(&bg, num_pixels);
				x_bm += num_pixels;	
			} else if (bitmap_byte == 0xff) {
				num_pixels = MIN(8,glyph_width - x_bm);
				LCD_Write_Rectangle_Pixel(&fg, num_pixels);
				x_bm += num_pixels;	
			} else {
				col = 0;
				num_pixels = 0;
				if ((bitmap_byte & 0x7f) == 0) {		// Up to 7 bit run
					num_pixels = MIN(7,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&bg, num_pixels);
				} else if ((bitmap_byte & 0x7f) == 0x7f) {
					num_pixels = MIN(7,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&fg, num_pixels);
				} else if ((bitmap_byte & 0x3f) == 0) { // Up to 6 bit run
					num_pixels = MIN(6,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&bg, num_pixels);
				} else if ((bitmap_byte & 0x3f) == 0x3f) {
					num_pixels = MIN(6,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&fg, num_pixels);
				} else if ((bitmap_byte & 0x1f) == 0) { // Up to 5 bit run
					num_pixels = MIN(5,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&bg, num_pixels);
				} else if ((bitmap_byte & 0x1f) == 0x1f) {
					num_pixels = MIN(5,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&fg, num_pixels);
				} else if ((bitmap_byte & 0x0f) == 0) {	// Up to 4 bit run
					num_pixels = MIN(4,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&bg, num_pixels);
				} else if ((bitmap_byte & 0x0f) == 0x0f) {
					num_pixels = MIN(4,glyph_width - x_bm);
					LCD_Write_Rectangle_Pixel(&fg, num_pixels);
				}
				if (num_pixels > 0) {
					x_bm += num_pixels;	// Advance position in glyph bitmap
					col += num_pixels;  // Advance position withing glyph bitmap byte
					bitmap_byte >>= num_pixels;
				}
				for (; (x_bm < glyph_width) && (col < 8); col++) { // Remaining pixels in byte
					if (bitmap_byte & 0x01) // if pixel is to be set
						pixel_color = &fg;
					else
						pixel_color = &bg;
					LCD_Write_Rectangle_Pixel(pixel_color, 1);
					bitmap_byte >>= 1;
					x_bm++;
				}
			}
			glyph_data++; 	// Advance to next byte of glyph data
		} while (x_bm < glyph_width);
		if (x_bm < CHAR_WIDTH) {
			// fill in rest of cell with background color for narrow glyphs
			LCD_Write_Rectangle_Pixel(&bg, CHAR_WIDTH - x_bm);	
		}
	}
#endif // BPP != 1
}

void LCD_Text_PrintStr(PT_T * pos, char * str) {
	while (*str) {
		LCD_Text_PrintChar(pos, *str);
#if FORCE_MONOSPACE
		pos->X += CHAR_WIDTH; // forces monospacing for fonts
#else
		if (*str == ' ')
			pos->X += CHAR_WIDTH; // Increase width for space character!
		else
			pos->X += LCD_Text_GetGlyphWidth(*str)+1; // add a pixel of padding 
#endif
		str++;
	}
}

void LCD_Text_PrintStr_RC( uint8_t  row, uint8_t  col, char  *str )
{
	PT_T pos;
	pos.X = COL_TO_X( col );
	pos.Y = ROW_TO_Y( row );
	while( *str )
	{
		LCD_Text_PrintChar( &pos, *str );
#if FORCE_MONOSPACE
		pos.X += CHAR_WIDTH; // forces monospacing for fonts
#else
		if (*str == ' ')
			pos.X += CHAR_WIDTH; // Increase width for space character!
		else
			pos.X += LCD_Text_GetGlyphWidth(*str)+1; // add a pixel of padding 
#endif		
		str++;
	}
}

 void LCD_Text_Test(void) {
	PT_T pos;
	uint32_t c;
	int i;
	 
 	pos.X = 0;
	pos.Y = 0;
		
	for (i=0;i<100;i++) {
		LCD_Text_PrintStr(&pos, "Testing");
		LCD_Refresh();
		Delay(10);
		LCD_Erase();
		pos.X = pos.Y = i++;
	}
 
	LCD_Text_PrintStr_RC(0, 0, "1+ACDVZ_af");

 	pos.X = 0;
	pos.Y = 80;

	 
	for (c= ' '; c<='~'; c++) {
			LCD_Text_PrintChar(&pos, c);
			pos.X += CHAR_WIDTH+2;
			if (pos.X >= LCD_WIDTH - CHAR_WIDTH) {
				pos.X = 0;
				pos.Y += CHAR_HEIGHT+2;
				if (pos.Y >= LCD_HEIGHT - CHAR_HEIGHT) {
					pos.Y = 0;
				}
			}
		}
	
}
	 


