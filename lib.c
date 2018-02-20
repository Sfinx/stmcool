
#include <app.h>

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
  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();
  // Configure the System clock to have a frequency of 80 MHz
  SystemClock_Config();
  leds_init();
}

void panic(int i)
{
 SEGGER_RTT_printf(0, "%sOops [%d] !\r\n%s", RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
 while(1);
}
