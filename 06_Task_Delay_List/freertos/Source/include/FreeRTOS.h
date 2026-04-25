#ifndef FREERTOS_H
#define FREERTOS_H

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portable.h"
#include "list.h"

typedef struct tskTaskControlBlock
{
	volatile StackType_t    *pxTopOfStack;    /* Stack 頂 */

	ListItem_t			    		xStateListItem;   /* 任務節點 */
    
  StackType_t             *pxStack;         /* Task Stack 起始地址 */
	                                          
	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];  /* 任務名稱 */
	
	TickType_t							xTicksToDelay;		
	UBaseType_t							uxPriority;
} tskTCB;
typedef tskTCB TCB_t;


#endif /* FREERTOS_H */