
#include "app.h"

void app()
{
 set_led(GREEN_LED, 1);
 HAL_Delay(LED_DELAY);
 set_led(GREEN_LED, 0);
 HAL_Delay(LED_DELAY);
}

void main(void)
{
 board_init();
 beep(BEEP_DELAY);
 HAL_Delay(BEEP_DELAY);
 beep(BEEP_DELAY);
 while (1) {
   if (!status.panic)
     app();
 }
}

u32 get_fan(u8 fan)
{
 if (fan > MAX_RPM_SENSORS)
   panic(FAN_OOPS);
 u32 f = (status.fan[fan]*60)/2;
 status.fan[fan] = 0;
 return f;
}

void _1_sec_tick()
{
 if (!status.seconds) {
   uint32_t s0 = (get_serial() >> 32);
   uint32_t s1 = (get_serial() & 0xFFFFFFFF);
   usb_cdc_printf("\r\n%s STMcool [ s/n %08X%08X ] booted ok\r\n", mcu_time(), s0, s1);
 }
 uint i = 0;
 for (; i < MAX_RPM_SENSORS; i++) {
   u32 fan = get_fan(i);
   if (fan)
     usb_cdc_printf("%s fan%d:%d\r\n", mcu_time(), i, fan);
 }
 usb_cdc_printf("%s mcu_temp: %d C\r\n", mcu_time(), get_mcu_temp());
}

void user_btn_cb(uchar pressed)
{
 usb_cdc_printf("%s user_btn %s\n\r", mcu_time(), pressed ? "pressed" : "released");
}

#include <string.h>

void usb_cdc_rx_cb(uint8_t* b, uint32_t len)
{
 // ugly: add 0 byte
 b[len] = 0;
 debug("usb_cdc_rx_cb: %d [%s]\n", len, b);
}

void _100_ms_tick()
{
}

status_t status;

uchar p2f[MAX_RPM_SENSORS] = { 6, 7, 1, 0, 2, 3, 4, 5 };

u8 pin2fan(u32 pin)
{
 if (pin < MAX_RPM_SENSORS)
   panic(FAN_OOPS);
 return p2f[pin - MAX_RPM_SENSORS];
}
