
#include "app.h"
#include "usbd_cdc_interface.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

USBD_CDC_LineCodingTypeDef LineCoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

#define CDC_RX_DATA_SIZE  2048

uint8_t UserRxBuffer[CDC_RX_DATA_SIZE];
extern USBD_HandleTypeDef  USBD_Device;

static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};

static int8_t CDC_Itf_Init(void)
{
 USBD_CDC_SetTxBuffer(&USBD_Device, 0, 0);
 USBD_CDC_SetRxBuffer(&USBD_Device, 0);
 return (USBD_OK);
}

static int8_t CDC_Itf_DeInit(void)
{
 return (USBD_OK);
}

static char cdc_not_ready = 1;

static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
 switch (cmd) {
   case CDC_SEND_ENCAPSULATED_COMMAND:
     break;
   case CDC_GET_ENCAPSULATED_RESPONSE:
     break;
   case CDC_SET_COMM_FEATURE:
     break;
   case CDC_GET_COMM_FEATURE:
     break;
   case CDC_CLEAR_COMM_FEATURE:
     break;
   case CDC_SET_LINE_CODING:
     LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
     LineCoding.format     = pbuf[4];
     LineCoding.paritytype = pbuf[5];
     LineCoding.datatype   = pbuf[6];
     // debug("speed:%d format:%d, parity:%d, datatype:%d\n", LineCoding.bitrate, LineCoding.format,
     //  LineCoding.paritytype, LineCoding.datatype);
     break;
   case CDC_GET_LINE_CODING:
     pbuf[0] = (uint8_t)(LineCoding.bitrate);
     pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
     pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
     pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
     pbuf[4] = LineCoding.format;
     pbuf[5] = LineCoding.paritytype;
     pbuf[6] = LineCoding.datatype;     
     break;
   case CDC_SET_CONTROL_LINE_STATE:
     cdc_not_ready = 0;
     break;
   case CDC_SEND_BREAK:
     break;        
   default:
     break;
 }
 return (USBD_OK);
}

#define MAX_CDC_TX_SIZE	128

uint8_t usb_cdc_send(const uint8_t* buf, uint16_t len)
{
 static char tx_b[MAX_CDC_TX_SIZE];
 if (cdc_not_ready || (len > MAX_CDC_TX_SIZE))
   return USBD_BUSY;
 USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*) USBD_Device.pClassData;
 if (hcdc->TxState != 0) {
   blink(RED_LED);
   return USBD_BUSY;
 }
 blink(BLUE_LED);
 memcpy(tx_b, buf, len);
 USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t *)tx_b, len);
 return USBD_CDC_TransmitPacket(&USBD_Device);
}

uint8_t usb_cdc_send_str(const char *s)
{
 return usb_cdc_send((const uint8_t *)s, strlen(s));
}

uint8_t usb_cdc_printf(const char *fmt, ...)
{
 char b[128] = { 0 };
 va_list ap;
 va_start(ap, fmt);
 vsnprintf(b, sizeof(b), fmt, ap);
 uint8_t r = usb_cdc_send_str(b);
 va_end(ap);
 return r;
}

static int8_t CDC_Itf_Receive(uint8_t* buf, uint32_t *len)
{
 blink(BLUE_LED);
 if (*len > CDC_RX_DATA_SIZE) {
   debug("cdc_rx: too big packet:%d\n", *len);
   return USBD_OK;
 }
 USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);
 USBD_CDC_ReceivePacket(&USBD_Device);
 usb_cdc_rx_cb(UserRxBuffer, *len);
 return (USBD_OK);
}
