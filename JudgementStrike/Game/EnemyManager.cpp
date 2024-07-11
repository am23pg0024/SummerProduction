#include "UnlimitedLib/UnlimitedLib.h"
#include "UnlimitedLib/UnlimitedLibExtension.h"
#include "EnemyManager.h"
#include "NormalEnemy.h"
#include "HighEnemy.h"
#include "ExplosionEnemy.h"
#include "Player.h"
#include "GameManager.h"
#include "Stage.h"
#include "HitCheck.h"
#include "2DHelperEx.h"
#include "Camera.h"
#include "Parameter.h"
#include <math.h>

int MAX_SPAWN;
float MAX_BLINK_TIME;

ENEMY* enemies[MAX_ENEMY] = { NULL };

ANIM_PARAM eAnimParam[] = {
	{ NULL, 13, 10, true },		// 0. �ҋ@
	{ NULL,  8, 10, true },		// 1. �ړ�
	{ NULL, 15, 10, false },	// 2. �U��1
	{ NULL, 10, 10, false },	// 3. �U��2
	{ NULL, 10, 10, false },	// 4. �U��3
	{ NULL,  3, 10, false },	// 5. ��_���[�W
	{ NULL,  8, 10, false },	// 6. ���S
};

int* eSpritePool;	// �A�j���[�V�����X�v���C�g�v�[��

static PLAYER* player;

ETYPE* spawnTable = NULL;	// �X�|�[���e�[�u��
float spawnTime;			// ���̓G�X�|�[���܂ł̎���
int enemyNum;				// ���݂̓G�̐�


/* �֐��v���g�^�C�v�錾 */
ENEMY** GetEnemies() { return enemies; }
int GetMaxSpawn() { return MAX_SPAWN; }

// �v���C���[�Ƃ̓����蔻��p
FRECT GetEnemyRect(ENEMY* enemy)
{
	enemy->obj.hitRect = { enemy->obj.x - 21, enemy->obj.y - 22, enemy->obj.x + 21, enemy->obj.y + 22 };
	return enemy->obj.hitRect;
}

void EnemyIdle(ENEMY* enemy);
void EnemyMove(ENEMY* enemy);
void EnemyAttack(ENEMY* enemy);
void EnemyDamaged(ENEMY* enemy);
void EnemyDead(ENEMY* enemy);

void EnemyHitCheckEnemy(ENEMY* enemy, float* slide_x, float* slide_y);
bool EnemyHitCheck(ENEMY* enemy);
bool EnemyHitCheckX(ENEMY* enemy);
bool EnemyHitCheckY(ENEMY* enemy);
void SearchSpawnPos(ENEMY* enemy);
void Spawn();
void BlinkEnemy(ENEMY* enemy);
void SortEnemies(int left, int right);

void SetEnemy(float x, float y, float angle, bool isRight, ETYPE type)
{
	for (int i = 0; i < MAX_SPAWN; i++) {
		ENEMY* enemy = enemies[i];
		OBJECT* obj = &enemy->obj;

		if (obj->isActive) continue;

		obj->x = x;
		obj->y = y;
		obj->angle = angle;
		obj->isActive = true;
		obj->isRight = isRight;
		obj->state = SMove;

		enemy->animator.animID = 0;
		enemy->animator.animCnt = 0;

		enemy->type = type;
		enemy->isBlinking = false;
		enemy->isInvisible = true;
		enemy->blinkTime = 0.f;

		const ENEMY_PARAM* param = GetParameter()->enemy.params;
		switch (type) {
		case ENormal:
			obj->vx = -param[0].speed * cosf(angle);
			obj->vy = -param[0].speed * sinf(angle);
			obj->speed = param[0].speed;
			obj->hp = param[0].hp;
			enemy->power = param[0].power;
			enemy->hitPower = param[0].hitPower;
			enemy->score = 100;
			enemy->hsv.hue = 0.f;
			enemy->hsv.saturation = 1.f;
			enemy->hsv.value = 1.f;
			break;
		case EHigh:
			obj->vx = -param[1].speed * cosf(angle);
			obj->vy = -param[1].speed * sinf(angle);
			obj->speed = param[1].speed;
			obj->hp = param[1].hp;
			enemy->power = param[1].power;
			enemy->hitPower = param[1].hitPower;
			enemy->score = 300;
			enemy->hsv.hue = -0.2f;
			enemy->hsv.saturation = 3.f;
			enemy->hsv.value = 1.2f;
			break;
		case EExplosion:
			obj->vx = -param[2].speed * cosf(angle);
			obj->vy = -param[2].speed * sinf(angle);
			obj->speed = param[2].speed;
			obj->hp = param[2].hp;
			enemy->power = param[2].power;
			enemy->hitPower = param[2].hitPower;
			enemy->score = 150;
			enemy->hsv.hue = -1.0f;
			enemy->hsv.saturation = 3.f;
			enemy->hsv.value = 1.2f;
			break;
		}

		spawnTime = GetParameter()->wave.spawnTime;
		enemyNum++;

		SearchSpawnPos(enemy);

		return;
	}
}

