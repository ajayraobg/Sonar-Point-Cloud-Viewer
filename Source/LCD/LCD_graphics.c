#include "LCD.h"
#include "LCD_driver.h"

#include "ST7789.h"
#include "T6963.h"

#define DRAW_RUNS_AS_LINES (0)
#define STEP 8

void Graphics_Test(void) {
	int x, y, r;
	PT_T p1, p2;
	COLOR_T c;
	volatile int n;
	
	p1.X = LCD_WIDTH/2;
	p1.Y = LCD_HEIGHT/2;

	c.R = 100;
	c.G = 255;
	c.B = 255;

	LCD_Fill_Buffer(&c);
	LCD_Refresh();

	c.G = 0;
	LCD_Fill_Buffer(&c);
	LCD_Refresh();

	c.G = 255;
	p1.X = p1.Y = 0;
	for (x = 0; x<LCD_WIDTH; x++) {
		p1.X++;
		if (x&1)
			p1.Y++;
		LCD_Plot_Pixel(&p1, &c);
		LCD_Refresh();
	}
	
	// vertical lines
	p1.Y = 0;
	p2.Y = LCD_HEIGHT-1;
	for (x=0; x<LCD_WIDTH; x += 1) {
		p1.X = p2.X = x;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}
	
	p1.X = LCD_WIDTH/2;
	p1.Y = LCD_HEIGHT/2;
	c.G = 0xff;

	// Do circles	
	// Time filled circles
	for (r = 100; r>8; r -= 1) {
		LCD_Draw_Circle(&p1, r, &c, 1);
		LCD_Refresh();
		c.B -= 8;
		c.G ^= 0xff;
		c.R -= 16;
	}

	c.G = 100;
	for (x=0; x<LCD_WIDTH; x += STEP) {
		p1.X = x+20;
		LCD_Draw_Circle(&p1, 5 + x/10, &c, 1);
		LCD_Refresh();
	}
	
	// Time drawing lines radiating from center
	p1.X = LCD_WIDTH/2;
	p1.Y = LCD_HEIGHT/2;
	
	p2.Y = 0;
	c.G = 100;
	for (x=0; x<LCD_WIDTH; x += STEP) {
		p2.X = x;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}
	
	c.G = 100;
	c.B = 100;
	c.R = 255;
	p2.X = LCD_WIDTH-1;
	for (y=0; y<LCD_HEIGHT; y += STEP) {
		p2.Y = y;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}

	c.R = 100;
	c.B = 100;
	c.G = 255;
	p2.Y = LCD_HEIGHT-1;
	for (x=0; x<LCD_WIDTH; x += STEP) {
		p2.X = x;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}

	c.R = 200;
	c.B = 100;
	c.G = 255;
	p2.X = 0;
	for (y=0; y<LCD_HEIGHT; y += STEP) {
		p2.Y = y;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}

#if 0 	
	// Dither test
	c.G = 0;
	LCD_Fill_Buffer(&c);
	p1.X = 0;
	p2.X = LCD_WIDTH-1;
	p1.Y = p2.Y = LCD_HEIGHT/2;
	n = 0;
	c.G = 0xff;
	LCD_Draw_Line(&p1, &p2, &c);
	LCD_Refresh();
	p1.Y+=2;
	p2.Y+=2;
	while (1) {
		if (n++ & 1)
			c.G = 0xff;
		else 
			c.G = 0;
		LCD_Draw_Line(&p1, &p2, &c);
		LCD_Refresh();
	}
#endif
	
}

