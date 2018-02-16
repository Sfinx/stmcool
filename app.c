
#include <app.h>

void _100_ms_tick()
{
}

u32 get_fan(u8 fan)
{
 u32 f = (status.fan[fan]*60)/2;
 status.fan[fan] = 0;
 return f;
}

u32 get_temp(u8 temp)
{
 u32 t = status.temp[temp];
 // status.temp[temp] = 0;
 return t;
}

#ifdef USE_RTT
static int counter;
#endif

void _1_sec_tick()
{
 uint i = 0;
 for (; i < MAX_FANS; i++)
   SEGGER_RTT_printf(0, "%d: fan%d:%d\r\n", counter++, i, get_fan(i));
 SEGGER_RTT_printf(0, "temp0: %d\r\n", get_temp(0));

}

int main(void)
{
 board_init();
 beep(70);
 while (1)  {
   set_led(GREEN_LED, 1);
   set_led(BLUE_LED, 0);
   hard_delay_ms(500);
   set_led(GREEN_LED, 0);
   set_led(BLUE_LED, 1);
   hard_delay_ms(500);
 }
}
