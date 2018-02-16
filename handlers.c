
#include <stm32f10x_it.h>
#include <app.h>

void NMIException(void)
{
 panic(NMI_OOPS);
}

void HardFaultException(void)
{
 panic(HARDFAULT_OOPS);
}

void MemManageException(void)
{
 panic(MEMMANAGE_OOPS);
}

void BusFaultException(void)
{
 panic(BUSFAULT_OOPS);
}

void UsageFaultException(void)
{
 panic(USAGEFAULT_OOPS);
}

void DebugMonitor(void)
{
 panic(DEBUGMONITOR_OOPS);
}

void SVCHandler(void)
{
 panic(SVC_OOPS);
}

void PendSVC(void)
{
 panic(PEND_SVC_OOPS);
}

// 1ms tick (1000 Hz)
void SysTick_Handler(void)
{
 if (!(status._1ms_tick % 100))
   _100_ms_tick(); 
 if (status._1ms_tick++ > 1000) {
   status.seconds++;
   _1_sec_tick();
   status._1ms_tick = 0;
 }
}

static u32 buzzer_cnt;

extern void buzzer(u32 on);
extern void toggle_buzzer();

void TIM2_IRQHandler(void)
{
 if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   if (!status.buzzer_timer--)
     buzzer(0);
   if (status.buzzer_on && (buzzer_cnt++ > BUZZER_FREQ)) {
     buzzer_cnt = 0;
     toggle_buzzer();
   }
 }
}

void WWDG_IRQHandler(void)
{
 panic(WWDG_OOPS);
}

void PVD_IRQHandler(void)
{
 panic(PVD_OOPS);
}

void TAMPER_IRQHandler(void) { }

void RTC_IRQHandler(void) { }

void FLASH_IRQHandler(void)
{
 panic(FLASH_OOPS);
}

void RCC_IRQHandler(void)
{
 panic(RCC_OOPS);
}

#define fan_interrupt(x, y) void x ## _IRQHandler(void)\
                         {\
                           if (EXTI_GetITStatus(EXTI_Line##y) != RESET) { \
                             if (GPIO_ReadInputDataBit(GPIOC, FAN##y)) \
                               status.fan[y]++; \
                             EXTI_ClearITPendingBit(EXTI_Line##y); \
                           } \
                         }

fan_interrupt(EXTI0, 0)
fan_interrupt(EXTI1, 1)
fan_interrupt(EXTI2, 2)
fan_interrupt(EXTI3, 3)
fan_interrupt(EXTI4, 4)

void EXTI9_5_IRQHandler(void)
{
 if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
   status.temp[0]++;
   EXTI_ClearITPendingBit(EXTI_Line5);
 }
}

void EXTI15_10_IRQHandler(void) { }

void DMAChannel1_IRQHandler(void) { }

void DMAChannel2_IRQHandler(void) { }

void DMAChannel3_IRQHandler(void) { }

void DMAChannel4_IRQHandler(void) { }

void DMAChannel5_IRQHandler(void) { }

void DMAChannel6_IRQHandler(void) { }

void DMAChannel7_IRQHandler(void) { }

void ADC_IRQHandler(void) { }

void USB_HP_CAN_TX_IRQHandler(void) { }

void USB_LP_CAN_RX0_IRQHandler(void) { }

void CAN_RX1_IRQHandler(void) { }

void CAN_SCE_IRQHandler(void) { }

void TIM1_BRK_IRQHandler(void) { }

void TIM1_UP_IRQHandler(void) { }

void TIM1_TRG_COM_IRQHandler(void) { }

void TIM1_CC_IRQHandler(void) { }

void TIM3_IRQHandler(void) { }

void TIM4_IRQHandler(void) { }

void I2C1_EV_IRQHandler(void) { }

void I2C1_ER_IRQHandler(void) { }

void I2C2_EV_IRQHandler(void) { }

void I2C2_ER_IRQHandler(void) { }

void SPI1_IRQHandler(void) { }

void SPI2_IRQHandler(void) { }

void USART1_IRQHandler(void) { }

void USART2_IRQHandler(void) { }

void USART3_IRQHandler(void) { }

void RTCAlarm_IRQHandler(void) { }

void USBWakeUp_IRQHandler(void) { }
