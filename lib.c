
#include <app.h>
#include <string.h>

void beep(u32 t)
{
 if (!status.buzzer_on) {
   status.buzzer_timer = (t * 10);
   status.buzzer_on = 1;
 }
}

void buzzer(u32 on)
{
 status.buzzer_on = on;
 HAL_GPIO_WritePin(GPIOA, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

TIM_HandleTypeDef buzz_timer = { 
  .Instance = TIM2
};

static u32 buzzer_cnt;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if (!status.buzzer_timer--)
   buzzer(0);
 if (status.buzzer_on && (buzzer_cnt++ > BUZZER_FREQ)) {
   buzzer_cnt = 0;
   // toggle buzzer
   HAL_GPIO_TogglePin(GPIOA, BUZZER_PIN);
 }
}

void buzzer_init()
{
 GPIO_InitTypeDef  GPIO_InitStruct = {0}; 

 __GPIOA_CLK_ENABLE();
 GPIO_InitStruct.Pin   = BUZZER_PIN;
 GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull  = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 buzzer(0);
 __TIM2_CLK_ENABLE();
 buzz_timer.Init.Prescaler = 1000;
 buzz_timer.Init.Period = 4;
 buzz_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 buzz_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
 buzz_timer.Init.RepetitionCounter = 0;
 HAL_TIM_Base_Init(&buzz_timer);
 HAL_TIM_Base_Start_IT(&buzz_timer);
 // HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
 HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
 HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  */

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  // MSI is enabled after System reset, activate PLL with MSI as source
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    panic(SYSTEMCLOCK_OOPS);
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  //   clocks dividers
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    panic(SYSTEMCLOCK_OOPS);
}

#ifdef  USE_FULL_ASSERT
void assert_failed(char *file, uint32_t line)
{ 
 debug("Assert faild at %s:%d\r\n", file, line);
 while (1);
}
#endif

static GPIO_TypeDef *GPIO_PORT[LEDn] = {LED4_GPIO_PORT};
static const uint16_t GPIO_PIN[LEDn] = {LED4_PIN};

void set_leds(uint on)
{
 if (on)
   HAL_GPIO_WritePin(GPIO_PORT[0], GPIO_PIN[0], GPIO_PIN_SET);
 else
   HAL_GPIO_WritePin(GPIO_PORT[0], GPIO_PIN[0], GPIO_PIN_RESET);
}

void leds_init()
{
 GPIO_InitTypeDef  GPIO_InitStruct = {0};
 __GPIOB_CLK_ENABLE();
 GPIO_InitStruct.Pin   = GPIO_PIN[0];
 GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull  = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(GPIO_PORT[0], &GPIO_InitStruct);
}

void board_init()
{
 memset(&status, 0, sizeof(status));
 HAL_Init();
 SystemClock_Config();
 leds_init();
 buzzer_init();
 SEGGER_RTT_printf(0, "%sstmcool powered on\n\r%s", RTT_CTRL_TEXT_GREEN, RTT_CTRL_RESET);

}

void panic(int i)
{
 SEGGER_RTT_printf(0, "%sOops [%d] !\r\n%s", RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
 while(1);
}
