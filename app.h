
#pragma once

#include <lib.h>

#pragma once

#define GREEN_LED   	GPIO_Pin_9
#define BLUE_LED    	GPIO_Pin_8
#define BUZZER_PIN  	GPIO_Pin_7

#define	FAN0		GPIO_Pin_0
#define	FAN1		GPIO_Pin_1
#define	FAN2		GPIO_Pin_2
#define	FAN3		GPIO_Pin_3
#define	FAN4		GPIO_Pin_4

// pump + flow meter + 4 radiator fans
#define MAX_FANS	5

#define	TEMP0		GPIO_Pin_5
#define	TEMP1		GPIO_Pin_6
#define	TEMP2		GPIO_Pin_7
#define	TEMP3		GPIO_Pin_8
#define	TEMP4		GPIO_Pin_9
#define	TEMP5		GPIO_Pin_10

#define MAX_TEMPS	6

extern void set_led(uint, int);

typedef struct status_t {
  volatile u32 _1ms_tick;
  volatile unsigned long long seconds;
  volatile u32 buzzer_timer;
  volatile unsigned buzzer_on:1;
  volatile unsigned sound:1;
  volatile unsigned short fan[MAX_FANS];
  volatile unsigned short temp[MAX_TEMPS];
} status_t;

extern status_t status;
