
#pragma once

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_pcd.h"

#define USBD_MAX_NUM_INTERFACES               1
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0x100
#define USBD_SUPPORT_USER_STRING              0 
#define USBD_SELF_POWERED                     1
#define USBD_DEBUG_LEVEL                      0
/* Memory management macros */
/* For footprint reasons and since only one allocation is handled in the CDC class
   driver, the malloc/free is changed into a static allocation method */

void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#define MAX_STATIC_ALLOC_SIZE     136 /*CDC Class Driver Structure size*/

#define USBD_malloc               (uint32_t *)USBD_static_malloc
#define USBD_free                 USBD_static_free
#define USBD_memset               /* Not used */
#define USBD_memcpy               /* Not used */
 
/* DEBUG macros */  
#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   debug(__VA_ARGS__)
#else
#define USBD_UsrLog(...)   
#endif                                                        
#if (USBD_DEBUG_LEVEL > 1)
#define  USBD_ErrLog(...)   debug(__VA_ARGS__)
#else
#define USBD_ErrLog(...)   
#endif                                                         
#if (USBD_DEBUG_LEVEL > 2)                         
#define  USBD_DbgLog(...)   debug(__VA_ARGS__)
#else
#define USBD_DbgLog(...)                         
#endif

extern PCD_HandleTypeDef hpcd;
