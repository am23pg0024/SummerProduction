#include "LinkedList.h"


LinkedNode* g_pHead = NULL;	// 先頭ノード
LinkedNode* g_pTail = NULL;	// 末尾ノード

// ノードの追加
void AddNode(LinkedNode* pNode)
{
	// 先頭ノードがない場合は先頭/末尾のノードとなる
	if (g_pHead == NULL) {
		g_pHead = g_pTail = pNode;
		return;
	}
	// 現在の末尾の次のノードとして登録する
	g_pTail->pNext = pNode;
	// 現在の末尾を追加したノードの手前のノードとして登録する
	pNode->pPrev = g_pTail;

	// 追加されたノードを末尾とする
	g_pTail = pNode;
}

// ノードの削除
void RemoveNode(LinkedNode* pNode)
{
	// ①手前にノードがある場合は次のノードとリンクさせる
	if (pNode->pPrev != NULL) pNode->pPrev->pNext = pNode->pNext;
	// ②次のノードがある場合は手前のノードとリンクさせる
	if (pNode->pNext != NULL) pNode->pNext->pPrev = pNode->pPrev;
	// ③先頭ノードが削除される場合は先頭ノードを差し替える
	if (g_pHead == pNode) g_pHead = pNode->pNext;
	// ④末尾ノードが削除される場合は手前のノードに差し替える
	if (g_pTail == pNode) g_pTail = pNode->pPrev;

	// mallocで確保したメモリを解放する
	free(pNode);
}

// ノードの作成
LinkedNode* CreateNode(const char* pFilename, int x, int y)
{
	// ノードのメモリを確保する
	LinkedNode* pNode = (LinkedNode*)malloc(sizeof(LinkedNode));

	// ノードの値を設定して初期化
	pNode->hSprite = CreateSprite(pFilename);
	SetSpriteSize(pNode->hSprite, 64, 64);
	pNode->x = x;
	pNode->y = y;

	pNode->pNext = NULL;
	pNode->pPrev = NULL;

	return pNode;
}

// ノードの全削除
void DeleteAll()
{
	// ノードがなくなるまで削除し続ける
	while (g_pHead != NULL) {
		RemoveNode(g_pHead);
	}
}

// 先頭ノードの取得
LinkedNode* GetTop()
{
	return g_pHead;
}
