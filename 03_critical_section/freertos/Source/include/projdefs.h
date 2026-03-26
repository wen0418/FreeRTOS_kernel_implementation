#ifndef PROJDEFS_H
#define PROJDEFS_H

typedef void (*TaskFunction_t)(void*); /* typedef的另一種用法 : 定義函數 */

#define pdFALSE 		((BaseType_t)0)
#define pdTRUE 			((BaseType_t)1)

#define pdPASS 			(pdTRUE)
#define pdFAIL 			(pdFALSE)

#endif /* PROJDEFS_H */