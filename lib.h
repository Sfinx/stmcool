
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
extern void delay_mks(u32 mks);
extern void _1_sec_tick();
extern void _100_ms_tick();

#define delay_ms(x)     delay_mks(x * 1000)