void EnemyManager_Restart()
{
	// Enemy������
	for (int i = 0; i < MAX_SPAWN; i++) {
		ENEMY* enemy = enemies[i];
		OBJECT* obj = &enemy->obj;

		obj->sprite = eAnimParam[0].sprites[0];
		obj->x = obj->y = 0;
		obj->vx = obj->vy = 0;
		obj->speed = 0;
		obj->angle = 0;
		obj->isActive = false;
		obj->isRight = false;
		obj->state = SIdle;
		obj->hp = 0;

		enemy->type = ENormal;
		enemy->power = 0;
		enemy->hitPower = 0;
		enemy->score = 0;
		enemy->isBlinking = false;
		enemy->isInvisible = true;
		enemy->blinkTime = 0.f;
		enemy->hsv.hue = 0.f;
		enemy->hsv.saturation = 1.f;
		enemy->hsv.value = 1.f;

		enemy->animator.params = eAnimParam;
		enemy->animator.animID = 0;
		enemy->animator.animCnt = 0;
	}

	MAX_SPAWN = GetParameter()->enemy.maxSpawn;
	MAX_BLINK_TIME = GetParameter()->enemy.maxBlinkTime;
	spawnTime = 0.f;
	enemyNum = 0;
}

void EnemyManager_Initialize()
{
	MAX_SPAWN = GetParameter()->enemy.maxSpawn;

	// �������m�ہ�����
	for (int i = 0; i < MAX_SPAWN; i++) {
		if (enemies[i] == NULL)
			enemies[i] = (ENEMY*)malloc(sizeof(ENEMY));
	}

	if (eSpritePool == NULL) {
		int size = 0;
		for (int i = 0; i < 7; i++)
			size += eAnimParam[i].total;

		// �������m�ہ��[��������
		eSpritePool = (int*)malloc(sizeof(int) * size);
		ZeroMemory(eSpritePool, sizeof(int) * size);

		// ����������
		int* itr = eSpritePool;
		for (int i = 0; i < 7; i++) {
			eAnimParam[i].sprites = itr;
			itr += eAnimParam[i].total;
		}
	}
	if (spawnTable == NULL) {
		int maxSpawnNum = 0;
		for (WAVE_PARAM wp : GetParameter()->wave.params)
			maxSpawnNum = max(wp.spawnNum, maxSpawnNum);
;
		spawnTable = (ETYPE*)malloc(sizeof(ETYPE) * maxSpawnNum);
		ZeroMemory(spawnTable, sizeof(ETYPE) * maxSpawnNum);
	}

	// �X�v���C�g�ǂݍ���
	CreateSpriteMatrix("Assets/Sprites/Enemy/Idle.png", 180, 170, 13, 1, 13, eAnimParam[0].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Walk.png", 180, 170, 8, 1, 8, eAnimParam[1].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Attack1.png", 180, 170, 15, 1, 15, eAnimParam[2].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Attack2.png", 180, 170, 10, 1, 10, eAnimParam[3].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Attack3.png", 180, 170, 10, 1, 10, eAnimParam[4].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Take Hit.png", 180, 170, 3, 1, 3, eAnimParam[5].sprites);
	CreateSpriteMatrix("Assets/Sprites/Enemy/Death.png", 180, 170, 8, 1, 8, eAnimParam[6].sprites);

	// �X�v���C�g�̌��_��ݒ�
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < eAnimParam[i].total; j++)
			SetSpriteOrigin(eAnimParam[i].sprites[j], 95, 85);

	// Enemy������
	for (int i = 0; i < MAX_SPAWN; i++) {
		ENEMY* enemy = enemies[i];
		OBJECT* obj = &enemy->obj;

		obj->sprite = eAnimParam[0].sprites[0];
		obj->x = obj->y = 0;
		obj->vx = obj->vy = 0;
		obj->speed = 0;
		obj->angle = 0;
		obj->isActive = false;
		obj->isRight = false;
		obj->state = SIdle;
		obj->hp = 0;

		enemy->type = ENormal;
		enemy->power = 0;
		enemy->hitPower = 0;
		enemy->score = 0;
		enemy->isBlinking = false;
		enemy->isInvisible = true;
		enemy->blinkTime = 0.f;
		enemy->hsv.hue = 0.f;
		enemy->hsv.saturation = 1.f;
		enemy->hsv.value = 1.f;

		enemy->animator.owner = obj;
		enemy->animator.params = eAnimParam;
		enemy->animator.animID = 0;
		enemy->animator.animCnt = 0;
	}

	player = GetPlayer();

	MAX_BLINK_TIME = GetParameter()->enemy.maxBlinkTime;
	spawnTime = 0.f;
	enemyNum = 0;

	//NormalEnemy_Initialize();
	//ExplosionEnemy_Initialize();
}

