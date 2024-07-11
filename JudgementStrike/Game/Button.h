#pragma once

// ���j���[���ڑI��p
enum MENU_ITEM {
	MStart,
	MResume,
	MRestart,
	MOption,
	MBGM,
	MSE,
	MExit,
	MYes,
	MNo,
	MArrowL,
	MArrowR,
};

// �{�^���\����
struct BUTTON {
	int sprite;				// �{�^���̃X�v���C�gID
	float x, y;				// �{�^������̍��W
	float width, height;	// �{�^���̕��ƍ���
	bool isActive;	// �A�N�e�B�u�ȃ{�^�����ǂ����i�X�e�[�W�Z���N�g�Ŏg���邩���H�j
};

BUTTON CreateButton(int sprite, float x, float y, float width, float height, bool isActive);
void SetButtonActive(BUTTON& button, bool isActive);

// �}�E�X�J�[�\�����{�^���ɏ���Ă��邩�𔻒�
bool IsMouseOnButton(BUTTON& button);

void RenderButton(BUTTON& button);
