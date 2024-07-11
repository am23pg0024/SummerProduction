#pragma once
#include "Game.h"
#include "Animation.h"

struct EXPLOSION {
	OBJECT obj;
	ANIMATOR animator;

	int power;	// çUåÇóÕ
};

void Explosion_Initialize();
void Explosion_Finalize();
void Explosion_Update();
void Explosion_Render();
void Explosion_Restart();

EXPLOSION* GetExplosions();

void CreateExplosion(float x, float y);
