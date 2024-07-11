#include "UnlimitedLib/UnlimitedLib.h"
#include "UnlimitedLib/UnlimitedLibExtension.h"
#include "2DHelperEx.h"
#include "Game.h"
#include "Player.h"
#include "Animation.h"
#include "Camera.h"
#include "math.h"
#include "Effect.h"

ANIM_OBJECT chargeEffect;
ANIM_OBJECT swordEffect;

static int* effSpritePool;	// アニメーションスプライトプール

void ChargeEffect();
bool IsSwordEffectActive() { return swordEffect.obj.isActive; }
void UpdateSwordEffect();

ANIM_PARAM effAnimParam[] = {
	{ NULL, 11, 12, true },		// 0.溜め中のエフェクト
	{ NULL,  6, 40, false},		// 1.剣攻撃時のエフェクト
};

void Effect_Restart()
{
	OBJECT* obj = &chargeEffect.obj;
	obj->x = 0;
	obj->y = 0;
	obj->isActive = false;
	chargeEffect.animator.animID = 0;
	chargeEffect.animator.animCnt = 0;

	obj = &swordEffect.obj;
	obj->x = 0;
	obj->y = 0;
	obj->angle = 0;
	obj->isActive = false;
	swordEffect.animator.animID = 1;
	swordEffect.animator.animCnt = 0;
}

void Effect_Initialize()
{
	ANIM_PARAM& param = chargeEffect.animator.params[chargeEffect.animator.animID];

	// メモリ確保＆割当
	if (effSpritePool == NULL) {
		int size = 0;
		for (int i = 0; i < 2; i++)
			size += effAnimParam[i].total;

		// メモリ確保＆ゼロ初期化
		effSpritePool = (int*)malloc(sizeof(int) * size);
		ZeroMemory(effSpritePool, sizeof(int) * size);

		// メモリ割当
		for (int i = 0; i < 2; i++) {
			if (i == 0)
				effAnimParam[i].sprites = effSpritePool;
			else
				effAnimParam[i].sprites = effAnimParam[i - 1].sprites + effAnimParam[i - 1].total;
		}
	}

	CreateSpriteMatrix("Assets/Effect/charge_effect.png", 40, 40, 11, 1, 11, effAnimParam[0].sprites);		// 溜め時のエフェクト
	CreateSpriteMatrix("Assets/Effect/sword_effect_01.png", 256, 257, 3, 2, 6, effAnimParam[1].sprites);	// 剣攻撃時のエフェクト

	// 中心位置設定
	for (int i = 0; i < effAnimParam[0].total; i++)
	{
		SetSpriteOrigin(effAnimParam[0].sprites[i], 20, 20);
	}
	for (int i = 0; i < effAnimParam[1].total; i++)
	{
		SetSpriteOrigin(effAnimParam[1].sprites[i], 128, 128);
	}

	OBJECT* obj = &chargeEffect.obj;
	obj->x = 0;
	obj->y = 0;
	obj->isActive = false;
	chargeEffect.animator.owner = obj;
	chargeEffect.animator.params = effAnimParam;
	chargeEffect.animator.animID = 0;
	chargeEffect.animator.animCnt = 0;

	obj = &swordEffect.obj;
	obj->x = 0;
	obj->y = 0;
	obj->angle = 0;
	obj->isActive = false;
	swordEffect.animator.owner = obj;
	swordEffect.animator.params = effAnimParam;
	swordEffect.animator.animID = 1;
	swordEffect.animator.animCnt = 0;
}

void Effect_Finalize()
{
	for (int i = 0; i < 2; i++)
	{
		ReleaseSpriteMatrix(effAnimParam[i].sprites, effAnimParam[i].total);
	}

	// メモリ解放
	free(effSpritePool);
	effSpritePool = NULL;
}

void Effect_Update()
{
	ChargeEffect();

	UpdateSwordEffect();
}

void Effect_Render()
{
	OBJECT* obj = &chargeEffect.obj;
	if (obj->isActive)
	{
		const Sprite* sprite = GetSprite(chargeEffect.animator.params[chargeEffect.animator.animID].sprites[0]);
		const CAMERA* cam = GetCamera();

		RenderSpriteRot(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, 1.f);
	}

	obj = &swordEffect.obj;
	if (obj->isActive)
	{
		const Sprite* sprite = GetSprite(swordEffect.animator.params[swordEffect.animator.animID].sprites[0]);
		const CAMERA* cam = GetCamera();

		switch (GetPlayer()->obj.state) {
		case SAttack:
			if (cosf(GetPlayer()->obj.angle) >= 0)
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, obj->angle + 0.615, GetPlayer()->swordLength / 117.f, GetPlayer()->swordLength / 117.f);
			else
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, -obj->angle + 0.615, GetPlayer()->swordLength / 117.f, -GetPlayer()->swordLength / 117.f);
			break;
		case SChargeAttack:
			// 色変更
			SetHSVParam(-1.f, 1.f, 1.f);
			SetCustomShaderParam(GetHSVParam(), sizeof(*GetHSVParam()));

			BeginCustomShader(GetChangeHSVShader());
			if (cosf(GetPlayer()->obj.angle) >= 0)
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, obj->angle + 0.615, GetPlayer()->cSwordLength / 117.f, GetPlayer()->cSwordLength / 117.f);
			else
				RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, -obj->angle + 0.615, GetPlayer()->cSwordLength / 117.f, -GetPlayer()->cSwordLength / 117.f);

			EndCustomShader();
			break;
		}
	}
}

void ChargeEffect()
{
	OBJECT* obj = &chargeEffect.obj;
	if (GetPlayer()->obj.state == SCharging || GetPlayer()->obj.state == SChargeAttack)
	{
		obj->x = GetPlayer()->obj.x;
		obj->y = GetPlayer()->obj.y;
		obj->isActive = true;
		
		Animate(&chargeEffect.animator);
	}
	else obj->isActive = false;
}

void CreateSwordEffect(float x, float y, float angle)
{
	OBJECT* obj = &swordEffect.obj;

	obj->x = x + cosf(angle);
	obj->y = y + sinf(angle);
	obj->angle = angle;
	obj->isActive = true;
	swordEffect.animator.animID = 1;
	swordEffect.animator.animCnt = 0;
}

void UpdateSwordEffect()
{
	OBJECT* obj = &swordEffect.obj;

	if (obj->isActive) {
		OBJECT* p_obj = &GetPlayer()->obj;
		obj->x = p_obj->x + cosf(obj->angle);
		obj->y = p_obj->y + sinf(obj->angle);
		
		Animate(&swordEffect.animator);

		if (p_obj->state != SAttack && p_obj->state != SChargeAttack) {
			float& cnt = swordEffect.animator.animCnt;
			ANIM_PARAM& param = swordEffect.animator.params[swordEffect.animator.animID];
			if (cnt >= param.total - 1)
				swordEffect.obj.isActive = false;
		}
	}
}