#pragma once
#include "Game.h"
#include "Animation.h"

#define MAX_ENEMY 1000

// 敵の種類
enum ETYPE {
	ENormal,
	EHigh,
	EExplosion,
};

struct ENEMY {
	OBJECT obj;
	ANIMATOR animator;

	ETYPE type;		// 種類
	int power;		// 攻撃力
	int hitPower;	// 接触時の攻撃力
	int score;		// スコア

	bool isBlinking;	// 点滅フラグ
	bool isInvisible;	// 明滅フラグ
	float blinkTime;	// 点滅時間

	HSV_PARAM hsv;	// スプライトのHSV
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