void EnemyManager_Finalize()
{
	for (int i = 0; i < 7; i++)
		ReleaseSpriteMatrix(eAnimParam[i].sprites, eAnimParam[i].total);

	// ���������
	for (int i = 0; i < MAX_SPAWN; i++) {
		free(enemies[i]);
		enemies[i] = NULL;
	}
	free(eSpritePool);
	eSpritePool = NULL;
	free(spawnTable);
	spawnTable = NULL;

	//NormalEnemy_Finalize();
	//ExplosionEnemy_Finalize();
}

void EnemyManager_Update()
{
	// �X�|�[������
	if (GetGameState() == GPlay) {
		if (enemyNum < MAX_SPAWN) {
			spawnTime -= DELTA_TIME;

			if (spawnTime <= 0.f)
				Spawn();
		}
	}

	for (int i = 0; i < MAX_SPAWN; i++) {
		ENEMY* enemy = enemies[i];
		OBJECT* obj = &enemy->obj;

		if (!obj->isActive) continue;

		//if (IsKeyPush('N')) obj->isRight = false;
		//if (IsKeyPush('M')) obj->isRight = true;

		// �X�e�[�g�؂�ւ�
		switch (obj->state) {
		case SIdle:		EnemyIdle(enemy);		break;
		case SMove:		EnemyMove(enemy);		break;
		case SAttack:	EnemyAttack(enemy);		break;
		case SDamaged:	EnemyDamaged(enemy);	break;
		case SDead:		EnemyDead(enemy);		break;
		}

		//switch (enemy->type) {
		//case ENormal:		NormalEnemy_Update();		break;
		//case EHigh:	break;
		//case EExplosion:	ExplosionEnemy_Update();	break;
		//}

		// �_��
		if (enemy->isBlinking)
			BlinkEnemy(enemy);

		// ���W�X�V
		if (obj->state != SDead) {
			float slide_x = 0;
			float slide_y = 0;

			// ���̓G�Ƃ̓����蔻��
			EnemyHitCheckEnemy(enemy, &slide_x, &slide_y);
			obj->vx += slide_x;
			obj->vy += slide_y;

			// ���x����
			float speed = sqrtf(obj->vx * obj->vx + obj->vy * obj->vy);
			if (speed > obj->speed) {
				obj->vx = obj->vx * obj->speed / speed;
				obj->vy = obj->vy * obj->speed / speed;
			}

			// �X�e�[�W�Ƃ̓����蔻��
			obj->x += obj->vx * DELTA_TIME;
			EnemyHitCheckX(enemy);
			obj->y += obj->vy * DELTA_TIME;
			EnemyHitCheckY(enemy);
		}

		Animate(&enemy->animator);
	}

	if (MAX_SPAWN > 0)
		SortEnemies(0, MAX_SPAWN - 1);
}

