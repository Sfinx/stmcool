
#pragma once

#include "stm32l4xx_hal.h"
#include "rtt.h"

extern void panic(int);

typedef enum oops_t {
    NMI_OOPS, 		// 0
    HARDFAULT_OOPS, 	// 1
    MEMMANAGE_OOPS, 	// 2
    BUSFAULT_OOPS, 	// 3
    USAGEFAULT_OOPS, 	// 4
    DEBUGMONITOR_OOPS,	// 5
    SVC_OOPS,		// 6
    PEND_SVC_OOPS,	// 7
    WWDG_OOPS,		// 8
    PVD_OOPS,		// 9
    FLASH_OOPS,		// 10
    RCC_OOPS,		// 11
    SYSTICK_OOPS,	// 12
    SYSTEMCLOCK_OOPS	// 13
} oops_t;

extern void board_init();
extern void set_leds(uint on);
