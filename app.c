
#include "app.h"

int main(void)
{
 board_init();
 beep(300);
 while (1) {
   set_leds(1);
   HAL_Delay(LED_DELAY);
   set_leds(0);
   HAL_Delay(LED_DELAY);
 }
}

void _1_sec_tick()
{
}

void _100_ms_tick()
{
}

status_t status;
