#pragma once

#define UIMAX 13
#define DRAWDAMAGEMAX 1000

enum UINAME {
	Hp,				// �v���C���[��HP
	Shield,			// �v���C���[�̃V�[���h
	StatusBarBlack,	// �X�e�[�^�X�o�[�̍��w�i
	StatusBarFrame,	// �X�e�[�^�X�o�[�̃t���[��
	Energy,			// �d�̓Q�[�W
	EnergyFrame,	// �G�l���M�[�Q�[�W�̃t���[��
	Timelimit,		// �^�C�����~�b�g
	TimelimitBack,	// �^�C�����~�b�g�\���̔w�i
	Kill,			// �L����
	KillBack,		// �L�����\���̔w�i
	Combo,			// �R���{��
	HighScore,		// �n�C�X�R�A
	Score,			// ���݂̃X�R�A
};

struct UI {
	int sprite;		// �摜�ێ�
	float x, y;		// ���W
	float ox, oy;	// ���S�ݒ�
	float count;	// �b���⃌�x�����̃J�E���g
	bool isActive;
	float timer;
};

void UI_Initialize();
void UI_Finalize();
void UI_Update();
void UI_Render();
void UI_Restart();

void AddDamageUI(float x, float y, int dmg);