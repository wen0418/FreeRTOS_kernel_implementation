/*
**************************************************************************************
*                                include herder file
**************************************************************************************
*/

#include "FreeRTOS.h"
#include "task.h"

/*
**************************************************************************************
*                                全局變數
**************************************************************************************
*/

portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*
**************************************************************************************
*                                TCB setting
**************************************************************************************
*/

TaskHandle_t Task1Handle;
#define TASK1_STACK_SIZE					128
StackType_t Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;

TaskHandle_t Task2Handle;
#define TASK2_STACK_SIZE					128
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

/* 定義 IdleTask 的 Stack */
#define configMINIMAL_STACK_SIZE 	((unsigned) 128)
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/* 定義 IdleTask 的 TCB ( 是一個全域變數 ) */
TCB_t IdleTaskTCB;

/*
**************************************************************************************
*                                函數聲明
**************************************************************************************
*/

void delay(volatile uint32_t count);
void Task1_Entry(void* p_arg);
void Task2_Entry(void* p_arg);

/*
**************************************************************************************
*                                main 函數
**************************************************************************************
*/

int main(void){
	/* 初始化就緒列表 */
	prvInitialiseTaskLists();
	
	/* 創建任務 */
	Task1Handle = xTaskCreateStatic((TaskFunction_t)Task1_Entry,
																	(char*)"Task1",
																  (uint32_t)TASK1_STACK_SIZE,
																	(void*)NULL,
																	(UBaseType_t) 1,
																	(StackType_t*)Task1Stack,
																	(TCB_t*)&Task1TCB);
	/*vListInsertEnd(&(pxReadyTasksLists[1]), &(((TCB_t*)(&Task1TCB))->xStateListItem));*/
																	
	Task2Handle = xTaskCreateStatic((TaskFunction_t)Task2_Entry,
																	(char*)"Task2",
																  (uint32_t)TASK2_STACK_SIZE,
																	(void*)NULL,
																	(UBaseType_t) 2,
																	(StackType_t*)Task2Stack,
																	(TCB_t*)&Task2TCB);
	/*vListInsertEnd(&(pxReadyTasksLists[2]), &(((TCB_t*)(&Task2TCB))->xStateListItem));*/
																	
	/* 開始調度 */
	vTaskStartScheduler();
																	
	for(;;){
		/* 若系統啟動成功則不會跑來這邊 */
	}
}

void delay(volatile uint32_t count){
	for(;count!=0;count--);
}

void Task1_Entry(void* arg){
	for(;;){
#if 0
		flag1 = 1;
		delay(1000);
		flag1 = 0;
		delay(1000);
		/* 自己呼叫taskYIELD，手動切換任務 */
		taskYIELD();
#else
		flag1 = 1;
		vTaskDelay(2);
		flag1 = 0;
		vTaskDelay(2);
#endif
		
		
	}
}
	
void Task2_Entry(void* arg){
	for(;;){
#if 0
		flag2 = 1;
		delay(1000);
		flag2 = 0;
		delay(1000);
		/* 自己呼叫taskYIELD，手動切換任務 */
		taskYIELD();
#else
		flag2 = 1;
		vTaskDelay(2);
		flag2 = 0;
		vTaskDelay(2);
#endif
	}
}


void vApplicationGetIdleTaskMemory(TCB_t **ppxIdleTaskTCBBuffer,
																	 StackType_t **ppxIdleTaskStackBuffer,
																	 uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack; // array 本身就是特殊指標 不需要再加&
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
