#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#define MASK(x) (1UL << (x))

// Debug Signals on port B
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 8		
#define DBG_5 9		
#define DBG_6 10  
#define DBG_7 11

// Debug Signal mapping 
#define DBG_TREADACC_POS		DBG_1 	
#define DBG_TREADTS_POS 		DBG_2
#define DBG_TREFILLSB_POS 	DBG_3
#define DBG_TUPDATESCR_POS	DBG_4
#define DBG_IRQDMA_POS 			DBG_5
#define DBG_TSNDMGR_POS 		DBG_6
#define DBG_TIDLE						DBG_7

void Init_Debug_Signals(void);

#endif // DEBUG_H
