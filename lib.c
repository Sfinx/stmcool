
#include <app.h>
#include <string.h>

void blink(uchar led)
{
 set_led(led, 1);
 status.led_blink[led] = 1;
}

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
 (void)htim;
 if (!status.buzzer_timer--)
   buzzer(0);
 if (status.buzzer_on && (buzzer_cnt++ > (status.panic ? PANIC_FREQ : BUZZER_FREQ))) {
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
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  // Enable LSE Oscillator to automatically calibrate the MSI clock
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE; // No PLL update
  RCC_OscInitStruct.LSEState       = RCC_LSE_ON; // External 32.768 kHz clock on OSC_IN/OSC_OUT
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK)
    RCC->CR |= RCC_CR_MSIPLLEN; // Enable MSI PLL-mode

  HAL_RCCEx_DisableLSECSS();

  // Enable MSI/HSI
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState             = RCC_MSI_ON;
  RCC_OscInitStruct.HSIState             = RCC_HSI_OFF;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11; // 48 MHz
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM            = 6;    // 8 MHz
  RCC_OscInitStruct.PLL.PLLN            = 40;   // 320 MHz
  RCC_OscInitStruct.PLL.PLLP            = 7;    // 45 MHz
  RCC_OscInitStruct.PLL.PLLQ            = 4;    // 80 MHz
  RCC_OscInitStruct.PLL.PLLR            = 4;    // 80 MHz

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    panic(SYSTEMCLOCK_OOPS);

  // Enable MSI Auto-calibration through LSE
  HAL_RCCEx_EnableMSIPLLMode();
  // Select MSI output as USB clock source
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI; // 48 MHz
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; // 80 MHz
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;         // 80 MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;           // 80 MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;           // 80 MHz

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) // FLASH_LATENCY_4
    panic(SYSTEMCLOCK_OOPS);

  // Enable Power Controller clock
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddUSB();
}

