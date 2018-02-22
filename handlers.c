
#include "app.h"

void NMI_Handler(void)
{
 panic(NMI_OOPS);
}

void HardFault_Handler(void)
{
 panic(HARDFAULT_OOPS);
}

void MemManage_Handler(void)
{
 panic(MEMMANAGE_OOPS);
}

void BusFault_Handler(void)
{
 panic(BUSFAULT_OOPS);
}

void UsageFault_Handler(void)
{
 panic(USAGEFAULT_OOPS);
}

void SVC_Handler(void)
{
 panic(SVC_OOPS);
}

void DebugMon_Handler(void)
{
 panic(DEBUGMONITOR_OOPS);
}

void PendSV_Handler(void)
{
 panic(PEND_SVC_OOPS);
}

static ushort ms_cnt = 100;

// happens each 1ms
void SysTick_Handler(void)
{
 HAL_IncTick();
 // HAL_SYSTICK_IRQHandler();
 if (!(ms_cnt++ % 100))
   _100_ms_tick();   
 if (ms_cnt == 1000) {
   ms_cnt = 0;
   _1_sec_tick();
 }
}

extern TIM_HandleTypeDef buzz_timer;

void TIM2_IRQHandler(void)
{
 HAL_TIM_IRQHandler(&buzz_timer);
}

void EXTI0_IRQHandler(void)
{
 HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
 HAL_GPIO_EXTI_IRQHandler(FAN_GPIO(0));
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
 pin--;
 if ((pin < MAX_RPM_SENSORS) && HAL_GPIO_ReadPin(GPIOC, pin + 1))
   status.fan[pin]++;
}

//void PPP_IRQHandler(void)
