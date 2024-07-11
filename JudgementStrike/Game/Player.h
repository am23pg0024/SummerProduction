#pragma once
#include "Game.h"
#include "Animation.h"

struct ENERGY_PARAM;

struct PLAYER {
	OBJECT obj;
	ANIMATOR animator;

	float speed2;	// �`���[�W���̃X�s�[�h
	float speed3;	// �`���[�W�e���ˎ��̃X�s�[�h
	int shield;		// �V�[���h�l

	bool isSword;		// ���킪�����ǂ���
	int swordPower;		// ���̍U����
	float swordLength;	// ���̒���
	float swordAngle;	// ���͈̔�
	int gunPower;		// �e�̍U����
	float gunScale;		// �`���[�W�e�̑傫��
	int cSwordPower;	// �`���[�W���̍U����
	float cSwordLength;	// �`���[�W���̒���
	float cSwordAngle;	// �`���[�W���͈̔�
	int cGunPower;		// �`���[�W�e�̍U����

	HSV_PARAM hsv;	// �X�v���C�g��HSV
};

void Player_Initialize();
void Player_Finalize();
void Player_Update();
void Player_Render();
void Player_Restart();

PLAYER* GetPlayer();
FRECT GetPlayerRect();
void PlayerSetInvincible(float time);
bool PlayerIsInvincible();

void Enhance(ENERGY_PARAM* energyParam);
void Damage2Player(PLAYER* player, int damage);
