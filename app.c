
#include "app.h"

int main(void)
{
 board_init();
 while (1) {
   set_leds(1);
   HAL_Delay(LED_DELAY);
   set_leds(0);
   HAL_Delay(LED_DELAY);
 }
}
