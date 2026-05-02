#ifndef LIST_H // 避免重複定義
#define LIST_H // 第一次進入時就會定義 (隨後可避免重複定義)

#include "portable.h"

struct xLIST_ITEM{
	TickType_t xItemValue; /* 排序用參數 */
	struct xLIST_ITEM* pxNext; /* 指向下一個list item */
	struct xLIST_ITEM* pxPrevious; /* 指向上一個list item */
	void* pvOwner;
	void* pvContainer;
};
typedef struct xLIST_ITEM ListItem_t;

/* list的結尾 也是list的頭 書中稱為生產者 */
struct xMINI_LIST_ITEM{
	TickType_t xItemValue;
	struct xLIST_ITEM* pxNext;
	struct xLIST_ITEM* pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;

typedef struct xLIST{
	UBaseType_t uxNumberOfItems; /* 此list的item數量 */
	ListItem_t* pxIndex; /* pointer索引，用來輪詢該鏈表 */
	MiniListItem_t xListEnd; /* 此list的結尾 */
}List_t;

/* -----------------節點帶參巨集函數------------------ */

/* 初始化節點擁有者 */
#define listSET_LIST_ITEM_OWNER(pxListItem, pxOwner) ((pxListItem)->pvOwner = (void*) (pxOwner))
/* 獲取節點擁有者 */
#define listGET_LIST_ITEM_OWNER(pxListItem) ((pxListItem)->pxOwner)
/* 初始化節點排序輔助值 */
#define listSET_LIST_ITEM_VALUE(pxListItem, xValue) ((pxListItem)->xItemValue = (xValue))
/* 獲取節點排序輔助值 */
#define listGET_LIST_ITEM_VALUE(pxListItem) ((pxListItem)->xItemValue)
/* 獲取第一個節點的排序輔助值 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxList) (((pxList)->xListEnd).pxNext->xItemValue)
/* 獲取第一個節點 */
#define listGET_HEAD_ENTRY(pxList) (((pxList)->xListEnd).pxNext)
/* 獲取該節點的下一個節點 */
#define listGET_NEXT(pxListItem) pxListItem->pxNext
/* 獲取此鏈表的最後一個節點 */
#define listGET_END_MARKER(pxList) ((ListItem_t const*)(&((pxList)xListEnd)))
/* 確認鏈表是否為空 */
#define listLIST_IS_EMPTY(pxList) ((BaseType_t)((pxList)->uxNumberOfItems == (UBaseType_t) 0))
/* 獲取鏈表節點數 */
#define listCURRENT_LIST_LENGTH(pxList) ((pxList)->uxNumberOfItems)
/* 獲取下一個節點的TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList){ \
	List_t* const pxConstList = (pxList); \
	(pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; \
	if((void*)(pxConstList)->pxIndex == (void*)&((pxConstList)->xListEnd)){ \
		(pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; \
	} \
	(pxTCB) = (pxConstList)->pxIndex->pvOwner; \
}
#define listGET_OWNER_OF_HEAD_ENTRY(pxList) ((&((pxList)->xListEnd))->pxNext)->pvOwner

/* prototypes 原型 */
void vListInitialize(List_t* const pxList);
void vListInitializeItem(ListItem_t* const pxItem);
void vListInsert(List_t* const pxList, ListItem_t* const pxItem);
void vListInsertEnd(List_t* const pxList, ListItem_t* const pxItem);
UBaseType_t uxListRemove(ListItem_t* const pxItemToRemove);

#endif /* LIST_H */