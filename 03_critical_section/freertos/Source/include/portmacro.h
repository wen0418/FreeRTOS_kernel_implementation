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
		/* %0 是一個「佔位符」，編譯器會自動把它替換成下面傳入的通用暫存器 */
		"msr basepri, %0 \n"    /* 將 %0 裡的值寫入 basepri 暫存器，用來關閉中斷 */
		"dsb             \n"    /* Data Synchronization Barrier: 確保這行以前的記憶體存取都確實做完 */
		"isb             \n"    /* Instruction Synchronization Barrier: 清空管線，確保下一行指令能立刻看到 basepri 更新後的效果 */
		
		/* --- 下面是 C 語言與組合語言的溝通橋樑 --- */
		:                       /* 這裡冒號後面是「輸出參數」：因為這函數不用回傳值，所以留空 */
		: "r" (ulNewBASEPRI)    /* 這裡冒號後面是「輸入參數」："r" 代表請編譯器隨便挑一個通用暫存器 (如 r0)，把 ulNewBASEPRI 放進去，並對應給上面的 %0 */
		: "memory"              /* 這裡告訴編譯器：「這段組語可能會改變記憶體或系統狀態，請不要隨便做最佳化」 */
	);
}

// 有return
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI(void){
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	
	__asm volatile (
		/* 注意：讀取特殊暫存器必須用 mrs，寫入才用 msr */
		"mrs %0, basepri \n"    /* 第一步：把當前 basepri 的狀態讀出來，存到 %0 (這會對應到下面的 ulReturn) */
		"msr basepri, %1 \n"    /* 第二步：把新的屏蔽值 %1 (對應下面的 ulNewBASEPRI) 寫入 basepri，關閉中斷 */
		"dsb             \n"    
		"isb             \n"    
		
		/* --- 下面是 C 語言與組合語言的溝通橋樑 --- */
		: "=r" (ulReturn)       /* 輸出參數："=r" 的 "=" 代表這個變數是被寫出的結果。它對應第一個佔位符 %0 */
		: "r" (ulNewBASEPRI)    /* 輸入參數："r" 代表把 C 變數 ulNewBASEPRI 放入暫存器，對應第二個佔位符 %1 */
		: "memory"
	);
	
	return ulReturn; /* 把剛才備份的原始狀態回傳給 C 語言的呼叫者 */
}

// 定義中斷相關函數(開中斷)
// 不帶中斷保護的開中斷定義
#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
// 帶中斷保護的開中斷定義 (將上一次中斷保存的BASEPRI傳進來)
#define portCLEAR_INTERRUPTS_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI){
	__asm volatile (
		"msr basepri, %0 \n"    /* 將傳入的 %0 (也就是 ulBASEPRI 的值) 寫回 basepri。如果是 0 就是打開所有中斷 */
		/* 開中斷通常不需要 dsb/isb，因為就算晚幾個 CPU 週期才真正發生中斷也沒關係，不像關中斷那樣十萬火急 */
		
		/* --- 下面是 C 語言與組合語言的溝通橋樑 --- */
		:                       /* 沒有輸出參數 */
		: "r" (ulBASEPRI)       /* 輸入參數：讓編譯器把 ulBASEPRI 放進暫存器，對應 %0 */
		: "memory"
	);
}

// 定義進出critical_section
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()

#endif /* PORTMACRO_H */