
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
 if (status.panic)
   return;
 if (status.user_btn != HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
   status.user_btn = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
   user_btn_cb(status.user_btn);
 }
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

#define fan_interrupt(x, y) void x ## _IRQHandler(void) \
  { \
    HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn + y); \
    HAL_GPIO_EXTI_IRQHandler((GPIO_PIN_0 << y)); \
  }

fan_interrupt(EXTI0, 0)
fan_interrupt(EXTI1, 1)
fan_interrupt(EXTI2, 2)
fan_interrupt(EXTI3, 3)
fan_interrupt(EXTI4, 4)

void EXTI9_5_IRQHandler(void)
{
 uchar i;
 for (i = 5; i <= 9; i++) {
   if (__HAL_GPIO_EXTI_GET_IT((GPIO_PIN_0 << i)) != RESET) {
     HAL_GPIO_EXTI_IRQHandler((GPIO_PIN_0 << i));
     break;
   }
 }
 HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
 uchar i;
 for (i = 10; i <= 15; i++) {
   if (__HAL_GPIO_EXTI_GET_IT((GPIO_PIN_0 << i)) != RESET) {
     HAL_GPIO_EXTI_IRQHandler((GPIO_PIN_0 << i));
     break;
   }
 }
 HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
 u8 pin_num = __builtin_ctz(pin);
 if (pin_num >= 8) { // fans
   if (HAL_GPIO_ReadPin(FAN_GPIO_BUS, (GPIO_PIN_0 << pin_num)))
     status.fan[pin2fan(pin_num)]++;
 } else {
   // process temps
   debug("HAL_GPIO_EXTI_Callback: temp%d\r\n", pin_num);
 }
}

void USB_IRQHandler(void)
{
 HAL_PCD_IRQHandler(&hpcd);
}

//void PPP_IRQHandler(void)