void LCD_Draw_Line(PT_T * p1, PT_T * p2, COLOR_T * color)
// Scan line conversion code from Michael Abrash
{
	PT_T p;
#if DRAW_RUNS_AS_LINES
	PT_T pe;
#endif
	
  int Temp, AdjUp, AdjDown, ErrorTerm, XAdvance, XDelta, YDelta;	 
  int WholeStep, InitialPixelCount, FinalPixelCount, i,j, RunLength;
  int XStart;
  int YStart;
  int XEnd;
  int YEnd;

  XStart = p1->X;
  YStart = p1->Y;
  XEnd = p2->X;
  YEnd = p2->Y;

  /* We'll always draw top to bottom, to reduce the number of cases we have to
     ** handle, and to make lines between the same endpoints draw the same pixels
  */
  if (YStart > YEnd) {
		Temp = YStart;
		YStart = YEnd;
		YEnd = Temp;
		Temp = XStart;
		XStart = XEnd;
		XEnd = Temp;
	}

  /* Figure out whethere we're going left or right, and how far we're going
  ** horizontally  */
  XDelta = XEnd - XStart;
  if (XDelta < 0) {
      XAdvance = -1;
      XDelta = -XDelta;
	} else {
      XAdvance = 1;
  }

  /* Figure out how far we're going vertically */
  YDelta = YEnd - YStart;

  /* Special-case horizontal, vertical, and diagonal lines, for speed and
  ** to avoid nasty boundary conditions and division by 0
  */

	p.X = XStart;
	p.Y = YStart;
	 
  /* Vertical Line case */
  if (XDelta == 0) {
    /* Vertical line */
#if DRAW_RUNS_AS_LINES
		pe.X = XEnd;
		pe.Y = YEnd;
		LCD_Fill_Rectangle(&p, &pe, color);
#else		
    for (i = 0; i <= YDelta; i++) {
				p.Y++;
				LCD_Plot_Pixel(&p, color);
     }
#endif
    return;
  }
  
  /* Horizontal Line Case */
  if (YDelta == 0) {
    /* Horizontal line */
#if DRAW_RUNS_AS_LINES
		pe.X = XEnd;
		pe.Y = YEnd;
		LCD_Fill_Rectangle(&p, &pe, color);
#else
    for (i = 0; i <= XDelta; i++) {
				LCD_Plot_Pixel(&p, color);
				p.X += XAdvance;
    }  
#endif
    return;
  }
   
  /* Diagonal Case */
  if (XDelta == YDelta) {
    /* Diagonal line */
    for (i = 0; i <= XDelta; i++)
      {
				LCD_Plot_Pixel(&p, color);
				p.X += XAdvance;
				p.Y++;
      }
    return;
  }
   
   
  /* Determine whether the line is X or Y major, and handle accordingly */
  if (XDelta >= YDelta) {
    /* X major line */
    /* Minimum # of pixels in a run in this line */
    WholeStep = XDelta / YDelta;													/* DIV */
    
    /* Error term adjust eacn time Y steps by 1; used to tel when one
    ** extra pixel should be drawn as part of a run, to account for
    ** fractional steps along the X axis per 1-pixel steps along Y
    */
    AdjUp = (XDelta % YDelta) * 2;													/* DIV */
    
    /* Error term adjust when the error term turns over, used to factor
    ** out the X step made at that time
    */
    AdjDown = YDelta * 2;
    
    /* Initial error term; reflects an initial step of 0.5 along the Y axis 
     */
    ErrorTerm = (XDelta % YDelta) - (YDelta * 2);											/* DIV */
    
    /* The initial and last runs are partial, because Y advancse only 0.5
    ** for these runs, rather than 1.  Divide one full run, plus the
    ** initial pixel, between the initial and last runs.
    */
    InitialPixelCount = (WholeStep / 2) + 1;
    FinalPixelCount = InitialPixelCount;
    
    /* If the basic run length is even and there's no fractional
    ** advance, we have one pixel that could go to either the initial
    ** or last partial run, which we'll arbitrarily allocate to the
    ** last run
    */
    if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
      InitialPixelCount--;
    
    /* If there's an odd number of pixels per run, we have 1 pixel that can't
       ** be allocated to either the initial run or last partial run, so we'll add 0.5
       ** to error term so this pixel will be handled by the normal full-run loop
    */
    if ((WholeStep & 0x01) != 0)
      ErrorTerm += YDelta;
    
    /* Draw the first, partial run of pixels */
		#if DRAW_RUNS_AS_LINES // TODO: Fixing here
		p.X = XStart;
		p.Y = pe.Y = YStart;
		if (XAdvance > 0) {
			pe.X = XStart + InitialPixelCount;
			LCD_Fill_Rectangle(&p, &pe, color);
		} else {
			pe.X = XStart - InitialPixelCount;
			LCD_Fill_Rectangle(&pe, &p, color);
		}
		p.X = pe.X;
#else
    for (j = 0; j < InitialPixelCount; j++) {
			LCD_Plot_Pixel(&p, color);
			p.X += XAdvance;
		}
#endif
    p.Y++;
    
    /* Draw all full runs */
    for (i = 0; i < (YDelta - 1); i++) {
			RunLength = WholeStep; /* run is at least this long */
			/* Advance the error term and add an extra pixel if the error
			** term so indicates
			*/
			if ((ErrorTerm += AdjUp) > 0) {
				RunLength++;
				ErrorTerm -= AdjDown; /* reset the error term */
			}

			/* Draw this scan line's run */
#if DRAW_RUNS_AS_LINES
			pe.Y = p.Y;
			pe.X = p.X + RunLength;
			LCD_Fill_Rectangle(&p, &pe, color);
			p.X = pe.X;
#else
			for (j = 0; j < RunLength; j++) {
				LCD_Plot_Pixel(&p, color);
				p.X += XAdvance;
			}
#endif
			p.Y++;
		}
    
    /* Draw the final run of pixels */
#if DRAW_RUNS_AS_LINES
		pe.Y = p.Y;
		pe.X = pe.X + FinalPixelCount;
		LCD_Fill_Rectangle(&p, &pe, color);
		p.X = pe.X;
#else
    for (j = 0; j < FinalPixelCount; j++) {
			LCD_Plot_Pixel(&p, color);
			p.X += XAdvance;
		}
#endif
    p.Y++;
    
    return;
  } else {
    /* Y major line */
    
    /* Minimum # of pixels in a run in this line */
    WholeStep = YDelta / XDelta;										/* DIV */
    
    /* Error term adjust each time X steps by 1; used to tell when 1 extra
    ** pixel should be drawn as part of a run, to account for
    ** fractional steps along the Y axis per 1-pixel steps along X
    */
    AdjUp = (YDelta % XDelta) * 2;										/* DIV */
    
    /* Error term adjust when the error term turns over, used to factor
    ** out the Y step made at that time
    */
    AdjDown = XDelta * 2;
    
    /* Initial error term; reflects initial step of 0.5 along the X axis */
    ErrorTerm = (YDelta % XDelta) - (XDelta * 2);								/* DIV */
    
    /* The initial and last runs are partial, because X advances only 0.5
    ** for these runs, rather than 1.  Divide ony full run, plus the
    ** initial pixel, between the initial and last runs
    */
    
    InitialPixelCount = (WholeStep / 2) + 1;
    FinalPixelCount = InitialPixelCount;
    
    /* If the basic run length is even and there's no fractional
      ** advance, we have one pixel that could go to either the initial
      ** or last partial run, which we'll arbitrarily allocate to the
      ** last run
      */
      if ((AdjUp == 0) && ((WholeStep & 0x01) == 0)) {
				InitialPixelCount--;
			}
      
      /* If there's an odd number of pixels per run, we have 1 pixel that can't
	 ** be allocated to either the initial run or last partial run, so we'll add 0.5
	 ** to error term so this pixel will be handled by the normal full-run loop
      */
      if ((WholeStep & 0x01) != 0) {
				ErrorTerm += XDelta;
			}
      
      /* Draw the first, partial run of pixels */
#if DRAW_RUNS_AS_LINES
			p.X = pe.X = XStart;
			p.Y = YStart;
			pe.Y = YStart + InitialPixelCount;
			LCD_Fill_Rectangle(&p, &pe, color);
#else
      for (j = 0; j < InitialPixelCount; j++) {
				LCD_Plot_Pixel(&p, color);
				p.Y++;
			}
      /* Update x,y position */
      p.X += XAdvance;
#endif
      
      /* Draw all full runs */
      for (i = 0; i < (XDelta - 1); i++) {
				RunLength = WholeStep; /* run is at least this long */
	  
				/* Advance the error term and add an extra pixel if the error
				** term so indicates
				*/
				if ((ErrorTerm += AdjUp) > 0)	{
					RunLength++;
					ErrorTerm -= AdjDown; /* reset the error term */
				}
				
				/* Draw this scan line's run */
#if DRAW_RUNS_AS_LINES
				p.X = pe.X = XStart;
				p.Y = YStart;
				pe.Y = YStart + RunLength;
				LCD_Fill_Rectangle(&p, &pe, color);
#else
				for (j = 0; j < RunLength; j++) {
					LCD_Plot_Pixel(&p, color);
					p.Y++;
				}
				/* Update x,y position */
				p.X += XAdvance;
#endif
			}
      
      /* Draw the final run of pixels */
#if DRAW_RUNS_AS_LINES
			p.X = pe.X= XStart;
			p.Y = YStart;
			pe.Y = YStart + FinalPixelCount;
			LCD_Fill_Rectangle(&p, &pe, color);
#else
      for (j= 0; j < FinalPixelCount; j++) {
				LCD_Plot_Pixel(&p, color);
				p.Y++;
			}
      /* Update x,y position */
      p.X += XAdvance;
#endif    
      return;
	}
}

