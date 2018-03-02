
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

// buzzer at PA0
#define BUZZER_PIN      	GPIO_PIN_0

// RPMS's at C0-C8
#define MAX_RPM_SENSORS		8

#define FAN_GPIO(x)		(GPIO_PIN_0 << x)

// TEMP's at C9-C15
#define MAX_TEMP_SENSORS 	8

typedef struct status_t {
  volatile u32 _1ms_tick;
  volatile unsigned long long seconds;
  volatile u32 buzzer_timer;
  volatile unsigned buzzer_on:1;
  volatile unsigned short fan[MAX_RPM_SENSORS];
  volatile unsigned short temp[MAX_TEMP_SENSORS];
} status_t;

extern status_t status;

extern void _1_sec_tick();
extern void _100_ms_tick();
