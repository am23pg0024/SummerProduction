#pragma once


// �����N���X�g�̃m�[�h
struct LinkedNode
{
	int hSprite;		// �摜�̃n���h��
	int x, y;			// ���W
	LinkedNode* pNext;	// ���̃m�[�h�ւ̃|�C���^�i�����N�j
	LinkedNode* pPrev;	// �O�̃m�[�h�ւ̃|�C���^�i�����N�j
};


// �m�[�h�̒ǉ�
void AddNode(LinkedNode* pNode);
// �m�[�h�̍폜
void RemoveNode(LinkedNode* pNode);
// �m�[�h�̍쐬
LinkedNode* CreateNode(const char* pFilename, int x, int y);
// �m�[�h�̑S�폜
void DeleteAll();

// �擪�m�[�h�̎擾
LinkedNode* GetTop();

