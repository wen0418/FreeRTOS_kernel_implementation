#include "FreeRTOS.h"
#include "task.h"

TCB_t* volatile pxCurrentTCB = NULL;
List_t pxReadyTaskLists[configMAX_PRIORITIES];

static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
																 const char * const pcName,
																 const uint32_t ulStackDepth,
																 void * const pvParameters,
																 TaskHandle_t * const pxCreatedTask,
																 TCB_t *pxNewTCB );
																 

#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
															 const char* const pcName,
															 const uint32_t ulStackDepth,
															 void* const pvParameters,
															 StackType_t* const puxStackBuffer,
															 TCB_t* const pxTaskBuffer)
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;
	
	if((pxTaskBuffer != NULL) && (puxStackBuffer != NULL)){
		pxNewTCB = (TCB_t*)pxTaskBuffer;
		pxNewTCB->pxStack = (StackType_t*) puxStackBuffer;
		
		prvInitialiseNewTask(pxTaskCode,
												 pcName,
												 ulStackDepth,
												 pvParameters,
												 &xReturn,
												 pxNewTCB);
	}else{
		xReturn = NULL;
	}
	return xReturn;
}
#endif //configSUPPORT_STATIC_ALLOCATION

static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
										 const char* const pcName,
										 const uint32_t ulStackDepth,
										 void* const pvParameters,
										 TaskHandle_t* const pxCreateTask,
										 TCB_t* pxNewTCB)
{
	StackType_t *pxTopOfStack;
	UBaseType_t x;
											 
	// 獲取stack頂位置
	pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (uint32_t)1);
	// 向下做8字節對齊 (讓它是8的倍數)
	pxTopOfStack = (StackType_t*)(((uint32_t)pxTopOfStack) & (~((uint32_t)0x0007)));
	
	// 放入姓名
	for(x=(UBaseType_t)0;x<(UBaseType_t)configMAX_TASK_NAME_LEN;x++){
		pxNewTCB->pcTaskName[x] = pcName[x];
		if(pcName[x] == 0x00){
			break;
		}
	}
	// pcTaskName防呆
	pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';
	
	// 初始化listitem節點(container為NULL)
	vListInitializeItem(&(pxNewTCB->xStateListItem));
	// 設置此任務節點的擁有者
	listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);
	
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack,
																								pxTaskCode,
																								pvParameters);
	
	// 讓xReturn帶東西會去交代 (Hint：TaskHandle_t是一個void*，跟xReturn一樣)
	if((void*)pxCreateTask != NULL){
		*pxCreateTask = (TaskHandle_t)pxNewTCB;
	}
}

void prvInitialiseTaskLists(void){
	UBaseType_t uxPriority;
	
	for(uxPriority = (UBaseType_t)0U;uxPriority < (UBaseType_t)configMAX_PRIORITIES;uxPriority++){
		vListInitialize(&pxReadyTaskLists[uxPriority]);
	}
}

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
void vTaskStartScheduler(void){
	pxCurrentTCB = &Task1TCB;
	if(xPortStartScheduler()!=pdFALSE){
		// 若調動啟動成功，即不會來到這裡
	}
}

void vTaskSwitchContext(void){
	if(pxCurrentTCB == &Task1TCB){
		pxCurrentTCB = &Task2TCB;
	}else{
		pxCurrentTCB = &Task1TCB;
	}
}