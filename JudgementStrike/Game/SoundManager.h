#pragma once

#define SOUNDMAX 13

enum SOUNDSCENE {
	/*BGM*/
	Title,				// �^�C�g��
	Game,				// �C���Q�[��
	Result,				// ���U���g
	/*SE*/
	GameOver,			// �Q�[���I�[�o�[
	SwordAttack,		// ���U��
	GunAttack,			// �e(�ʏ�)�U��
	GunChargeAttack,	// �e(�`���[�W)�U��
	PlayerDamage,		// �V�[���h�Ȃ��_���[�W
	ShieldDamage,		// �V�[���h����_���[�W
	ShieldBreak,		// �V�[���h�j��
	MoveCursor,			// �J�[�\���ړ�
	ButtonDecision,		// �{�^������
	ButtonCancel,		// �{�^���L�����Z��
};

enum SOUNDTYPE {
	BGM,
	SE,
};

struct VOLUME {
	int BGM;		// BGM�̉���
	int SE;		// SE�̉���
	int nowBGM;		// �\���p��BGM����
	int nowSE;		// �\���p��SE����
};

struct SOUNDMANAGER {
	int handle;		// �T�E���h�ۑ�
	bool loop;		// ���[�v�Đ����邩
	SOUNDTYPE type;	// BGM��SE��
};

const VOLUME* GetVolume();

void Sounds_Initialize();
void Sounds_Finalize();
void Sounds_Update();
void PlaySounds(SOUNDSCENE scene);
void StopSounds(SOUNDSCENE scene);
void SetSoundVolume(SOUNDTYPE type, int vol);