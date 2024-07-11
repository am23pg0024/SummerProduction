#include "LinkedList.h"


LinkedNode* g_pHead = NULL;	// �擪�m�[�h
LinkedNode* g_pTail = NULL;	// �����m�[�h

// �m�[�h�̒ǉ�
void AddNode(LinkedNode* pNode)
{
	// �擪�m�[�h���Ȃ��ꍇ�͐擪/�����̃m�[�h�ƂȂ�
	if (g_pHead == NULL) {
		g_pHead = g_pTail = pNode;
		return;
	}
	// ���݂̖����̎��̃m�[�h�Ƃ��ēo�^����
	g_pTail->pNext = pNode;
	// ���݂̖�����ǉ������m�[�h�̎�O�̃m�[�h�Ƃ��ēo�^����
	pNode->pPrev = g_pTail;

	// �ǉ����ꂽ�m�[�h�𖖔��Ƃ���
	g_pTail = pNode;
}

// �m�[�h�̍폜
void RemoveNode(LinkedNode* pNode)
{
	// �@��O�Ƀm�[�h������ꍇ�͎��̃m�[�h�ƃ����N������
	if (pNode->pPrev != NULL) pNode->pPrev->pNext = pNode->pNext;
	// �A���̃m�[�h������ꍇ�͎�O�̃m�[�h�ƃ����N������
	if (pNode->pNext != NULL) pNode->pNext->pPrev = pNode->pPrev;
	// �B�擪�m�[�h���폜�����ꍇ�͐擪�m�[�h�������ւ���
	if (g_pHead == pNode) g_pHead = pNode->pNext;
	// �C�����m�[�h���폜�����ꍇ�͎�O�̃m�[�h�ɍ����ւ���
	if (g_pTail == pNode) g_pTail = pNode->pPrev;

	// malloc�Ŋm�ۂ������������������
	free(pNode);
}

// �m�[�h�̍쐬
LinkedNode* CreateNode(const char* pFilename, int x, int y)
{
	// �m�[�h�̃��������m�ۂ���
	LinkedNode* pNode = (LinkedNode*)malloc(sizeof(LinkedNode));

	// �m�[�h�̒l��ݒ肵�ď�����
	pNode->hSprite = CreateSprite(pFilename);
	SetSpriteSize(pNode->hSprite, 64, 64);
	pNode->x = x;
	pNode->y = y;

	pNode->pNext = NULL;
	pNode->pPrev = NULL;

	return pNode;
}

// �m�[�h�̑S�폜
void DeleteAll()
{
	// �m�[�h���Ȃ��Ȃ�܂ō폜��������
	while (g_pHead != NULL) {
		RemoveNode(g_pHead);
	}
}

// �擪�m�[�h�̎擾
LinkedNode* GetTop()
{
	return g_pHead;
}
