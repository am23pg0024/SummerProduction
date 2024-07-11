#pragma once
#include "Game.h"
#include "Animation.h"

struct BULLET {
	OBJECT obj;
	ANIMATOR animator;

	float scale;	// �X�P�[��
	int power;		// �U����
};

void Bullet_Initialize();
void Bullet_Finalize();
void Bullet_Update();
void Bullet_Render();
void Bullet_Restart();

void Fire(float x, float y, float rad, bool isRight, STATE state);