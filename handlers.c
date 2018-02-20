
#include "app.h"
#include "stm32l4xx_it.h"

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

void SysTick_Handler(void)
{
 HAL_IncTick();
}

//void PPP_IRQHandler(void)
