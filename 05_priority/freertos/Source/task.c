#include "FreeRTOS.h"
#include "task.h"

// 初始化 CurrentTCB
TCB_t* volatile pxCurrentTCB = NULL;
// 任務就緒列表
List_t pxReadyTasksLists[configMAX_PRIORITIES];

// 初始化 IdleTaskHandler
static TaskHandle_t xIdleTaskHandle = NULL;
// xTickCount全域變數宣告
TickType_t xTickCount;

// 初始化任務數
static UBaseType_t uxCurrentNumberOfTasks = 0;

static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
																 const char * const pcName,
																 const uint32_t ulStackDepth,
																 void * const pvParameters,
																 UBaseType_t uxPriority,
																 TaskHandle_t * const pxCreatedTask,
																 TCB_t *pxNewTCB );
																 

#if(configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
															 const char* const pcName,
															 const uint32_t ulStackDepth,
															 void* const pvParameters,
															 UBaseType_t uxPriority,
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
												 uxPriority,
												 &xReturn,
												 pxNewTCB);
		// 將任務放置ReadyList
		prvAddNewTaskToReadyList(pxNewTCB);
	}else{
		xReturn = NULL;
	}
	return xReturn;
}
#endif //configSUPPORT_STATIC_ALLOCATION

/* 定義排程方式 */
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;
#if (configUSE_PORT_OPTIMISED_TASK_SELECTION == 0)
	/* uxTopReadyPriority 存儲已就緒任務的最高優先級 */
	#define taskRECORD_READY_PROIRITY(uxPriority){\
	if((uxPriority) > uxTopReadyPriority){\
	uxTopReadyPriority = uxPriority;\
	}\
	}/* taskRECORD_READY_PROIRITY */
	
	#define taskSELECT_HIGHEST_PRIORITY_TASK(){\
	UBaseType_t uxTopPriority = uxTopReadyPriority;\
	while(listLIST_IS_EMPTY(&(pxReadyTasksLists[uxTopPriority]))){\
	--uxTopPriority;\
	}\
	listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[uxTopPriority]));\
	uxTopReadyPriority = uxTopPriority;\
	}
	
	/* 以下兩個在優化方法才有用 所以定義為空 */
	#define taskRESET_READY_PRIORITY(uxPriority)
	#define portRESET_READY_PRIORITY(uxPriority, uxTopReadyPriority)
	
	/* --------------- 以上是通用方法 --------------- */
#else
	/* --------------- 以下是優化方法 --------------- */
	#define taskRECORD_READY_PRIORITY(uxPriority) portRECORD_READY_PRIORITY(uxPriority, uxTopReadyPriority)
	#define taskSELECT_HIGHEST_PRIORITY_TASK(){\
	UBaseType_t uxTopPriority;\
	portGET_HIGHEST_PRIORITY(uxTopPriority, uxTopReadyPriority);\
	listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[uxTopPriority]));\
	}
#if 0
	#define taskRESET_READY_PRIORITY(uxPriority){\
	if(listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[uxPriority])) == (UBaseType_t)0){\
	portRESET_READY_PRIORITY((uxPriority), (uxTopReadyPriority));\
	}\
	}
#else
	#define taskRESET_READY_PRIORITY(uxPriority){\
	portRESET_READY_PRIORITY((uxPriority), (uxTopReadyPriority));\
	}
#endif
#endif

static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
										 const char* const pcName,
										 const uint32_t ulStackDepth,
										 void* const pvParameters,
										 UBaseType_t uxPriority,
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
	
	// 初始化何防呆	priority
	if(uxPriority >= (UBaseType_t)configMAX_PRIORITIES){
		uxPriority = (UBaseType_t)configMAX_PRIORITIES - (UBaseType_t)1U;
	}
	pxNewTCB->uxPriority = uxPriority;
	
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
		vListInitialize(&(pxReadyTasksLists[uxPriority]));
	}
}

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
extern TCB_t IdleTaskTCB;

/* 分配 IdleTask 內存 */
void vApplicationGetIdleTaskMemory(TCB_t **ppxIdleTaskTCBBuffer,
															StackType_t **ppxIdleTaskStackBuffer,
															uint32_t *pulIdleTaskStackSize);

/* 定義 idle task */
static portTASK_FUNCTION(prvIdleTask, pvParameter){
	(void) pvParameter;
	
	for(;;){
		/* idle task 目前什麼都不用做 */
	}
}

