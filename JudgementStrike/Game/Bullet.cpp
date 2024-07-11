#include "UnlimitedLib/UnlimitedLib.h"
#include "Bullet.h"
#include "MemoryPool.h"
#include "2DHelperEx.h"
#include "math.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "HitCheck.h"
#include "Stage.h"
#include "GameManager.h"
#include "Player.h"
#include "Parameter.h"
#include "SoundManager.h"
#include "UI.h"

#define BULLET_MAX 1000

static CIRCLE b_circle;

BULLET bullet[BULLET_MAX];

ANIM_PARAM bAnimParam[] = {
	{ NULL, 1, 10, true},		// �ʏ�e
	{ NULL, 3, 10, true},		// �`���[�W�e
};

static int* bSpritePool;	// �A�j���[�V�����X�v���C�g�v�[��


void DamageBullet(OBJECT* obj, BULLET* bullet);

void Bullet_Restart()
{
	// ������
	for (int i = 0; i < BULLET_MAX; i++)
	{
		OBJECT* obj = &bullet[i].obj;
		obj->x = obj->y = 0;
		obj->vx = obj->vy = 0;
		obj->speed = GetParameter()->bullet.speed;
		obj->angle = 0;
		obj->isActive = false;
		obj->state = SAttack;

		ANIMATOR* animator = &bullet[i].animator;
		animator->animID = 0;
		animator->animCnt = 0;

		bullet[i].scale = 1.f;
		bullet[i].power = 0;
	}
}

void Bullet_Initialize()
{	
	// �������m�ہ�����
	if (bSpritePool == NULL) {
		int size = 0;
		for (int i = 0; i < 2; i++)
			size += bAnimParam[i].total;

		// �������m�ہ��[��������
		bSpritePool = (int*)malloc(sizeof(int) * size);
		ZeroMemory(bSpritePool, sizeof(int) * size);

		// ����������
		for (int i = 0; i < 2; i++) {
			if (i == 0)
				bAnimParam[i].sprites = bSpritePool;
			else
				bAnimParam[i].sprites = bAnimParam[i - 1].sprites + bAnimParam[i - 1].total;
		}
	}

	CreateSpriteMatrix("Assets/Sprites/Player/smallprojectile1.png", 16, 16, 1, 1, 1, bAnimParam[0].sprites);			// �ʏ�e
	CreateSpriteMatrix("Assets/Sprites/Player/bigprojectile-spritesheet.png", 64, 64, 3, 1, 3, bAnimParam[1].sprites);	// �`���[�W�e

	SetSpriteOrigin(bAnimParam[0].sprites[0], 8, 8);
	for (int i = 0; i < 3; i++)
	{
		SetSpriteOrigin(bAnimParam[1].sprites[i], 32, 32);
	}

	// ������
	for (int i = 0; i < BULLET_MAX; i++)
	{
		OBJECT* obj = &bullet[i].obj;
		obj->x = obj->y = 0;
		obj->vx = obj->vy = 0;
		obj->speed = GetParameter()->bullet.speed;
		obj->angle = 0;
		obj->isActive = false;
		obj->state = SAttack;

		ANIMATOR* animator = &bullet[i].animator;
		animator->owner = obj;
		animator->params = bAnimParam;
		animator->animID = 0;
		animator->animCnt = 0;

		bullet[i].scale = 1.f;
		bullet[i].power = 0;
	}
}
void Bullet_Finalize()
{
	for (int i = 0; i < 2; i++)
	{
		ReleaseSpriteMatrix(bAnimParam[i].sprites, bAnimParam[i].total);
	}

	// ���������
	free(bSpritePool);
	bSpritePool = NULL;
}

