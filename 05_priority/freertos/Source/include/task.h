#ifndef INC_TASK_H
#define INC_TASK_H

#include "list.h"

#define taskYIELD()								portYIELD()

typedef void* TaskHandle_t;

#if(configSUPPORT_STATIC_ALLOCATION == 1)

TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,
					            const char * const pcName,
					            const uint32_t ulStackDepth,
					            void * const pvParameters,
					            StackType_t * const puxStackBuffer,
					            TCB_t * const pxTaskBuffer );

#endif /*configSUPPORT_STATIC_ALLOCATION*/

void prvInitialiseTaskLists();
void vTaskStartScheduler( void );
void vTaskSwitchContext( void );
void xTaskIncrementTick( void );
void vTaskDelay(const TickType_t xTicksToDelay);

// 進入與退出critical section之定義
#define taskENTER_CRITICAL() 					portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR()	portSET_INTERRUPT_MASK_FROM_ISR()

#define taskEXIT_CRITICAL() 					portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR()	portCLEAR_INTERRUPT_MASK_FROM_ISR()

#endif /* INC_TASK_H */ 