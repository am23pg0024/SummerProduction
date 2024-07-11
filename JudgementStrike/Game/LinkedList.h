#pragma once


// リンクリストのノード
struct LinkedNode
{
	int hSprite;		// 画像のハンドル
	int x, y;			// 座標
	LinkedNode* pNext;	// 次のノードへのポインタ（リンク）
	LinkedNode* pPrev;	// 前のノードへのポインタ（リンク）
};


// ノードの追加
void AddNode(LinkedNode* pNode);
// ノードの削除
void RemoveNode(LinkedNode* pNode);
// ノードの作成
LinkedNode* CreateNode(const char* pFilename, int x, int y);
// ノードの全削除
void DeleteAll();

// 先頭ノードの取得
LinkedNode* GetTop();

