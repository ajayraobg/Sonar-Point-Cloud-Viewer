/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"

#include "LCD.h"
#include "font.h"

#include "LEDs.h"
#include "timers.h"
#include "UART.h"

#include "I2C.h"
#include "mma8451.h"
#include "delay.h"
#include "profile.h"
#include "math.h"

#define NUM_TESTS 4000
uint8_t samples[1000] = {0};
uint8_t res;

void delay_us(unsigned a) { // Probably not tuned for KL25Z! 
 	volatile int i;
	for (i=0; i<(3*a); i++) {
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

void init_ADC(void) {
	
	SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT); 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//PORTE-> PCR[20] &= ~PORT_PCR_MUX_MASK;
	//PORTE-> PCR[20] |= PORT_PCR_MUX(0);
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADICLK(0) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(0);
	ADC0->SC2 = ADC_SC2_REFSEL(0);
}
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint16_t r;
	int start_flag = 1;
	int cnt = 0;
	int d[50] = {0};
	int t_off = 5;
	int t_spike = 20;
	char buffer[32];
	PT_T p;
	uint8_t c;
	uint8_t arr[5] = {1,2,3,4,5};
	Init_RGB_LEDs();
	init_ADC();
	Init_UART0(115200);
	__enable_irq();
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[1] &= ~PORT_PCR_MUX_MASK;          
	PORTC->PCR[1] |= PORT_PCR_MUX(1);
  PTC->PDDR |= MASK(1) ;	 
	PTC->PCOR = MASK(1) ;
	int Set_flag = 0;
	int count = 1000;
	Control_RGB_LEDs(1,0,0);
	Delay(5);
	int sum1,sum2;
	int trans_flag = 1;
					int l = 254;
	int m = 250;	
	int n = 123;
	while(1){
		if(Set_flag == 1)
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
				PORTC->PCR[1] &= ~PORT_PCR_MUX_MASK;          
				PORTC->PCR[1] |= PORT_PCR_MUX(1);

				Set_flag = 0;
			}
		c = UART0_Receive_Poll();
		Control_RGB_LEDs(0,1,0);

		Delay(5);
		if(c == 'a'){

			Control_RGB_LEDs(0,0,1);	
			PTC->PSOR = MASK(1);
				delay_us(10);
				PTC->PCOR = MASK(1);
				Set_flag = 1;
				for(int i=0;i<count;i++)
				{
					ADC0->SC1[0] = 0x00; // start conversion on channel	
					while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
					;
					samples[i] = ADC0->R[0];
					delay_us(25);

				}

				for(int i =0;i<1000;i=i+5){
					sum1 = samples[i+1] + samples[i+2] + samples[i+3] + samples[i+4];

				printf("%d,",samples[i]);
				printf("%d,",sum1);

			}
			
	

		}

	}

}

