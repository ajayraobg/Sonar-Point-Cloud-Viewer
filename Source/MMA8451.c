#include <MKL25Z4.H>
#include <math.h>
#include <stdio.h>

#include "MMA8451.h"
#include "I2C.h"
#include "delay.h"
#include "LEDs.h"

#define VALIDATE 0
#define SHOW_DATA 0
#define MAX_ERROR 2

#define M_PI_2 (M_PI/2.0f)
#define M_PI_4 (M_PI/4.0f)

int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

/*
 Initializes mma8451 sensor. I2C has to already be enabled.
 */
int init_mma()
{
	  //check for device
		if(i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI)	{	
		  Delay(100);
		  //turn on data ready irq; defaults to int2 (PTA15)
		  i2c_write_byte(MMA_ADDR, REG_CTRL4, 0x01);
		  Delay(100);
		  //set active, 14bit mode, low noise and 100Hz (0x1D)
		  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x1F);
				
		  //enable the irq in the NVIC
		  //NVIC_EnableIRQ(PORTA_IRQn);
		  return 1;
		}
		
		//else error
		return 0;
}

/* 
  Reads full 16-bit X, Y, Z accelerations.
*/
void read_full_xyz()
{
	
	int i;
	uint8_t data[6];
	
	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);
	
	for( i=0;i<6;i++)	{
		if(i==5)
			data[i] = i2c_repeated_read(1);
		else
			data[i] = i2c_repeated_read(0);
	}
	
	acc_X = (((int16_t) data[0])<<8) | data[1];
	acc_Y = (((int16_t) data[2])<<8) | data[3];
	acc_Z = (((int16_t) data[4])<<8) | data[5];
	printf("%d %d %d \n",acc_X,acc_Y,acc_Z);
}

void read_xyz_msb(void){

	int i;
	uint8_t data[3];
	
	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);
	
	for( i=0;i<3;i++)	{
		if(i==2)
			data[i] = i2c_repeated_read(1);
		else
			data[i] = i2c_repeated_read(0);
	}
	
	acc_X = ((int16_t) ((int8_t)data[0])<<8) ;
	acc_Y = ((int16_t) ((int8_t)data[1])<<8) ;
	acc_Z = ((int16_t) ((int8_t)data[2])<<8) ;
	printf("%d %d %d \n",acc_X,acc_Y,acc_Z);
}


void read_xyz(void)
{
	// sign extend byte to 16 bits - need to cast to signed since function
	// returns uint8_t which is unsigned
	acc_X = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_XHI))) << 8;
	Delay(1);
	acc_Y = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_YHI))) << 8;
	Delay(1);
	acc_Z = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_ZHI))) << 8;
	printf("%d %d %d \n",acc_X,acc_Y,acc_Z);
}

float approx_sqrt(float x)
 {
	 // http://bits.stephan-brumme.com/squareRoot.html
   unsigned int i = *(unsigned int*) &x; 
   // adjust bias
   i  += 127 << 23;
   // approximation of square root
   i >>= 1; 
   return *(float*) &i;
 }   

 float approx_atan2f(float y, float x)
{
	//http://pubs.opengroup.org/onlinepubs/009695399/functions/atan2.html
	//Volkan SALMA

	const float ONEQTR_PI = M_PI / 4.0;
	const float THRQTR_PI = 3.0 * M_PI / 4.0;
	//const float THRQTR_PI = 3.0 * ONEQTR_PI;
	float r = 0.0, angle = 0.0;
	float abs_y = fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	if ( x < 0.0f )
	{
		r = (x + abs_y) / (abs_y - x);
		angle = THRQTR_PI;
	}
	else
	{
		r = (x - abs_y) / (x + abs_y);
		angle = ONEQTR_PI;
	}
	angle += (0.1963f * r * r - 0.9817f) * r;
	if ( y < 0.0f )
		return( -angle );     // negate if in quad III or IV
	else
		return( angle );
}

void convert_xyz_to_roll_pitch(void) {
	float ax = acc_X/COUNTS_PER_G,
				ay = acc_Y/COUNTS_PER_G,
				az = acc_Z/COUNTS_PER_G;
#if VALIDATE
	float roll_ref, pitch_ref;
#endif
	
	roll = approx_atan2f(ay, az)*(180/M_PI);
	pitch = approx_atan2f(ax, approx_sqrt(ay*ay + az*az))*(180/M_PI);

#if VALIDATE
	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) { 
		roll_ref = atan2f(ay, az)*(180/M_PI);
		pitch_ref = atan2f(ax, sqrt(ay*ay + az*az))*(180/M_PI);
		if (fabs(roll-roll_ref) > MAX_ERROR) {
			printf("Roll Error: %f, should be %f.\n\r ay=%f, az=%f\n\r", roll, roll_ref, ay, az);
		}
		if (fabs(pitch-pitch_ref) > MAX_ERROR) {
			printf("Pitch Error: %f, should be %f.\n\r ax=%f, ay=%f, az=%f\n\r", pitch, pitch_ref, ax, ay, az);
		}
	}
#endif
	
#if SHOW_DATA
	printf("Roll: %f \tPitch: %f\n\r", roll, pitch);
#endif
}