void Bullet_Update(BULLET* bullet)
{
	OBJECT* obj = &bullet->obj;

	// �ړ�����
	obj->x += obj->vx * DELTA_TIME;
	obj->y += obj->vy * DELTA_TIME;

	/*�G�Ƃ̓����蔻��*/
	for (int i = 0; i < GetMaxSpawn(); i++) {
		ENEMY* enemy = GetEnemies()[i];
		if (!enemy->obj.isActive) continue;

		switch (obj->state) {
		case SAttack:
			b_circle = { obj->x, obj->y, 6 };
			break;
		case SChargeAttack:
			float offset = 16.f;
			float radius = 8.f * GetPlayer()->gunScale;
			b_circle = { obj->x + cosf(bullet->obj.angle) * offset, obj->y + sinf(bullet->obj.angle) * offset, radius };
		}

		FRECT e_rect = GetEnemyRect(enemy);

		if (IsHitRectWithCircle(e_rect, b_circle)) {
			switch(obj->state){
			case SAttack:
				if (enemy->obj.state != SDead)
					DamageBullet(&enemy->obj, bullet);
				break;
			case SChargeAttack:
				if (enemy->obj.state != SDead && enemy->obj.state != SDamaged)
					Damage(&enemy->obj, bullet->power);
				break;
			}
			if (obj->state == SAttack) {
				obj->isActive = false;
				return;
			}
		}
	}

	/*�X�e�[�W�Ƃ̓����蔻��*/
	// �X�e�[�W�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// �e�̓����蔻�蒲��
	b_circle.y += b_circle.radius;

	// �����蔻�菈���y��
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	if (x_start < 0) x_start = 0;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	if (y_start < 0) y_start = 0;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// ���菈��
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_end; j++) {
			if (hitTable[stage[j + i * STAGE_SIZE_X]] == 1)
			{
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile = { left, top, left + TILE_SIZE, top + TILE_SIZE };

				if (IsHitRectWithCircle(tile, b_circle)) {
					obj->isActive = false;
					return;
				}
			}
		}
	}
}

void Bullet_Update()
{
	for (int i = 0; i < BULLET_MAX; i++) 
	{
		if (bullet[i].obj.isActive) {
			Bullet_Update(&bullet[i]);

			Animate(&bullet[i].animator);
		}
	}
}

void Bullet_Render()
{
	const CAMERA* cam = GetCamera();

	for (int i = 0; i < BULLET_MAX; i++)
	{
		OBJECT* obj = &bullet[i].obj;
		if (!obj->isActive) continue;

		const Sprite* sprite = GetSprite(bullet[i].animator.params[bullet[i].animator.animID].sprites[0]);

		RenderSpriteRot(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, obj->angle, bullet[i].scale);

		//RenderPoly(b_circle.x - b_circle.radius - cam->x, b_circle.y - b_circle.radius - cam->y, b_circle.radius * 2, b_circle.radius * 2, D3DCOLOR_RGBA(255, 0, 0, 128));
	}
}

void Fire(float x, float y, float rad, bool isRight, STATE state)
{
	const GAME_MANAGER* gm = GetGameManager();
	int num  = 0;

	switch (state) {
	case SAttack:
		switch (gm->energyLevel)
		{
		case 1: num = 1;
			break;
		case 2: num = 3;
			break;
		case 3: num = 5;
			break;
		case 4: num = 7;
			break;
		}
		PlaySounds(GunAttack);
		break;
	case SChargeAttack:
		num = 1;
		PlaySounds(GunChargeAttack);
		break;
	}

	for (int i = -num / 2; i <= num / 2; i++) {
		// �g���Ă��Ȃ��e��T���o��
		for (int j = 0; j < BULLET_MAX; j++) {
			if (bullet[j].obj.isActive) continue;

			OBJECT* obj = &bullet[j].obj;
			obj->isActive = true;
			obj->x = x;
			obj->y = y;
			obj->vx = cosf(rad + 0.1f * i) * obj->speed;
			obj->vy = sinf(rad + 0.1f * i) * obj->speed;
			obj->angle = rad;
			obj->isRight = isRight;
			obj->state = state;
			bullet[j].power = GetPlayer()->gunPower;

			switch (state) {
			case SAttack:
				bullet[j].scale = 1.f;
				bullet[j].power = GetPlayer()->gunPower;
				AnimChange(&bullet[j].animator, 0);
				break;
			case SChargeAttack:
				bullet[j].scale = GetPlayer()->gunScale;
				bullet[j].power = GetPlayer()->cGunPower;
				AnimChange(&bullet[j].animator, 1);
				break;
			}
			break;
		}
	}
}

void DamageBullet(OBJECT* obj, BULLET* bullet)
{
	AddDamageUI(bullet->obj.x, bullet->obj.y, bullet->power);
	obj->hp -= bullet->power;

	if (obj->hp <= 0)
		obj->state = SDead;
	else
		obj->state = SDamaged;
}