void vTaskStartScheduler(void){
	
	/* ==== 創建 IdleTask start ==== */
	TCB_t *pxIdleTaskTCBBuffer = NULL; //用於指向 IdleTask Stack 的 TCB
	StackType_t *pxIdleTaskStackBuffer = NULL; //用於指向 IdleTask Stack 的起始位置
	uint32_t ulIdleTaskStackSize;
	
	// 獲取 IdleTask 的儲存空間
	vApplicationGetIdleTaskMemory(&pxIdleTaskTCBBuffer,
																&pxIdleTaskStackBuffer,
																&ulIdleTaskStackSize);
	// 創建 IdleTask
	xIdleTaskHandle = xTaskCreateStatic((TaskFunction_t)prvIdleTask,
																			(char*)"IDLE",
																			(uint32_t)ulIdleTaskStackSize,
																			(void*)NULL,
																			(UBaseType_t) tskIDLE_PRIORITY,
																			(StackType_t*)pxIdleTaskStackBuffer,
																			(TCB_t*)pxIdleTaskTCBBuffer);
	vListInsertEnd(&(pxReadyTasksLists[0]), &(((TCB_t*)pxIdleTaskTCBBuffer)->xStateListItem));
	/* ==== 創建 IdleTask end ==== */
																			
	// pxCurrentTCB = &Task1TCB;
	if(xPortStartScheduler()!=pdFALSE){
		// 若調動啟動成功，即不會來到這裡
	}
}

// 一次替代兩行程式碼~
#define prvAddTaskToReadyList(pxTCB)\
				taskRECORD_READY_PRIORITY((pxTCB)->uxPriority);\
				vListInsertEnd(&(pxReadyTasksLists[(pxTCB)->uxPriority]), &((pxTCB)->xStateListItem));

// 將新建 task 放入 ReadyList
static void prvAddNewTaskToReadyList(TCB_t* pxNewTCB){
	taskENTER_CRITICAL();
	{
		uxCurrentNumberOfTasks++;
		
		if(pxCurrentTCB == NULL){
			pxCurrentTCB = pxNewTCB;
			
			if(uxCurrentNumberOfTasks == (UBaseType_t)1){
				prvInitialiseTaskLists();
			}
		}else{
			if(pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority){
				pxCurrentTCB = pxNewTCB;
			}
		}
		
		prvAddTaskToReadyList(pxNewTCB);
		
	}
	taskEXIT_CRITICAL();
}



#if 1
void vTaskSwitchContext(void){
	taskSELECT_HIGHEST_PRIORITY_TASK();
}
#else
void vTaskSwitchContext(void){
	if(pxCurrentTCB == &IdleTaskTCB){ // 若目前任務是idletask
		if(Task1TCB.xTicksToDelay == 0){
			pxCurrentTCB = &Task1TCB;
		}else if(Task2TCB.xTicksToDelay == 0){
			pxCurrentTCB = &Task2TCB;
		}else{
			return; /* 任務都仍再延遲，繼續執行空閒任務 */
		}
	}else{ //若目前任務非idletask
		/*
		目前沒有優先級的概念，所以只要另一個任務的delay結束，就馬上切換成他，若別人的delay還沒到，自己也進入delay，那就進入idletask
		*/
		if(pxCurrentTCB == &Task1TCB){
			if(Task2TCB.xTicksToDelay == 0){
				pxCurrentTCB = &Task2TCB;
			}else if(pxCurrentTCB->xTicksToDelay != 0){
				pxCurrentTCB = &IdleTaskTCB;
			}else{
				return;
			}
		}else if(pxCurrentTCB == &Task2TCB){
			if(Task1TCB.xTicksToDelay == 0){
				pxCurrentTCB = &Task1TCB;
			}else if(pxCurrentTCB->xTicksToDelay != 0){
				pxCurrentTCB = &IdleTaskTCB;
			}else{
				return;
			}
		}
	}
}
#endif

// delay
void vTaskDelay(const TickType_t xTicksToDelay){
	TCB_t* pxTCB = NULL;
	pxTCB = pxCurrentTCB;
	pxTCB->xTicksToDelay = xTicksToDelay;
	
	// 將任務從就緒列表中移除
	// uxListRemove(&(pxTCB->xStateListItem));
	taskRESET_READY_PRIORITY(pxTCB->uxPriority);
	
	/* 任務切換 */
	taskYIELD();
}

void xTaskIncrementTick(void){
	TCB_t *pxTCB = NULL;
	BaseType_t i = 0;
	
	// 更新系統計時器 xTickCount
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;
	
	// 掃描就緒列表中所有任務的xTickToDelay，如果不為0，則-1
	for(i=0; i<configMAX_PRIORITIES; i++){
        if( listLIST_IS_EMPTY( &(pxReadyTasksLists[i]) ) == pdFALSE ){
            pxTCB = (TCB_t*)(listGET_HEAD_ENTRY((&pxReadyTasksLists[i]))->pvOwner);
            if(pxTCB->xTicksToDelay > 0){
                pxTCB->xTicksToDelay--;
							
								if(pxTCB->xTicksToDelay == 0){  // 若沒有Delay 讓他進入排程
									taskRECORD_READY_PRIORITY(pxTCB->uxPriority);
								}
            }
        }
    }
	portYIELD();
}