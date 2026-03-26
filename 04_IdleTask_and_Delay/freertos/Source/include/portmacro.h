#ifndef PORTMACRO_H // avoid repeat define
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"

/* redefine data type */
#define portCHAR				char
#define portFLOAT   		float
#define portDOUBLE  		double
#define portLONG    		long
#define portSHORT   		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE   long
	
typedef portSTACK_TYPE 	StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL  // UL是Unsigned long的意思
#endif

/* 中断控制状态寄存器：0xe000ed04
 * Bit 28 PENDSVSET: PendSV 悬起位
 */
#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )

#define portSY_FULL_READ_WRITE		( 15 )

// critical用define
#define portINLINE __inline
#ifndef portFORCE_INLINE
#define portFORCE_INLINE __attribute__((always_inline)) inline
#endif

#define portYIELD(){                                                                \
    /* 觸發 PendSV，產生上下文切換 */                                               \
  portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;                                 \
	__asm volatile ( "dsb" ::: "memory" );                                          \
	__asm volatile ( "isb" );                                                       \
}

// 定義中斷相關函數(關中斷)
// 無return
#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()
static portFORCE_INLINE void vPortRaiseBASEPRI(void){
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm volatile (
		"msr basepri, %0 \n"    // %0 代表第一個輸入參數
		"dsb             \n"
		"isb             \n"
        :                       // 沒有輸出參數
        : "r" (ulNewBASEPRI)    // 輸入參數：將 ulNewBASEPRI 放入通用暫存器("r")
        : "memory"              // 告訴編譯器記憶體可能被修改
	);
}

// 有return
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI(void){
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm volatile (
		"mrs %0, basepri \n"    // 修正：讀取特殊暫存器要用 mrs。%0 代表 ulReturn
		"msr basepri, %1 \n"    // %1 代表 ulNewBASEPRI
		"dsb             \n"
		"isb             \n"
        : "=r" (ulReturn)       // 輸出參數："=r" 表示將結果寫入 ulReturn
        : "r" (ulNewBASEPRI)    // 輸入參數：將 ulNewBASEPRI 傳入
        : "memory"
	);
	return ulReturn;
}

// 定義中斷相關函數(開中斷)
// 不帶中斷保護的開中斷定義
#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
// 帶中斷保護的開中斷定義 (將上一次中斷保存的BASEPRI傳進來)
#define portCLEAR_INTERRUPTS_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI){
	__asm volatile (
		"msr basepri, %0"
        :                       // 沒有輸出參數
        : "r" (ulBASEPRI)       // 輸入參數+
        : "memory"
	);
}

// 定義進出critical_section
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()

// IdleTask 移植性定義函數
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void *pvParameters)

#endif /* PORTMACRO_H */