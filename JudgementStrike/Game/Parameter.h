#pragma once

// �G�֘A�̃p�����[�^���Ǘ�����\����
struct ENEMY_PARAM {
	int hp;
	float speed;
	int power;
	int hitPower;
};

// �d�̓V�X�e���֘A�̃p�����[�^���Ǘ�����\����
struct ENERGY_PARAM {
	int maxGauge;	// �d�̓��x���A�b�v�ɕK�v�ȃQ�[�W��

	int swordPower;		// ���̓d�̓��x���̌��̍U����
	float swordLength;	// ���̓d�̓��x���̌��̒���
	float swordAngle;	// ���̓d�̓��x���̌��͈̔�
	int gunPower;		// ���̓d�̓��x���̏e�̍U����
	float gunScale;		// ���̓d�̓��x���̃`���[�W�e�̃X�P�[��
	int cGunPower;		// ���̓d�̓��x���̃`���[�W�e�̍U����
};

// �E�F�[�u�֘A�̃p�����[�^���Ǘ�����\����
struct WAVE_PARAM {
	int spawnNum;		// ��x�ɃX�|�[������G�̐�
	int border;			// ���̃E�F�[�u�܂łɕK�v�ȃL����
	float highRate;		// Explosion Enemy���X�|�[������m��
	float explRate;		// Explosion Enemy���X�|�[������m��
};

struct PARAM {
	float timeLimit;	// ��������

	// �v���C���[
	struct {
		float speed;
		float speed2;
		float speed3;
		float CHARGE_CHECK_TIME;
		float CHARGING_TIME;
		float MAX_INVINCIBLE_TIME;
		float MAX_COOL_TIME;
		float AVOID_DIST;
	} player;

	// �e
	struct {
		float speed;
	} bullet;

	// �V�[���h��
	struct {
		float startTime;	// �񕜂��n�܂�܂ł̎���
		float interval;		// �񕜂���C���^�[�o��
		int num;			// ��x�ɉ񕜂���V�[���h��
	} shieldRecovery;

	// �d�̓V�X�e��
	ENERGY_PARAM energyParams[5];

	// �G
	struct {
		ENEMY_PARAM params[3];
		int maxSpawn;
		float maxBlinkTime;
	} enemy;

	// ����
	struct {
		int power;
		float scale;
	} explosion;

	// �E�F�[�u
	struct {
		WAVE_PARAM params[4];
		float spawnTime;
	} wave;
	
};

const PARAM* GetParameter();

void SaveParam();
void LoadParam();
