
#include "app.h"

u32 get_fan(u8 fan)
{
 if (fan > MAX_RPM_SENSORS)
   panic(FAN_OOPS);
 u32 f = (status.fan[fan]*60)/2;
 status.fan[fan] = 0;
 return f;
}

void user_btn_cb(uchar pressed)
{
 // usb_cdc_printf("%s user_btn %s\n\r", mcu_time(), pressed ? "pressed" : "released");
 (void)pressed;
}

void set_time(const char *t)
{
 long long int tm;
 if ((strlen(t) != 10) || sscanf(t, "%lld", &tm) != 1) {
   send_tty_printf("\r\nInvalid timestamp");
   return;
 }
 status.time = tm;
 status.seconds = status.milliseconds = 0;
 send_tty_printf("\r\ndevice time set to %s", mcu_time(0));
}

void help()
{
 send_tty_str("\r\n\nSTMCool help:\r\n");
 send_tty_str("\t?/h\t- help\r\n");
 send_tty_str("\tr\t- iwdt reset by app hang\r\n");
 send_tty_str("\tR\t- iwdt reset by h/w crash\r\n");
 send_tty_str("\tt <u>\t- set time from unix timestamp\r\n");
 send_tty_str("\ti\t- info\r\n");
}

void info()
{
 send_tty_str("\r\n");
 for (uchar i = 0; i < MAX_RPM_SENSORS; i++)
   send_tty_printf("fan%d:%d rpm\r\n", i, get_fan(i));
 send_tty_printf("mcu_temp: %d C\r\n", get_mcu_temp());
 send_tty_printf("reset type: %s [0x%x]\r\n", get_reset_type_str(), status.reset_type);
 send_tty_printf("device time is %s\r\n", status.time ? mcu_time(0) : "not set");
 send_tty_printf("uptime: %s", mcu_time(1));
}

void exec_cmd()
{
 const char *cmd = status.cmd;
 switch(cmd[0]) {
   case 't':
     set_time(cmd + 2);
     break;
   case 'R':
     *(__IO uint32_t *) 0x00040001 = 0xFF; // trigger infinite h/w fault
     break;
   case 'r':
     while(1);
   case 'h':
   case '?':
     help();
     break;
   case 'i':
     info();
     break;
   default:
     send_tty_str("\n\rUnknown cmd: [");
     send_tty_str(cmd);
     send_tty_str("], try h");
     break;
 }
 send_tty_str("\n\r>");
}

void app_blink()
{
 static char ledv;
 static ushort oldt;
 if (abs(oldt - status.milliseconds) > APP_LED_DELAY) {
   oldt = status.milliseconds;
   set_led(GREEN_LED, ledv);
   ledv = ledv ? 0 : 1;
 }
}

static ring_t tty_out;

void app()
{
 char c[2] = { 0 };
 if (ring_read(&tty_out, (uint8_t *)&c, 1))
   send_tty_str(c);
 wdt_reset();
 app_blink();
 if (status.new_cmd) {
   exec_cmd();
   status.new_cmd = 0;
 }
}

void usb_cdc_ready()
{
 uint32_t s0 = (get_serial() >> 32);
 uint32_t s1 = (get_serial() & 0xFFFFFFFF);
 usb_cdc_printf("\r\n%s STMcool [ s/n %08X%08X ]%s\r\n>", mcu_time(0), s0, s1,
   status.cdc_ok ? "" : " booted ok");
 status.cdc_ok = 1;
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

void process_input(char *b, uchar len)
{
 static char cmd[MAX_CMD_HISTORY][MAX_CMD_SIZE + 1];
 static uchar curr_history_idx, curr_cmd_idx;;
 if (len == 1) {
   switch (b[0]) {
     case 0xC: // clear screen
       usb_cdc_send_char(b[0]);
       break;
     case 0x1B:
       break;
     case 0xd:
       if (curr_cmd_idx) {
         status.cmd = (const char *)&(cmd[curr_history_idx++]);
         status.new_cmd = 1;
         if (curr_history_idx >= MAX_CMD_HISTORY)
           curr_history_idx = 0;
       } else // print prompt
           usb_cdc_send_str("\n\r>");
       memset(cmd[curr_history_idx], 0, MAX_CMD_SIZE);
       curr_cmd_idx = 0;
       break;
     case 0x8:
       if (curr_cmd_idx) {
         curr_cmd_idx--;
         cmd[curr_history_idx][curr_cmd_idx] = 0;
         char q[] = { b[0], ' ', b[0], 0 };
         usb_cdc_send_str(q);
       }
       break;
     default:
       cmd[curr_history_idx][curr_cmd_idx++] = b[0];
       if (curr_cmd_idx > MAX_CMD_SIZE) {
         curr_cmd_idx--;
         cmd[curr_history_idx][curr_cmd_idx] = 0;
         beep(20);
       } else
           ring_write(&tty_out, (uint8_t *)b, 1);
   }
 } else if (len == 2) {
   if (b[0] == 0x5B) {
     switch(b[1]) {
       case 0x41: // up
         usb_cdc_send_str("todo: up\n\r>");
         break;
       case 0x42: // down
         usb_cdc_send_str("todo: down\n\r>");
         break;
       case 0x43: // right
         usb_cdc_send_str("todo: right\n\r>");
         break;
       case 0x44: // left
         usb_cdc_send_str("todo: left\n\r>");
         break;
     }
   }
 }
}

void usb_cdc_rx_cb(uint8_t* b, uint32_t len)
{
 // ugly: add 0 byte
 b[len] = 0;
 process_input((char*)b, len);
}

// called from systick interrupt
void _1_sec_tick()
{
}

// called from systick interrupt
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
