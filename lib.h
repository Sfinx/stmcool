
#pragma once

#include "stm32l4xx_hal.h"
#include "rtt.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef uint8_t u8;
typedef uint16_t u16;
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
    TEMP_SENSOR_OOPS,	// 15
    FAN_OOPS,		// 16
    USB_OOPS
} oops_t;

#define BUZZER_FREQ	7

extern void board_init();
extern void set_led(uchar led, uchar on);
extern void beep(u32 t);
extern short get_mcu_temp();

#define PANIC_DELAY	100

#include "usbd_conf.h"

extern uint8_t usb_cdc_send_str(const char *s);
extern uint8_t usb_cdc_printf(const char *fmt, ...);
extern uint8_t usb_cdc_send(const uint8_t* buf, uint16_t len);
extern void usb_cdc_send_rx_cb(uint8_t* buf, uint32_t len);
extern void user_btn_cb(uchar pressed);
extern void blink(uchar led);
extern const char *mcu_time(void);
extern uint64_t get_serial(void);
