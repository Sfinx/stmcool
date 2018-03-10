
#include "app.h"
#include "usbd_cdc_interface.h"

USBD_CDC_LineCodingTypeDef LineCoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

#define APP_RX_DATA_SIZE  2048

uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
extern USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
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

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{
 USBD_CDC_SetTxBuffer(&USBD_Device, 0, 0);
 USBD_CDC_SetRxBuffer(&USBD_Device, 0);
 return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
 return (USBD_OK);
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static char cdc_not_ready = 1;

static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case CDC_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_SET_LINE_CODING:
    LineCoding.bitrate = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
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
    /* Add your code here */
    break;

  case CDC_SEND_BREAK:
     /* Add your code here */
    break;    
    
  default:
    break;
  }
  return (USBD_OK);
}

#include <string.h>
#include <stdlib.h>

uint8_t usb_cdc_send(const uint8_t* buf, uint16_t len)
{
 if (cdc_not_ready)
   return USBD_BUSY;
 USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*) USBD_Device.pClassData;
 if (hcdc->TxState != 0) {
   blink(RED_LED);
   return USBD_BUSY;
 }
 blink(BLUE_LED);
 // !!! we use the unknown external (may be temp) buffer here
 USBD_CDC_SetTxBuffer(&USBD_Device, (uint8_t *)buf, len);
 return USBD_CDC_TransmitPacket(&USBD_Device);
}

uint8_t usb_cdc_send_str(const char *s)
{
 return usb_cdc_send((const uint8_t *)s, strlen(s));
}

#include <stdarg.h>

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

/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */

static int8_t CDC_Itf_Receive(uint8_t* buf, uint32_t *len)
{
 blink(BLUE_LED);
 if (*len > APP_RX_DATA_SIZE) {
   debug("cdc_rx: too big buflen:%d\n", *len);
   return USBD_OK;
 }
 USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);
 USBD_CDC_ReceivePacket(&USBD_Device);
 usb_cdc_rx_cb(UserRxBuffer, *len);
 return (USBD_OK);
}
