
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
   debug("shit\n");
 u32 f = (status.fan[fan]*60)/2;
 status.fan[fan] = 0;
 return f;
}

static int counter;

void _1_sec_tick()
{
 if (!counter)
   usb_cdc_send_str("\r\nSTMcool booted ok\n\r");
 uint i = 0;
 for (; i < MAX_RPM_SENSORS; i++) {
   u32 fan = get_fan(i);
   if (fan)
     usb_cdc_printf("%d: fan%d:%d\r\n", counter++, i, fan);
 }
 usb_cdc_printf("%d: mcu_temp: %d C\n\r", counter++, get_mcu_temp());
}

#include <string.h>

void usb_cdc_send_rx_cb(uint8_t* buf, uint32_t len)
{
 char b[128] = { 0 };
 if (len < 128) {
  memcpy(b, buf, len);
  debug("usb_cdc_send_rx_cb: %s\n", b);
 }
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