void EnemyManager_Render()
{
	const CAMERA* cam = GetCamera();

	BeginCustomShader(GetChangeHSVShader());

	for (int i = 0; i < MAX_SPAWN; i++) {
		ENEMY* enemy = enemies[i];
		OBJECT* obj = &enemy->obj;

		if (!obj->isActive) continue;

		const Sprite* sprite = GetSprite(enemy->animator.params[enemy->animator.animID].sprites[0]);

		if (obj->x < cam->x - sprite->width || cam->x + SCREEN_WIDTH + sprite->width < obj->x ||
			obj->y < cam->y - sprite->height || cam->y + SCREEN_HEIGHT + sprite->height < obj->y)
			continue;

		// HSV�ύX
		SetHSVParam(enemy->hsv.hue, enemy->hsv.saturation, enemy->hsv.value);
		SetCustomShaderParam(GetHSVParam(), sizeof(*GetHSVParam()));

		// �`�揈��
		if (enemy->isInvisible) {
			if (obj->isRight)
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0.f, 1.f, 1.f);
			else
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0.f, -1.f, 1.f);
		}
	}

	EndCustomShader();
}

void EnemyIdle(ENEMY* enemy)
{
	switch (enemy->type) {
	case ENormal:		NormalEnemyIdle(enemy);		break;
	case EHigh:			HighEnemyIdle(enemy);		break;
	case EExplosion:	ExplosionEnemyIdle(enemy);	break;
	}

}

void EnemyMove(ENEMY* enemy)
{
	AnimChange(&enemy->animator, 1);

	OBJECT* obj = &enemy->obj;
	PLAYER* player = GetPlayer();

	// �v���C���[�ւ̃x�N�g��
	float target_vx = 0;
	float target_vy = 0;

	if (GetGameState() == GPlay) {
		// �v���C���[��ǂ�������
		float angle = atan2f(player->obj.y - obj->y, player->obj.x - obj->x);
		target_vx = cosf(angle) * obj->speed;
		target_vy = sinf(angle) * obj->speed;
		obj->vx += (target_vx - obj->vx) / 10;
		obj->vy += (target_vy - obj->vy) / 10;

		// �v���C���[�ƈ�苗���ȉ��ɂȂ�����Attack��
		if (GetDistance(obj, &player->obj) < 70) {
			obj->state = SAttack;
			AnimChange(&enemy->animator, 2);
		}
	}
	else {
		obj->state = SIdle;
	}

	// �����`�F�b�N
	if (target_vx > 0)
		obj->isRight = true;
	else if (target_vx < 0)
		obj->isRight = false;
}

void EnemyAttack(ENEMY* enemy)
{
	switch (enemy->type) {
	case ENormal:		NormalEnemyAttack(enemy);		break;
	case EHigh:			HighEnemyAttack(enemy);			break;
	case EExplosion:	ExplosionEnemyAttack(enemy);	break;
	}
}

void EnemyDamaged(ENEMY* enemy)
{
	AnimChange(&enemy->animator, 5);
	
	if (!enemy->isBlinking)
		enemy->isBlinking = true;

	OBJECT* obj = &enemy->obj;
	obj->vx = obj->vy = 0;

	float& cnt = enemy->animator.animCnt;
	ANIM_PARAM& param = enemy->animator.params[enemy->animator.animID];
	if (cnt >= param.total - 1)
	{
		enemy->obj.state = SIdle;
	}
}

void EnemyDead(ENEMY* enemy)
{
	switch (enemy->type) {
	case EExplosion:
		ExplosionEnemyDead(enemy);
		break;
	default:
		AnimChange(&enemy->animator, 6);
		OBJECT* obj = &enemy->obj;
		obj->vx = obj->vy = 0;

		float& cnt = enemy->animator.animCnt;
		ANIM_PARAM& param = enemy->animator.params[enemy->animator.animID];
		if (cnt >= param.total - 1)
		{
			obj->isActive = false;
			if (GetGameState() == GPlay) {
				AddKillCount();
				AddComboCount();
				AddScore(enemy);
			}
			enemyNum--;
		}
		break;
	}
}

