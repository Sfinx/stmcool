
#include <app.h>

int main(void)
{
 board_init();
 beep(70);
 while (1)  {
   set_led(GREEN_LED, 1);
   set_led(BLUE_LED, 0);
   delay_ms(500);
   set_led(GREEN_LED, 0);
   set_led(BLUE_LED, 1);
   delay_ms(500);
 }
}
