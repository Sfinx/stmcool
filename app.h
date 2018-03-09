
#pragma once

#include <lib.h>

#define LED_DELAY  		500

// Red LED - PA1
#define RED_LED_PIN         	GPIO_PIN_1
// Green LED - PH0
#define GREEN_LED_PIN         	GPIO_PIN_0
// Blue LED - PH1
#define BLUE_LED_PIN         	GPIO_PIN_1

typedef enum leds_t {
    RED_LED,
    GREEN_LED,
    BLUE_LED
} leds_t;

// buzzer at PB4 (SO)
#define BUZZER_PIN      	GPIO_PIN_4
#define BUZZER_GPIO_BUS		GPIOB
#define BUZZER_GPIO_CLK_ENABLE	__GPIOB_CLK_ENABLE

// RPMS's at buttefly D0-D7
#define MAX_RPM_SENSORS		8

#define FAN0_GPIO		(GPIO_PIN_11)
#define FAN1_GPIO		(GPIO_PIN_10)
#define FAN2_GPIO		(GPIO_PIN_12)
#define FAN3_GPIO		(GPIO_PIN_13)
#define FAN4_GPIO		(GPIO_PIN_14)
#define FAN5_GPIO		(GPIO_PIN_15)
#define FAN6_GPIO		(GPIO_PIN_8)
#define FAN7_GPIO		(GPIO_PIN_9)

#define FAN_GPIO_BUS		GPIOB
#define FAN_GPIO_CLK_ENABLE	__GPIOB_CLK_ENABLE

extern u32 fan2pin(uchar x);
extern u8 pin2fan(u32 pin);

// TEMP's at C9-C15
#define MAX_TEMP_SENSORS 	8

typedef struct status_t {
  volatile u32 _1ms_tick;
  volatile unsigned long long seconds;
  volatile u32 buzzer_timer;
  volatile unsigned panic:1;
  volatile unsigned buzzer_on:1;
  volatile unsigned user_btn:1;
  volatile unsigned short fan[MAX_RPM_SENSORS];
  volatile unsigned short temp[MAX_TEMP_SENSORS];
  short mcu_temp;
} status_t;

extern status_t status;

extern void _1_sec_tick();
extern void _100_ms_tick();

#define BEEP_DELAY	300