void EnemyHitCheckEnemy(ENEMY* enemy, float* slide_x, float* slide_y)
{
	OBJECT* obj = &enemy->obj;

	// �ΓG�p�����蔻��
	CIRCLE hit_circle = { obj->x, obj->y, 21 };

	float max_diff = 0;

	for (int i = 0; i < MAX_SPAWN; i++) {
		OBJECT* target = &enemies[i]->obj;
		if (!target->isActive || obj == target)
			continue;

		CIRCLE target_circle = { target->x, target->y, 21 };

		if (IsHitCircle(hit_circle, target_circle)) {
			// �^�[�Q�b�g���玩���ւ̃x�N�g��
			float tx = obj->x - target->x;
			float ty = obj->y - target->y;
			float length = sqrtf(tx * tx + ty * ty);

			// �d�Ȃ��Ă��镔���̒������v�Z
			float r = hit_circle.radius + target_circle.radius;
			float diff = r - length;
			max_diff = max(max_diff, diff);

			// �^�[�Q�b�g�̑��x�x�N�g�������Z
			tx += target->vx * DELTA_TIME;
			ty += target->vy * DELTA_TIME;

			// ���K��
			length = sqrtf(tx * tx + ty * ty);
			if (length > 0) {
				tx /= length;
				ty /= length;
			}

			// ������x�N�g���ɉ��Z���Ă���
			*slide_x += tx * diff * 2.f;
			*slide_y += ty * diff * 2.f;
		}
	}
}

bool EnemyHitCheck(ENEMY* enemy)
{
	OBJECT* obj = &enemy->obj;

	// �����蔻���`
	FRECT enemy_rect = { obj->x - 21, obj->y, obj->x + 21, obj->y + 24 };

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// ������s���͈͂𐧌�
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;

	if (x_start < 0) x_start = 0;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	if (y_start < 0) y_start = 0;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// �X�e�[�W�Ƃ̓����蔻��
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_end; j++) {
			int tile_num = stage[j + i * STAGE_SIZE_X];

			if (hitTable[tile_num]) {
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile_rect = { left, top, left + TILE_SIZE, top + TILE_SIZE };

				if (IsHit(&enemy_rect, &tile_rect)) {
					return true;
				}
			}
		}
	}

	return false;
}

