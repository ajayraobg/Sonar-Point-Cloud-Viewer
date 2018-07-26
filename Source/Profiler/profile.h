#ifndef PROFILE_H
#define PROFILE_H

#include "system_MKL25Z4.h"

//// Configuration 

#define PROFILER_SAMPLE_FREQ_HZ (1000)

// Comment out PROFILER_LCD_SUPPORT definition if not using LCD
// #define PROFILER_LCD_SUPPORT 

// Comment out USING_RTOS definition if not using RTOS
// #define USING_RTOS

// Comment out PROFILER_SERIAL_SUPPORT if not using printf/serial port for profile output
#define PROFILER_SERIAL_SUPPORT

//// End of configuration


/* Exception Behavior and Stacks:
RTOS: 
	HW pushes context onto PSP (PC offset is +24 bytes)
	SW (handler) builds stack frame on MSP 
	SW: Call to Process_Profile_Sample builds stack frame on MSP 
	=> Return address is is at PSP + 24
	
No RTOS:
	HW pushes context onto MSP (PC offset is 24 bytes)
	SW: IRQ handler builds stack frame on MSP (+8 bytes)
	SW: Call to Process_Profile_Sample builds stack frame on MSP (+12 bytes)
	=> Return address is is at MSP + 44
	
*/ 
#define HW_RET_ADX_OFFSET (24)
#define IRQ_FRAME_SIZE (8) 
#define PPS_FRAME_SIZE (12)

#ifdef USING_RTOS // Don't need these since PC is on PSP, not MSP
	#define FRAME_SIZE 	(0)
	#define CUR_SP 			(_psp)
#else // Using MSP, so stack frames are also on the MSP stack 
	#define FRAME_SIZE 	(IRQ_FRAME_SIZE + PPS_FRAME_SIZE)
	#define CUR_SP 			(_msp)
#endif	

#define SAMPLE_FREQ_HZ_TO_TICKS(freq) ((SystemCoreClock/(2*freq))-1)

extern unsigned char profiling_enabled;

extern void Init_Profiling(void);

extern void Disable_Profiling(void);
extern void Enable_Profiling(void);

// Need to call this function from timer IRQHandler (e.g. PIT_IRQHandler)
extern void Process_Profile_Sample(void);

extern void Sort_Profile_Regions(void);

#ifdef PROFILER_LCD_SUPPORT 
extern void Display_Profile(void);
#endif

#ifdef PROFILER_SERIAL_SUPPORT
void Serial_Print_Sorted_Profile(void);
#endif

#endif
