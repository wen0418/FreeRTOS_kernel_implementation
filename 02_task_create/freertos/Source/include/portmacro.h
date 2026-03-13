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

#define portYIELD(){                                                                \
    /* 觸發 PendSV，產生上下文切換 */                                               \
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;                                 \
    __asm volatile ( "dsb" ::: "memory" );                                          \
    __asm volatile ( "isb" );                                                       \
}


#endif /* PORTMACRO_H */