bool EnemyHitCheckX(ENEMY* enemy)
{
	OBJECT* obj = &enemy->obj;

	// �����蔻���`
	FRECT enemy_rect = { obj->x - 21, obj->y, obj->x + 21, obj->y + 24 };

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// ������s���͈͂𐧌�
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;

	if (x_start < 0) x_start = 0;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	if (y_start < 0) y_start = 0;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// �X�e�[�W�Ƃ̓����蔻��
	bool isOk = false;
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_end; j++) {
			int tile_num = stage[j + i * STAGE_SIZE_X];

			if (hitTable[tile_num]) {
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile_rect = { left, top, left + TILE_SIZE, top + TILE_SIZE };

				// �E�̔���
				if (obj->vx > 0) {
					if (IsHitRight(&enemy_rect, &tile_rect)) {
						obj->x = tile_rect.left - (enemy_rect.right - obj->x);
						//obj->vx *= -1;
						//obj->vx = 0;
						//obj->isRight = !obj->isRight;
						return true;
					}
				}
				// ���̔���
				else if (obj->vx < 0) {
					if (IsHitLeft(&enemy_rect, &tile_rect)) {
						obj->x = tile_rect.right + (obj->x - enemy_rect.left);
						//obj->vx *= -1;
						//obj->vx = 0;
						//obj->isRight = !obj->isRight;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool EnemyHitCheckY(ENEMY* enemy)
{
	OBJECT* obj = &enemy->obj;

	// �����蔻���`
	FRECT enemy_rect = { obj->x - 21, obj->y, obj->x + 21, obj->y + 24 };

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// ������s���͈͂𐧌�
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;

	if (x_start < 0) x_start = 0;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	if (y_start < 0) y_start = 0;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// �X�e�[�W�Ƃ̓����蔻��
	bool isOk = false;
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_end; j++) {
			int tile_num = stage[j + i * STAGE_SIZE_X];

			if (hitTable[tile_num]) {
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile_rect = { left, top, left + TILE_SIZE, top + TILE_SIZE };

				// ��̔���
				if (obj->vy < 0) {
					if (IsHitTop(&enemy_rect, &tile_rect)) {
						obj->y = tile_rect.bottom + (obj->y - enemy_rect.top);
						//obj->vy *= -1;
						//obj->vy = 0;
						return true;
					}
				}
				// ���̔���
				else if (obj->vy > 0) {
					if (IsHitBottom(&enemy_rect, &tile_rect)) {
						obj->y = tile_rect.top - (enemy_rect.bottom - obj->y);
						//obj->vy *= -1;
						//obj->vy = 0;
						return true;
					}
				}
			}
		}
	}

	return false;
}

// �X�|�[���ʒu��T��
void SearchSpawnPos(ENEMY* enemy)
{
	OBJECT* obj = &enemy->obj;
	float min_dist = 100000.f;

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// �G�̃X�e�[�W���W
	int enemy_x = obj->x / TILE_SIZE;
	int enemy_y = obj->y / TILE_SIZE;

	int range = 1;		// �T�[�`�p�ϐ�
	bool isOk = false;

	while (!isOk) {
		int dx[] = { -1, 0, 1 };
		int dy[] = { -1, 0, 1 };

		// �G�̎���8�������T�[�`
		for (int i = dy[0] * range; i < dy[2] * range; i++) {
			for (int j = dx[0] * range; j < dx[2] * range; j++) {
				int stage_x = enemy_x + j;
				int stage_y = enemy_y + i;
				if (stage_x<0 || stage_x>STAGE_SIZE_X - 1 || stage_y<0 || stage_y>STAGE_SIZE_Y - 1)
					continue;

				int target = stage[stage_x + stage_y * STAGE_SIZE_X];

				// �^�[�Q�b�g�`�b�v��0�Ȃ�
				if (hitTable[target] == 0) {
					// �G�̍��W����^�[�Q�b�g�`�b�v�̒��S�܂ł̋������v�Z
					float dist_x = stage_x * TILE_SIZE + TILE_SIZE / 2 - obj->x;
					float dist_y = stage_y * TILE_SIZE + TILE_SIZE / 2 - obj->y;
					float dist = dist_x * dist_x + dist_y * dist_y;

					// ���������܂łōŏ��Ȃ�
					if (dist < min_dist) {
						float tmp_x = obj->x;
						float tmp_y = obj->y;
						obj->x = stage_x * TILE_SIZE + TILE_SIZE / 2;
						obj->y = stage_y * TILE_SIZE + TILE_SIZE / 2;

						if (!EnemyHitCheck(enemy)) {
							// �X�e�[�W�Ƃ̓����蔻�肪false�Ȃ�ŏ��������X�V���Ď��̃��[�v��
							min_dist = min(dist, min_dist);
						}
						else {
							// true�Ȃ���W��߂�
							obj->x = tmp_x;
							obj->y = tmp_y;
						}
					}
				}
			}
		}

		if (min_dist != 100000.f) {
			isOk = true;
		}
		else {
			min_dist = 100000.f;
			range++;
		}
	}
}

void Spawn()
{
	const GAME_MANAGER* gm = GetGameManager();

	/* �X�|�[���e�[�u���ݒ� */
	int cnt = 0;

	// Normal�ŏ�����
	for (int i = 0; i < gm->spawnNum; i++)
		spawnTable[i] = ENormal;

	// High
	for (int i = 0; i < gm->spawnNum * gm->highRate; i++) {
		spawnTable[cnt] = EHigh;
		cnt++;
		if (cnt >= gm->spawnNum)
			break;
	}
	// Explosion
	for (int i = 0; i < gm->spawnNum * gm->explRate; i++) {
		spawnTable[cnt] = EExplosion;
		cnt++;
		if (cnt >= gm->spawnNum)
			break;
	}

	// �X�|�[���������v���C���[�̐i�s�����ɂ���Č��߂�
	float p_angle = 0;
	if (player->obj.vx == 0 && player->obj.vy == 0)
		p_angle = rand() % 314 / 100.f;		// �v���C���[���~�܂��Ă����烉���_��
	else
		p_angle = atan2f(player->obj.vy, player->obj.vx);

	for (int i = 0; i < gm->spawnNum; i++) {
		if (enemyNum >= MAX_SPAWN) break;

		// �X�|�[�����W�ݒ�
		float pos_x = cosf(p_angle) * SCREEN_WIDTH + rand() % 200 - 100;
		float pos_y = sinf(p_angle) * SCREEN_HEIGHT + rand() % 200 - 100;
		float spawn_x = player->obj.x + pos_x;
		float spawn_y = player->obj.y + pos_y;
		if (spawn_x <= TILE_SIZE)
			spawn_x = TILE_SIZE;
		if (spawn_x >= TILE_SIZE * STAGE_SIZE_X - TILE_SIZE)
			spawn_x = TILE_SIZE * STAGE_SIZE_X - TILE_SIZE * 2;
		if (spawn_y <= TILE_SIZE)
			spawn_y = TILE_SIZE * 4;
		if (spawn_y >= TILE_SIZE * STAGE_SIZE_Y - TILE_SIZE)
			spawn_y = TILE_SIZE * STAGE_SIZE_Y - TILE_SIZE * 2;

		// �X�|�[�����W���J��������������pos_x, pos_y�𔽓]������
		const CAMERA* cam = GetCamera();
		if (cam->x <= spawn_x && spawn_x <= cam->x + SCREEN_WIDTH) {
			spawn_x = player->obj.x - pos_x;

			if (spawn_x <= TILE_SIZE)
				spawn_x = TILE_SIZE;
			if (spawn_x >= TILE_SIZE * STAGE_SIZE_X - TILE_SIZE)
				spawn_x = TILE_SIZE * STAGE_SIZE_X - TILE_SIZE * 2;
		}
		if (cam->y <= spawn_y && spawn_y <= cam->y + SCREEN_HEIGHT) {
			spawn_y = player->obj.y - pos_y;

			if (spawn_y <= TILE_SIZE)
				spawn_y = TILE_SIZE * 4;
			if (spawn_y >= TILE_SIZE * STAGE_SIZE_Y - TILE_SIZE)
				spawn_y = TILE_SIZE * STAGE_SIZE_Y - TILE_SIZE * 2;
		}

		// �X�|�[���e�[�u���Ɋ�Â��G���X�|�[��
		float angle = atan2f(player->obj.y - spawn_y, player->obj.x - spawn_x);
		SetEnemy(spawn_x, spawn_y, angle, false, spawnTable[i]);
	}
}

void SubEnemyNum()
{
	enemyNum--;
}

void BlinkEnemy(ENEMY* enemy)
{
	if (enemy->isInvisible) {
		if ((int)(enemy->blinkTime * 10) % 2 == 0) {
			enemy->isInvisible = false;
		}
	}
	else {
		if ((int)(enemy->blinkTime * 10) % 2 == 0) {
			enemy->isInvisible = true;
		}
	}

	enemy->blinkTime += DELTA_TIME;
	if (enemy->blinkTime > MAX_BLINK_TIME) {
		enemy->isBlinking = false;
		enemy->isInvisible = true;
		enemy->blinkTime = 0.f;
	}
}

// �N�C�b�N�\�[�g�œG��y���W���Ƀ\�[�g����
void SortEnemies(int left, int right)
{
	float pivot = enemies[(left + right) / 2]->obj.y;
	int i = left;
	int j = right;

	while (1) {
		while (enemies[i]->obj.y < pivot) i++;
		while (enemies[j]->obj.y > pivot) j--;

		if (i >= j) break;

		ENEMY* tmp = enemies[i];
		enemies[i] = enemies[j];
		enemies[j] = tmp;
		i++; j--;
	}

	if (left < i - 1) SortEnemies(left, i - 1);
	if (j + 1 < right) SortEnemies(j + 1, right);
}
