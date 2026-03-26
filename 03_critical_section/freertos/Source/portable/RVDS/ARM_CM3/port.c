#include "FreeRTOS.h"
#include "task.h"
#include "ARMCM3.h"

#define portINITIAL_XPSR 							(0x01000000)
#define portSTART_ADDRESS_MASK				((StackType_t) 0xfffffffeUL)

#define portNVIC_SYSPRI2_REG					(*((volatile uint32_t*)0xe000ed20))
#define portNVIC_PENDSV_PRI 					(((uint32_t) configKERNEL_INTERRUPT_PRIORITY) << 16UL)
#define portNVIC_SYSTICK_PRI 					(((uint32_t) configKERNEL_INTERRUPT_PRIORITY) << 24UL)

// critical_section用參數
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;


// 組語內容函數宣告
void prvStartFirstTask(void) __attribute__((naked));
void vPortSVCHandler( void ) __attribute__( ( naked ) );
void xPortPendSVHandler( void ) __attribute__( ( naked ) );

static void prvTaskExitError(void){
	for(;;);
}

StackType_t *pxPortInitialiseStack(StackType_t* pxTopOfStack,
																	 TaskFunction_t pxCode,
																	 void* pvParameters)
{
	// 中斷或CPU特定事件發生時，自動加載到CPU暫存器的內容 (r15 -> r14 -> r12 -> "r3-r0")
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)pxCode) & portSTART_ADDRESS_MASK;
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t)prvTaskExitError;
	pxTopOfStack-=5;
	*pxTopOfStack = (StackType_t) pvParameters;
	// 所以以上跳過(或做初始設定)
	
	// 中斷或CPU特定事件發生時，手動加載到CPU暫存器的內容 (r11-r4)
	pxTopOfStack-=8; // 所以跳過
	
	return pxTopOfStack;
}

BaseType_t xPortStartScheduler(void){
	// 配置pendSV和SYSTICK的中斷優先級為最低
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	prvStartFirstTask();
	
	return 0;
}

__attribute__( ( naked ) ) void prvStartFirstTask( void ){
    __asm volatile (
        " ldr r0, =0xE000ED08 \n" /* 獲取 VTOR 暫存器位址 */
        " ldr r0, [r0]        \n" /* 獲取向量表起始位址 */
        " ldr r0, [r0]        \n" /* 獲取 MSP 初始值 */
        " msr msp, r0         \n" /* 設置主堆疊指標 MSP */
        " cpsie i             \n" /* 使能全局中斷 */
        " cpsie f             \n" /* 使能 HardFault 中斷 */
        " dsb                 \n" /* 資料同步屏障 */
        " isb                 \n" /* 指令同步屏障 */
        " svc 0               \n" /* 呼叫 SVC 啟動第一個任務 */
        " nop                 \n"
        " nop                 \n"
    );
}

void vPortSVCHandler( void )
{
    __asm volatile (
        " ldr r3, =pxCurrentTCB   \n" /* 載入 pxCurrentTCB 的位址到 r3 */
        " ldr r1, [r3]            \n" /* 載入 pxCurrentTCB 到 r1 */
        " ldr r0, [r1]            \n" /* r0 = 第一個任務堆疊的棧頂 */
        " ldmia r0!, {r4-r11}     \n" /* 出棧：將棧內容載入 r4~r11，r0 遞增 */
        " msr psp, r0             \n" /* 更新任務棧指標到 psp */
        " isb                     \n"
        " mov r0, #0              \n"
        " msr basepri, r0         \n" /* 打開所有中斷屏蔽 */
        " orr r14, #0xd           \n" /* 準備退出異常，返回 Thumb 狀態並使用 PSP */
        " bx r14                  \n" /* 異常返回，硬體自動出棧剩下的暫存器 */
    );
}

void xPortPendSVHandler( void )
{
    __asm volatile (
        " mrs r0, psp             \n" /* 獲取當前任務棧指標到 r0 */
        " isb                     \n"
        " ldr r3, =pxCurrentTCB   \n" 
        " ldr r2, [r3]            \n"
        " stmdb r0!, {r4-r11}     \n" /* 儲存 r4~r11 到當前任務棧 */
        " str r0, [r2]            \n" /* 更新新的棧頂指標到 TCB */
        " stmdb sp!, {r3, r14}    \n" /* 壓棧保護 r3 和 r14 (準備呼叫 C 函數) */
        " mov r0, %0              \n" /* 進入臨界段 (使用下面傳入的巨集值) */
        " msr basepri, r0         \n"
        " dsb                     \n"
        " isb                     \n"
        " bl vTaskSwitchContext   \n" /* 呼叫 C 函數：尋找下一個要執行的任務 */
        " mov r0, #0              \n" 
        " msr basepri, r0         \n" /* 退出臨界段 */
        " ldmia sp!, {r3, r14}    \n" /* 恢復 r3 和 r14 */
        " ldr r1, [r3]            \n" 
        " ldr r0, [r1]            \n" /* 獲取新任務的棧頂 */
        " ldmia r0!, {r4-r11}     \n" /* 新任務出棧 r4~r11 */
        " msr psp, r0             \n" /* 更新 PSP 到新任務棧頂 */
        " isb                     \n"
        " bx r14                  \n" /* 異常返回，執行新任務 */
        " nop                     \n"
        :: "i"(configMAX_SYSCALL_INTERRUPT_PRIORITY) /* 將 C 巨集安全地傳給組合語言 */
    );
}

// 定義進入critical_section
void vPortEnterCritical(void){
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	
	if(uxCriticalNesting == 1){
		// configASSERT((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK)) == 0;
	}
}

// 定義退出critical_section
void vPortExitCritical(void){
	// configASSERT(uxCriticalNesting);
	uxCriticalNesting--;
	
	if(uxCriticalNesting == 0){
		portENABLE_INTERRUPTS();
	}
}