/* Draw a circle at coordinates xm, ym with radius r and specified color c. */
void LCD_Draw_Circle(PT_T * pc, int radius, COLOR_T * c, int filled) {
	PT_T p1, p2;
  int x = -radius, y = 0, err = 2-2*radius; /* II. Quadrant */ 
  if (filled>0) { 
		do {
      //setPixel(xm+x, ym-y); /* III. Quadrant */
		 	p1.X=pc->X+x;
			p1.Y=pc->Y-y;
			p2.X=pc->X-x;
			p2.Y=pc->Y-y;
			LCD_Draw_Line(&p1, &p2, c);
			p1.Y=pc->Y+y;
			p2.Y=pc->Y+y;
			LCD_Draw_Line(&p1, &p2, c);

			radius = err;
      if (radius <= y) 
				err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (radius > x || err > y) 
				err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
		} while (x < 0); 
 } else {
		do {
      //setPixel(xm-x, ym+y); /*   I. Quadrant */
			p1.X=pc->X-x;
			p1.Y=pc->Y+y;
			LCD_Plot_Pixel(&p1,c);
      //setPixel(xm-y, ym-x); /*  II. Quadrant */
		 	p1.X=pc->X-y;
			p1.Y=pc->Y-x;
			LCD_Plot_Pixel(&p1,c);
      //setPixel(xm+x, ym-y); /* III. Quadrant */
		 	p1.X=pc->X+x;
			p1.Y=pc->Y-y;
			LCD_Plot_Pixel(&p1,c);
      //setPixel(xm+y, ym+x); /*  IV. Quadrant */
		 	p1.X=pc->X+y;
			p1.Y=pc->Y+x;
			LCD_Plot_Pixel(&p1,c);
      radius = err;
      if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (radius > x || err > y) 
				err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
		} while (x < 0);
	}
}


