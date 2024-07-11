//#include "UnlimitedLib/UnlimitedLib.h"
//#include "MemoryPool.h"
//#include "Animation.h"
//
//static ANIM_OBJ_POOL animObjPool;
//
//void InitAnimObjPool()
//{
//	animObjPool.head = animObjPool.objects[0];
//
//	for (int i = 0; i < POOL_SIZE - 1; i++)
//		animObjPool.objects[i]->state.next = animObjPool.objects[i + 1];
//
//	animObjPool.objects[POOL_SIZE - 1]->state.next = NULL;
//}
//
//void FinalIntMemory(ANIM_OBJ_POOL* memPool)
//{
//	free(memPool->pool);
//	memPool->emptyNum = memPool->poolSize;
//}
//
//void CreateAnimObj(ANIM_OBJECT* animObj, bool isActive)
//{
//	if (animObjPool.head == NULL) return;
//
//	ANIM_OBJECT* newAnimObj = animObjPool.head;
//	animObjPool.head = animObjPool.head->state.next;
//	newAnimObj->isActive = isActive;
//	newAnimObj = animObj;
//}
//
//void InitSpritePool(unsigned int size)
//{
//	spritePool.pool = InitAnimObjPool(&spritePool, size);
//}
//
//void FinalSpritePool()
//{
//	FinalIntMemory(&spritePool);
//}
//
//int* GetEmptySpritePool(unsigned int size)
//{
//	return CreateAnimObj(&spritePool, size);
//}
