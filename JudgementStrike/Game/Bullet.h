#pragma once
#include "Game.h"
#include "Animation.h"

struct BULLET {
	OBJECT obj;
	ANIMATOR animator;

	float scale;	// ÉXÉPÅ[Éã
	int power;		// çUåÇóÕ
};

void Bullet_Initialize();
void Bullet_Finalize();
void Bullet_Update();
void Bullet_Render();
void Bullet_Restart();

void Fire(float x, float y, float rad, bool isRight, STATE state);