#ifdef  USE_FULL_ASSERT
void assert_failed(char *file, uint32_t line)
{
 debug("Assert failed at %s:%d\r\n", file, line);
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

// PC13
void user_btn_init()
{
 GPIO_InitTypeDef gpio = { 0 };
 __GPIOC_CLK_ENABLE();
 gpio.Pin = GPIO_PIN_13;
 gpio.Pull  = GPIO_NOPULL;
 gpio.Mode = GPIO_MODE_INPUT;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(GPIOC, &gpio);
}

void fan_sensors_init()
{
 GPIO_InitTypeDef gpio = { 0 };
 FAN_GPIO_CLK_ENABLE();
 gpio.Pin = (FAN0_GPIO | FAN1_GPIO | FAN2_GPIO | FAN3_GPIO | FAN4_GPIO | FAN5_GPIO |
   FAN6_GPIO | FAN7_GPIO);
 gpio.Mode = GPIO_MODE_IT_RISING;
 gpio.Pull  = GPIO_PULLUP;
 gpio.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(FAN_GPIO_BUS, &gpio);
 // fans are connected to EXTI_Line[0..7], which has EXTI[0..7]_IRQn vector
 for (uchar i = 0; i < 5; i++) {
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

#include "usbd_desc.h"
#include "usbd_cdc_interface.h"

USBD_HandleTypeDef USBD_Device;

void usb_init()
{
 // Init Device Library
 if (USBD_Init(&USBD_Device, &VCP_Desc, 0) != USBD_OK)
   panic(USB_OOPS);
 // Register the HID class
 if (USBD_RegisterClass(&USBD_Device, &USBD_CDC) != USBD_OK)
   panic(USB_OOPS);
 if (USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops) != USBD_OK)
   panic(USB_OOPS);
 if (USBD_Start(&USBD_Device) != USBD_OK)
   panic(USB_OOPS);
}

static IWDG_HandleTypeDef IwdgHandle;

void wdt_reset()
{
 HAL_IWDG_Refresh(&IwdgHandle);
}

#define	WDT_TIMEOUT	2

void wdt_init()
{
 __HAL_RCC_CLEAR_RESET_FLAGS();
 /* Set counter reload value to obtain 1 sec IWDG TimeOut
     IWDG counter clock Frequency = LSI_Freq
     Set Prescaler to 32 (IWDG_PRESCALER_32)
     Timeout Period = (Reload Counter Value * 32) / LSI_Freq
     So Set Reload Counter Value = (1 * LSI_Freq) / 32 */
 IwdgHandle.Instance = IWDG;
 IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
 IwdgHandle.Init.Reload = (32000 / 32) * WDT_TIMEOUT; // LSI freq is 32kHz = ~1000ms = 1 second
 IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;
 if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
  panic(WDT_OOPS);
}

const char *get_reset_type_str()
{
 switch (status.reset_type) {
   case 0x4:
     return "button";
   case 0x14:
     return "software";
   case 0x24:
     return "iwdt";
   case 0xC:
     return "power";
   default:
     break;
 }
 return "unknown";
}

void board_init()
{
 memset(&status, 0, sizeof(status));
 status.reset_type = (RCC->CSR >> 24);
 HAL_Init();
 SystemClock_Config();
 user_btn_init();
 leds_init();
 buzzer_init();
 usb_init();
 temp_init();
 fan_sensors_init();
 wdt_init();
 debug("%sstmcool powered on\r\n%s", RTT_CTRL_TEXT_GREEN, RTT_CTRL_RESET);
}

void leds_off()
{
 set_led(RED_LED, 0);
 set_led(GREEN_LED, 0);
 set_led(BLUE_LED, 0);
}

#define CYCLES_PER_LOOP		3

inline void wait_cycles(uint32_t n)
{
 uint32_t l = n/CYCLES_PER_LOOP;
 asm volatile("0:" "SUBS %[count], 1;" "BNE 0b;" :[count]"+r"(l));
}

void hard_delay(uint32_t d)
{
 wait_cycles(d);
}

void panic(int i)
{
 if (status.panic)
   return;
 status.panic = 1;
 leds_off();
 debug("%sOops [%d] !\r\n%s", RTT_CTRL_BG_BRIGHT_RED, i, RTT_CTRL_RESET);
 while(1) {
   static char count;
   set_led(RED_LED, 1);
   hard_delay(0xFFFFFF);
   set_led(RED_LED, 0);
   hard_delay(0xFFFFFF);
   if (++count > 2) {
     beep(BEEP_DELAY * 2);
     count = 0;
   }
 }
}

// stolen from glibc

struct tm {
 int tm_sec;    /* Seconds (0-60) */
 int tm_min;    /* Minutes (0-59) */
 int tm_hour;   /* Hours (0-23) */
 int tm_mday;   /* Day of the month (1-31) */
 int tm_mon;    /* Month (0-11) */
 int tm_year;   /* Year - 1900 */
 int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
 int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
 int tm_isdst;  /* Daylight saving time */
};

#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)
# define __isleap(year) \
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

const unsigned short int __mon_yday[2][13] =
  {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
  };

struct tm *gmtime(time_t epoch)
{
 static struct tm tb;
 struct tm *tp = &tb;
 long int offset = 0; // seconds east of UTC
 time_t days, rem, y;
 const unsigned short int *ip;

 days = epoch / SECS_PER_DAY;
 rem = epoch % SECS_PER_DAY;
 rem += offset;
 while (rem < 0) {
   rem += SECS_PER_DAY;
   --days;
 }
 while (rem >= SECS_PER_DAY) {
   rem -= SECS_PER_DAY;
   ++days;
 }
 tp->tm_hour = rem / SECS_PER_HOUR;
 rem %= SECS_PER_HOUR;
 tp->tm_min = rem / 60;
 tp->tm_sec = rem % 60;
 /* January 1, 1970 was a Thursday.  */
 tp->tm_wday = (4 + days) % 7;
 if (tp->tm_wday < 0)
   tp->tm_wday += 7;
 y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

 while (days < 0 || days >= (__isleap (y) ? 366 : 365)) {
   /* Guess a corrected year, assuming 365 days per year.  */
   time_t yg = y + days / 365 - (days % 365 < 0);
   /* Adjust DAYS and Y to match the guessed year.  */
   days -= ((yg - y) * 365 + LEAPS_THRU_END_OF (yg - 1) - LEAPS_THRU_END_OF (y - 1));
   y = yg;
 }
 tp->tm_year = y; // - 1900;
 tp->tm_yday = days;
 ip = __mon_yday[__isleap(y)];
 for (y = 11; days < (long int) ip[y]; --y)
   continue;
 days -= ip[y];
 tp->tm_mon = y + 1;
 tp->tm_mday = days + 1;
 return tp;
}

const char *mcu_time(uchar uptime)
{
 static char b[32];
 if (!uptime && status.time) {
   struct tm *t = gmtime(status.seconds + status.time);
   sprintf(b, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]", t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour,
     t->tm_min, t->tm_sec, status.milliseconds);
 } else {
     uchar days = (status.seconds / 86400);
     uchar hours = (status.seconds / 3600) % 24;
     uchar minutes = (status.seconds / 60) % 60;
     uchar seconds = status.seconds % 60;
     sprintf(b, "[%04d:%02d:%02d:%02d.%03d]", days, hours, minutes, seconds, status.milliseconds);
 }
 return b;
}

uint64_t get_serial()
{
 return 0x1ULL;
}
