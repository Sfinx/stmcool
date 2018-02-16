
#pragma once

#include <stm32f10x.h>
#include <stm32f10x_exti.h>
#include <string.h>
#include <stdio.h>
#include <rtt.h>

// #define __PUT_IN_RAM__ __attribute__((section(".data")))

typedef unsigned int uint;

extern void board_init();
extern void panic(int);
extern void beep(u32 t);
extern void hard_delay_mks(u32 mks);

#define hard_delay_ms(x)	hard_delay_mks(x * 1000)

#define BUZZER_FREQ		0

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
    SYSTICK_OOPS	// 12
} oops_t;
