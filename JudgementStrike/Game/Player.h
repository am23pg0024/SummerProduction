#pragma once
#include "Game.h"
#include "Animation.h"

struct ENERGY_PARAM;

struct PLAYER {
	OBJECT obj;
	ANIMATOR animator;

	float speed2;	// チャージ中のスピード
	float speed3;	// チャージ弾発射時のスピード
	int shield;		// シールド値

	bool isSword;		// 武器が剣かどうか
	int swordPower;		// 剣の攻撃力
	float swordLength;	// 剣の長さ
	float swordAngle;	// 剣の範囲
	int gunPower;		// 銃の攻撃力
	float gunScale;		// チャージ弾の大きさ
	int cSwordPower;	// チャージ剣の攻撃力
	float cSwordLength;	// チャージ剣の長さ
	float cSwordAngle;	// チャージ剣の範囲
	int cGunPower;		// チャージ弾の攻撃力

	HSV_PARAM hsv;	// スプライトのHSV
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
