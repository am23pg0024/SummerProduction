#include "UnlimitedLib/UnlimitedLib.h"
#include "Explosion.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Camera.h"
#include "HitCheck.h"
#include "Parameter.h"

EXPLOSION explosions[MAX_ENEMY];

ANIM_PARAM explAnimParam = { NULL, 30, 20, false };

// 攻撃判定(円)
static CIRCLE at_circle;

static float scale = 1.f;

EXPLOSION* GetExplosions() { return explosions; }
CIRCLE* GetExplosionCircle(EXPLOSION* explosion) {
	at_circle = { explosion->obj.x, explosion->obj.y, 96 * scale };
	return &at_circle;
}

void CreateExplosion(float x, float y)
{
	for (int i = 0; i < GetMaxSpawn(); i++) {
		OBJECT* obj = &explosions[i].obj;
		if (obj->isActive) continue;

		obj->x = x;
		obj->y = y;
		obj->isActive = true;

		ANIMATOR* animator = &explosions[i].animator;
		animator->animID = 0;
		animator->animCnt = 0;

		explosions[i].power = GetParameter()->explosion.power;

		break;
	}
}

void Explosion_Restart()
{
	// 初期化
	for (int i = 0; i < MAX_ENEMY; i++) {
		OBJECT* obj = &explosions[i].obj;
		obj->sprite = explAnimParam.sprites[0];
		obj->x = obj->y = 0;
		obj->angle = 0;
		obj->isActive = false;

		ANIMATOR* animator = &explosions[i].animator;
		animator->animID = 0;
		animator->animCnt = 0;

		explosions[i].power = 0;
	}

	scale = GetParameter()->explosion.scale;
}

void Explosion_Initialize()
{
	// メモリ確保
	explAnimParam.sprites = (int*)malloc(sizeof(int) * explAnimParam.total);

	// スプライト読み込み
	CreateSpriteMatrix("Assets/Effect/enemy_explosion.png", 192, 192, 10, 3, 30, explAnimParam.sprites);

	// スプライトの原点を設定
	for (int i = 0; i < explAnimParam.total; i++) {
		SetSpriteOrigin(explAnimParam.sprites[i], 96, 96);
		SetSpriteScale(explAnimParam.sprites[i], scale, scale);
	}

	// 初期化
	for (int i = 0; i < MAX_ENEMY; i++) {
		OBJECT* obj = &explosions[i].obj;
		obj->sprite = explAnimParam.sprites[0];
		const Sprite* sp = GetSprite(explAnimParam.sprites[0]);
		obj->width = sp->width;
		obj->height = sp->height;
		obj->x = obj->y = 0;
		obj->angle = 0;
		obj->isActive = false;

		ANIMATOR* animator = &explosions[i].animator;
		animator->owner = &explosions[i].obj;
		animator->params = &explAnimParam;
		animator->animID = 0;
		animator->animCnt = 0;

		explosions[i].power = 0;
	}

	scale = GetParameter()->explosion.scale;
}

void Explosion_Finalize()
{
	ReleaseSpriteMatrix(explAnimParam.sprites, explAnimParam.total);
	free(explAnimParam.sprites);
}

void Explosion_Update()
{
	for (int i = 0; i < GetMaxSpawn(); i++) {
		OBJECT* obj = &explosions[i].obj;
		if (!obj->isActive) continue;

		ANIMATOR* animator = &explosions[i].animator;
		float& cnt = animator->animCnt;
		ANIM_PARAM& param = animator->params[animator->animID];

		if ((int)cnt == 0) {
			/* 当たり判定 */
			// プレイヤーとの当たり判定
			PLAYER* player = GetPlayer();
			if (player->obj.isActive && !PlayerIsInvincible()) {
				CIRCLE* circle = GetExplosionCircle(&explosions[i]);
				FRECT p_rect = GetPlayerRect();
				if (IsHitRectWithCircle(p_rect, at_circle))
					if (!CheckBetween(*obj, player->obj))
						Damage2Player(player, explosions[i].power);
			}

			// 敵との当たり判定
			for (int j = 0; j < GetMaxSpawn(); j++) {
				ENEMY* enemy = GetEnemies()[j];
				if (!enemy->obj.isActive || enemy->obj.state == SDamaged || enemy->obj.state == SDead) continue;

				FRECT e_rect = GetEnemyRect(enemy);
				at_circle = { obj->x, obj->y, 96 * scale };
				if (IsHitRectWithCircle(e_rect, at_circle))
					if (!CheckBetween(*obj, enemy->obj))
						Damage(&enemy->obj, explosions[i].power);
			}
		}

		Animate(animator);

		if (cnt >= param.total - 1) {
			obj->isActive = false;
		}
	}
}

void Explosion_Render()
{
	const CAMERA* cam = GetCamera();

	for (int i = 0; i < GetMaxSpawn(); i++) {
		OBJECT* obj = &explosions[i].obj;
		if (!obj->isActive) continue;

		ANIMATOR* animator = &explosions[i].animator;

		const Sprite* sprite = GetSprite(animator->params[animator->animID].sprites[0]);

		if (obj->x < cam->x - sprite->width || cam->x + SCREEN_WIDTH + sprite->width < obj->x ||
			obj->y < cam->y - sprite->height || cam->y + SCREEN_HEIGHT + sprite->height < obj->y)
			continue;

		// 描画処理
		RenderSpriteRot(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0.f, scale);
	}
}
