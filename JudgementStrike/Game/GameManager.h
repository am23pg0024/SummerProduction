#pragma once

struct ENEMY;

// �Q�[���}�l�[�W���[
struct GAME_MANAGER {
	int wave;			// ���݂̃E�F�[�u
	int spawnNum;		// ��x�ɃX�|�[������G�̐�
	int waveBorder;		// ���̃E�F�[�u�܂łɕK�v�ȃL����
	float highRate;		// High Enemy���X�|�[������m��
	float explRate;		// Explosion Enemy���X�|�[������m��

	int energyLevel;	// �d�̓��x��
	int energyGauge;	// �d�̓Q�[�W
	int maxGauge;		// �d�̓��x���A�b�v�ɕK�v�ȃQ�[�W��

	int killCnt;		// �G�̓�����
	float timeLimit;	// ��������(�b)

	int comboCnt;		// �R���{��
	int maxCombo;		// �ő�R���{��
	float comboTimer;	// �R���{�𑝂₷���߂��������߂鎞��

	unsigned int score;		// �X�R�A(�ŏ��l 0)
	unsigned int highScore;	// �n�C�X�R�A(�ŏ��l 0)
	double scoreRate;		// �R���{���ɂ��X�R�A�{��
};

void GameManager_Initialize();
void GameManager_Finalize();
void GameManager_Update();

const GAME_MANAGER* GetGameManager();

void AddEnergy();
void AddEnergy(int num);
void AddKillCount();
void AddKillCount(int cnt);
void SubTimeLimit(float num);
void AddComboCount();
void AddComboCount(int cnt);
void AddScore(ENEMY* enemy);
