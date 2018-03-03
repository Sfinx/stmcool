
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
 HAL_GPIO_WritePin(BUZZER_GPIO_BUS, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
   HAL_GPIO_TogglePin(BUZZER_GPIO_BUS, BUZZER_PIN);
 }
}

void buzzer_init()
{
 GPIO_InitTypeDef gpio = { 0 }; 

 BUZZER_GPIO_CLK_ENABLE();
 gpio.Pin   = BUZZER_PIN;
 gpio.Mode  = GPIO_MODE_OUTPUT_PP;
 gpio.Pull  = GPIO_NOPULL;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(BUZZER_GPIO_BUS, &gpio);

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

void SystemClock_Config(void)
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

void set_led(uchar led, uchar on)
{
 GPIO_TypeDef* bus;
 uint16_t pin;

 switch (led) {
   case RED_LED:
     bus = GPIOA;
     pin = RED_LED_PIN;
     break;
   case GREEN_LED:
     bus = GPIOH;
     pin = GREEN_LED_PIN;
     break;
   case BLUE_LED:
     bus = GPIOH;
     pin = BLUE_LED_PIN;
     break;
   default:
     return;
 }
 if (on)
   HAL_GPIO_WritePin(bus, pin, GPIO_PIN_RESET);
 else
   HAL_GPIO_WritePin(bus, pin, GPIO_PIN_SET);
}

void leds_init()
{
 GPIO_InitTypeDef gpio = { 0 };
 __GPIOH_CLK_ENABLE();
 gpio.Pin   = GREEN_LED_PIN | BLUE_LED_PIN;
 gpio.Mode  = GPIO_MODE_OUTPUT_PP;
 gpio.Pull  = GPIO_NOPULL;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(GPIOH, &gpio);
 __GPIOA_CLK_ENABLE();
 gpio.Pin   = RED_LED_PIN;
 HAL_GPIO_Init(GPIOA, &gpio);
 set_led(RED_LED, 0);
 set_led(GREEN_LED, 0);
 set_led(BLUE_LED, 0);
}

void fan_sensors_init()
{
 uchar i;
 GPIO_InitTypeDef gpio = { 0 };
 FAN_GPIO_CLK_ENABLE();
 gpio.Pin = (FAN0_GPIO | FAN1_GPIO | FAN2_GPIO | FAN3_GPIO | FAN4_GPIO | FAN5_GPIO |
   FAN6_GPIO | FAN7_GPIO);
 gpio.Mode = GPIO_MODE_IT_RISING;
 gpio.Pull  = GPIO_PULLUP;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(FAN_GPIO_BUS, &gpio);
 // fans are connected to EXTI_Line[0..7], which has EXTI[0..7]_IRQn vector
 for (i = 0; i < 5; i++) {
   HAL_NVIC_SetPriority(EXTI0_IRQn + i, 2, 0);
   HAL_NVIC_EnableIRQ(EXTI0_IRQn + i);
 }
 HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
 HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
 HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

ADC_HandleTypeDef hadc1;

void temp_init()
{
 ADC_ChannelConfTypeDef sConfig;
 __HAL_RCC_ADC_CLK_ENABLE();
 hadc1.Instance = ADC1;
 hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
 hadc1.Init.Resolution = ADC_RESOLUTION_12B;
 hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
 hadc1.Init.ContinuousConvMode = DISABLE;
 hadc1.Init.DiscontinuousConvMode = DISABLE;
 hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
 hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
 hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
 hadc1.Init.NbrOfConversion = 1;
 hadc1.Init.DMAContinuousRequests = DISABLE;
 hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
 hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
 hadc1.Init.OversamplingMode = DISABLE;
 hadc1.Init.DMAContinuousRequests = DISABLE;
 if (HAL_ADC_Init(&hadc1) != HAL_OK)
   panic(ADC_INIT_OOPS);
 sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
 sConfig.Rank = 1;
 sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
 sConfig.SingleDiff = ADC_SINGLE_ENDED;
 sConfig.OffsetNumber = ADC_OFFSET_NONE;
 sConfig.Offset = 0;
 if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  panic(TEMP_SENSOR_OOPS);
 while(HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK);
}

#define VDDA_APPLI                       ((uint32_t)3300)

short get_mcu_temp()
{
 HAL_ADC_Start(&hadc1);
 if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
   status.mcu_temp = __HAL_ADC_CALC_TEMPERATURE(VDDA_APPLI, HAL_ADC_GetValue(&hadc1), ADC_RESOLUTION_12B);
   HAL_ADC_Stop(&hadc1);
 } else
     status.mcu_temp = -1;
 return status.mcu_temp;
}

void board_init()
{
 memset(&status, 0, sizeof(status));
 HAL_Init();
 SystemClock_Config();
 leds_init();
 buzzer_init();
 temp_init();
 fan_sensors_init();
 debug("%sstmcool powered on\n\r%s", RTT_CTRL_TEXT_GREEN, RTT_CTRL_RESET);
}

void leds_off()
{
 set_led(RED_LED, 0);
 set_led(GREEN_LED, 0);
 set_led(BLUE_LED, 0);
}

void panic(int i)
{
 status.panic = 1;
 leds_off();
 debug("%sOops [%d] !\r\n%s", RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
 while(1) {
   static char count;
   set_led(RED_LED, 1);
   HAL_Delay(PANIC_DELAY);
   set_led(RED_LED, 0);
   HAL_Delay(PANIC_DELAY);
   if (++count > 2) {
     beep(BEEP_DELAY * 2);
     count = 0;
   }
 }
}
