
#include <stm32f10x_it.h>
#include <app.h>

void NMIException(void)
{
 panic(0);
}

void HardFaultException(void)
{
 panic(1);
}

void MemManageException(void)
{
 panic(2);
}

void BusFaultException(void)
{
 panic(3);
}

void UsageFaultException(void)
{
 panic(4);
}

void DebugMonitor(void)
{
 panic(5);
}

void SVCHandler(void)
{
 panic(6);
}

void PendSVC(void)
{
 panic(7);
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
   if (status.buzzer_on && (buzzer_cnt++)) { // buzzer_cnt - freq divider
     buzzer_cnt = 0;
     toggle_buzzer();
   }
 }
}

void WWDG_IRQHandler(void)
{
 panic(8);
}

void PVD_IRQHandler(void)
{
 panic(10);
}

void TAMPER_IRQHandler(void) { }

void RTC_IRQHandler(void) { }

void FLASH_IRQHandler(void) { }

void RCC_IRQHandler(void)
{
 panic(9);
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
