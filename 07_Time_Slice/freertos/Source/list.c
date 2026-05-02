#include "FreeRTOS.h"
#include "list.h"

/* list item init */
void vListInitializeItem(ListItem_t* const pxItem){
	pxItem->pvContainer = NULL;
}

/* list root init */
void vListInitialize(List_t* const pxList){
	pxList->pxIndex = (ListItem_t*) &(pxList->xListEnd);
	pxList->xListEnd.xItemValue = portMAX_DELAY;
	pxList->xListEnd.pxNext = (ListItem_t*) &(pxList->xListEnd);
	pxList->xListEnd.pxPrevious = (ListItem_t*) &(pxList->xListEnd);
	pxList->uxNumberOfItems = (UBaseType_t) 0U;
}

void vListInsertEnd(List_t* const pxList, ListItem_t* const pxNewListItem){
	ListItem_t* const pxIndex = pxList->pxIndex;
	
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;
	
	pxNewListItem->pvContainer = (void*) pxList;
	
	(pxList->uxNumberOfItems)++;
}

void vListInsert(List_t* const pxList, ListItem_t* const pxNewListItem){
	ListItem_t* pxIterator;
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
	if(xValueOfInsertion == portMAX_DELAY){
		pxIterator = pxList->xListEnd.pxPrevious;
	}else{
		for(pxIterator = (ListItem_t*) &(pxList->xListEnd); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext){
			/* 單純尋找可插入節點 */
		}
	}
	/* 插入list當中 */
	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;
	
	/* 確認容器主人 */
	pxNewListItem->pvContainer = (void*) pxList;
	
	/* 更新list節點數量 */
	(pxList->uxNumberOfItems)++;
}

UBaseType_t uxListRemove(ListItem_t* const pxItemToRemove){
	/* 獲取節點所在的鏈表 */
	List_t* const pxList = (List_t*) pxItemToRemove->pvContainer;
	/* 將節點從鏈表刪除 */
	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	/* 如果要刪除的節點是所在鏈表的index，那需要調整index */
	if(pxList->pxIndex == pxItemToRemove){
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}
	
	/* 將此節點所在之鏈表改為空，代表他已經不屬於任何鏈表 */
	pxItemToRemove->pvContainer = NULL;
	
	/* 更變鏈表節點數量 */
	(pxList->uxNumberOfItems)--;
	
	/* 返回鏈表中剩餘的節點數 */
	return pxList->uxNumberOfItems;
}