
#include <app.h>

status_t status;

void systick_init()
{
 status._1ms_tick = 1000; // do the first tick as soon as possible
 // 1 ms general tick
 if (SysTick_Config(SystemCoreClock / 1000)) // 1 ms (1000 Hz)
  panic(SYSTICK_OOPS);
}

void beep(u32 t)
{
 if (!status.buzzer_on) {
   status.buzzer_timer = (t * 10);
   status.buzzer_on = 1;
 }
}

void set_buzzer(u32 on)
{
 if (on)
   GPIO_SetBits(GPIOC, BUZZER_PIN);
 else
   GPIO_ResetBits(GPIOC, BUZZER_PIN);
}

void buzzer(u32 on)
{
 status.buzzer_on = on;
 set_buzzer(on);
}

void toggle_buzzer()
{
 if (GPIO_ReadOutputDataBit(GPIOC, BUZZER_PIN))
   set_buzzer(0);
 else
   set_buzzer(1);
}

void buzzer_init()
{
 NVIC_InitTypeDef NVIC_InitStructure;
 TIM_TimeBaseInitTypeDef TIM_InitStructure; 
 GPIO_InitTypeDef gpio;

 GPIO_StructInit(&gpio);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 gpio.GPIO_Pin = BUZZER_PIN;
 gpio.GPIO_Speed = GPIO_Speed_2MHz;
 gpio.GPIO_Mode = GPIO_Mode_Out_PP; 
 GPIO_Init(GPIOC, &gpio);

 buzzer(0);

 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // buzzer priority
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);

 TIM_TimeBaseStructInit(&TIM_InitStructure);
 TIM_InitStructure.TIM_Prescaler = 1000;
 TIM_InitStructure.TIM_Period = 4;
 TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
 TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
 TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
 TIM_Cmd(TIM2, ENABLE);

 TIM_ClearFlag(TIM2, TIM_FLAG_Update);
 TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
 TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

// C0 - C4 fans
void fan_sensors_init()
{
 uint i;
 GPIO_InitTypeDef gpio;

 GPIO_StructInit(&gpio);

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

 gpio.GPIO_Pin = (FAN0 | FAN1 | FAN2 | FAN3 |FAN4);
 gpio.GPIO_Mode = GPIO_Mode_IPU;
 GPIO_Init(GPIOC, &gpio);

 EXTI_InitTypeDef EXTI_InitStruct;
 NVIC_InitTypeDef NVIC_InitStruct;
 EXTI_StructInit(&EXTI_InitStruct);
 // enable clock for alternate function (EXTI)
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 // Add IRQ vector to NVIC
 // PBx is connected to EXTI_Linex, which has EXTIx_IRQn vector
 for (i = 0; i < MAX_FANS; i++) {
   NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn + i;
   // Set priorities
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0xF;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0xF;
   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
   // Add to NVIC
   NVIC_Init(&NVIC_InitStruct);
   // Tell system that you will use PCx for EXTI_Linex
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0 + i);
 }

 EXTI_InitStruct.EXTI_Line = (EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
 // Enable interrupt
 EXTI_InitStruct.EXTI_LineCmd = ENABLE;
 // Interrupt mode
 EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
 // Triggers on rising and falling edge
 EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
 // Add to EXTI
 EXTI_Init(&EXTI_InitStruct);
}

void leds_init()
{
 GPIO_InitTypeDef gpio;
 GPIO_StructInit(&gpio);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 gpio.GPIO_Pin = (GREEN_LED | BLUE_LED);
 gpio.GPIO_Speed = GPIO_Speed_2MHz;
 gpio.GPIO_Mode = GPIO_Mode_Out_PP; 
 GPIO_Init(GPIOC, &gpio);
}

// PB5 - PB10
void temp_sensors_init()
{
 GPIO_InitTypeDef gpio;
 GPIO_StructInit(&gpio);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 gpio.GPIO_Pin = (TEMP0);
 gpio.GPIO_Speed = GPIO_Speed_10MHz;
 gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOB, &gpio); 

 EXTI_InitTypeDef EXTI_InitStruct;
 NVIC_InitTypeDef NVIC_InitStruct;
 EXTI_StructInit(&EXTI_InitStruct);
 // enable clock for alternate function (EXTI)
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 // Add IRQ vector to NVIC
 // PBx is connected to EXTI_Linex, which has EXTIx_IRQn vector
 NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
 // Set priorities
 NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0xF;
 NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0xF;
 NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
 // Add to NVIC
 NVIC_Init(&NVIC_InitStruct);
 // Tell system that you will use PCx for EXTI_Linex
 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

 EXTI_InitStruct.EXTI_Line = EXTI_Line5;
 // Enable interrupt
 EXTI_InitStruct.EXTI_LineCmd = ENABLE;
 // Interrupt mode
 EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
 // Triggers on rising and falling edge
 EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
 // Add to EXTI
 EXTI_Init(&EXTI_InitStruct);
}

void board_init()
{
 memset(&status, 0, sizeof(status));
 systick_init();
 leds_init();
 buzzer_init();
 fan_sensors_init();
 temp_sensors_init();
 SEGGER_RTT_printf(0, "%sPowered On\n\r%s", RTT_CTRL_TEXT_GREEN, RTT_CTRL_RESET);
}

void set_led(uint led, int on)
{
 if (on)
   GPIO_SetBits(GPIOC, led);
 else
   GPIO_ResetBits(GPIOC, led);
}

void led_toggle(uint led)
{
 if (GPIO_ReadOutputDataBit(GPIOC, led))
   set_led(led, 0);
 else
   set_led(led, 1);
}

void set_leds(int v)
{
 set_led(GREEN_LED, v);
 set_led(BLUE_LED, v);
}

void hard_elay(vu32 nCount)
{
 for(; nCount != 0; nCount--);
}

void panic(int i)
{
 while(1) {
  SEGGER_RTT_printf(0, "%sOops [%d] !\r\n%s", RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
  set_leds(1);
  hard_elay(0x9FFFFF);
  set_leds(0);
  hard_elay(0x9FFFFF);
 }
}

void hard_delay_mks(u32 mks)
{
 volatile u32 nCount;
 RCC_ClocksTypeDef RCC_Clocks;
 RCC_GetClocksFreq (&RCC_Clocks);
 nCount = (RCC_Clocks.HCLK_Frequency / 10000000) * mks;
 for (; nCount !=0 ; nCount--);
}
