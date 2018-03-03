
#include "app.h"

void app()
{
 set_led(GREEN_LED, 1);
 HAL_Delay(LED_DELAY);
 set_led(GREEN_LED, 0);
 HAL_Delay(LED_DELAY);
 debug("mcu_temp: %d C\n", get_mcu_temp());
}

void main(void)
{
 board_init();
 beep(300);
 while (1) {
   if (!status.panic)
     app();
 }
}

u32 get_fan(u8 fan)
{
 u32 f = (status.fan[fan]*60)/2;
 status.fan[fan] = 0;
 return f;
}

#ifdef USE_RTT
static int counter;
#endif

void _1_sec_tick()
{
#ifdef USE_RTT
 uint i = 0;
 for (; i < MAX_RPM_SENSORS; i++)
   debug("%d: fan%d:%d\r\n", counter++, i, get_fan(i));
#endif
}

void _100_ms_tick()
{
}

status_t status;
