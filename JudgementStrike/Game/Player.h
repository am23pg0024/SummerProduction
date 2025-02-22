#pragma once
#include "Game.h"
#include "Animation.h"

struct ENERGY_PARAM;

struct PLAYER {
	OBJECT obj;
	ANIMATOR animator;

	float speed2;	// `[WÌXs[h
	float speed3;	// `[We­ËÌXs[h
	int shield;		// V[hl

	bool isSword;		// íª©Ç¤©
	int swordPower;		// ÌUÍ
	float swordLength;	// Ì·³
	float swordAngle;	// ÌÍÍ
	int gunPower;		// eÌUÍ
	float gunScale;		// `[WeÌå«³
	int cSwordPower;	// `[WÌUÍ
	float cSwordLength;	// `[WÌ·³
	float cSwordAngle;	// `[WÌÍÍ
	int cGunPower;		// `[WeÌUÍ

	HSV_PARAM hsv;	// XvCgÌHSV
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
