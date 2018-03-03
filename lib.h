
#pragma once

#include "stm32l4xx_hal.h"
#include "rtt.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint16_t ushort;

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
    SYSTEMCLOCK_OOPS,	// 13
    ADC_INIT_OOPS,	// 14
    TEMP_SENSOR_OOPS	// 15
} oops_t;

#define BUZZER_FREQ	7

extern void board_init();
extern void set_led(uchar led, uchar on);
extern void beep(u32 t);
extern short get_mcu_temp();

#define PANIC_DELAY	100
