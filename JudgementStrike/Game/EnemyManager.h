#pragma once
#include "Game.h"
#include "Animation.h"

#define MAX_ENEMY 1000

// �G�̎��
enum ETYPE {
	ENormal,
	EHigh,
	EExplosion,
};

struct ENEMY {
	OBJECT obj;
	ANIMATOR animator;

	ETYPE type;		// ���
	int power;		// �U����
	int hitPower;	// �ڐG���̍U����
	int score;		// �X�R�A

	bool isBlinking;	// �_�Ńt���O
	bool isInvisible;	// ���Ńt���O
	float blinkTime;	// �_�Ŏ���

	HSV_PARAM hsv;	// �X�v���C�g��HSV
};

void EnemyManager_Initialize();
void EnemyManager_Finalize();
void EnemyManager_Update();
void EnemyManager_Render();
void EnemyManager_Restart();

ENEMY** GetEnemies();
int GetMaxSpawn();
FRECT GetEnemyRect(ENEMY* enemy);

void SubEnemyNum();
