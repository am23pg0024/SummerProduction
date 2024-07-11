#include "UnlimitedLib/UnlimitedLib.h"
#include "UnlimitedLib/UnlimitedLibExtension.h"
#include "Player.h"
#include "MemoryPool.h"
#include "EnemyManager.h"
#include "2DHelperEx.h"
#include "HitCheck.h"
#include "Stage.h"
#include "Bullet.h"
#include "Camera.h"
#include "GameManager.h"
#include "Explosion.h"
#include "Parameter.h"
#include "Shield.h"
#include "Effect.h"
#include "SoundManager.h"
#include "UI.h"
#include <math.h>

PLAYER player;

float outAnimCnt;	// 歩き→歩き銃攻撃のアニメーション遷移用変数

static bool isLPress;		// 剣攻撃ボタン入力フラグ
static bool isRPress;		// 銃攻撃ボタン入力フラグ

float CHARGE_CHECK_TIME;	// チャージ攻撃と判定される時間
float CHARGING_TIME;		// チャージにかかる時間
float chargeTimer;			// チャージ関連のタイマー
bool isCharging;			// チャージ中フラグ

float invincibleTime;		// 無敵時間
float MAX_INVINCIBLE_TIME;	// 無敵時間の最大値

float coolTime;			// 回避のクールタイム
float MAX_COOL_TIME;	// 回避のクールタイムの最大値
float AVOID_DIST;		// 回避で動く距離

static float blink_timer;	// 明滅時間
static bool isBlinking;		// 明滅フラグ
static bool isLight;		// 明滅フラグ(光っているかどうか)

ANIM_PARAM pAnimParam[] = {
	{ NULL, 4    , 10, true  },		// 0. 立ち/正面
	{ NULL, 6    , 10, true  },		// 1. 移動
	{ NULL, 3 + 1, 10, false },		// 2. 回避
	{ NULL, 3 + 1, 10, false },		// 3. 被ダメージ
	{ NULL, 2 + 1, 10, false },		// 4. 銃攻撃(待機)
	{ NULL, 6 + 1, 10, false },		// 5. 銃攻撃(移動しながら)
	{ NULL, 5 + 1, 20, false },		// 6. 剣攻撃
	{ NULL, 6 + 1, 10, false },		// 7. チャージ攻撃(銃)
	{ NULL, 2	 , 10, true  },		// 8. チャージ完了(銃)
	{ NULL, 4	 , 10, false },		// 9. チャージ発射(銃)
};

static int* pSpritePool;	// アニメーションスプライトプール
int pArrow;	// 攻撃方向可視化用マーカー

// 攻撃判定用
static FRECT at_rect;
static FAN fan;

HSV_PARAM hsv_default = { 0.f, 1.f, 1.f };

void PlayerSetInvincible(float time) { invincibleTime = time; }
bool PlayerIsInvincible() { return invincibleTime > 0 ? true : false; }

void PlayerIdle();
void PlayerMove();
void PlayerAttack();
void PlayerCharging();
void PlayerChargeAttack();
void PlayerAvoid();
void PlayerDamaged();
void PlayerDead();

void PlayerHitCheckX();
void PlayerHitCheckY();
void PlayerMoveInput();
void BlinkPlayer(const HSV_PARAM& hsv_light, const HSV_PARAM& hsv_dark);

PLAYER* GetPlayer() { return &player; }
FRECT GetPlayerRect()
{
	player.obj.hitRect = { player.obj.x - 14, player.obj.y - 25, player.obj.x + 14, player.obj.y + 25 };
	return player.obj.hitRect;
}

void Player_Restart()
{
	const PARAM* param = GetParameter();
	OBJECT* obj = &player.obj;
	obj->x = (TILE_SIZE * STAGE_SIZE_X + TILE_SIZE) / 2;
	obj->y = (TILE_SIZE * STAGE_SIZE_Y + TILE_SIZE) / 2;
	obj->vx = 0;
	obj->vy = 0;
	obj->speed = param->player.speed;
	obj->angle = 0;
	obj->isActive = true;
	obj->isRight = true;
	obj->state = SIdle;
	obj->hp = 35;

	player.speed2 = param->player.speed2;
	player.speed3 = param->player.speed3;
	player.shield = 35;

	player.isSword = true;
	player.swordPower = param->energyParams[0].swordPower;
	player.swordLength = param->energyParams[0].swordLength;
	player.swordAngle = param->energyParams[0].swordAngle;
	player.gunPower = param->energyParams[0].gunPower;
	player.gunScale = param->energyParams[0].gunScale;
	player.cSwordPower = param->energyParams[1].swordPower;
	player.cSwordLength = param->energyParams[1].swordLength;
	player.cSwordAngle = param->energyParams[1].swordAngle;
	player.cGunPower = param->energyParams[0].cGunPower;

	player.animator.animID = 0;
	player.animator.animCnt = 0;

	outAnimCnt = 0.f;

	isLPress = false;
	isRPress = false;

	chargeTimer = 0.f;
	isCharging = false;

	invincibleTime = 0.f;
	coolTime = 0.f;

	blink_timer = 0.f;
	isBlinking = false;
	isLight = true;

	fan = { obj->x, obj->y, player.swordLength, player.swordAngle };
}

void Player_Initialize()
{
	// メモリ確保＆割当
	if (pSpritePool == NULL) {
		int size = 0;
		for (int i = 0; i <= 9; i++)
			size += pAnimParam[i].total;

		// メモリ確保＆ゼロ初期化
		pSpritePool = (int*)malloc(sizeof(int) * size);
		ZeroMemory(pSpritePool, sizeof(int) * size);

		// メモリ割当
		for (int i = 0; i <= 9; i++) {
			if (i == 0)
				pAnimParam[i].sprites = pSpritePool;
			else
				pAnimParam[i].sprites = pAnimParam[i - 1].sprites + pAnimParam[i - 1].total;
		}
	}

	CreateSpriteMatrix("Assets/Sprites/Player/idle-spritesheet.png", 64, 64, 4, 1, 4, pAnimParam[0].sprites);			// 正面
	CreateSpriteMatrix("Assets/Sprites/Player/move-spritesheet.png", 64, 64, 6, 1, 6, pAnimParam[1].sprites);			// 移動
	CreateSpriteMatrix("Assets/Sprites/Player/jump-spritesheet.png", 64, 64, 3, 1, 3, pAnimParam[2].sprites);			// 回避
	CreateSpriteMatrix("Assets/Sprites/Player/damage-spritesheet.png", 64, 64, 3, 1, 3, pAnimParam[3].sprites);			// 被ダメージ
	CreateSpriteMatrix("Assets/Sprites/Player/robot-shoot-spritesheet.png", 64, 64, 2, 1, 2, pAnimParam[4].sprites);	// 銃攻撃(待機)
	CreateSpriteMatrix("Assets/Sprites/Player/moveshoot-spritesheet.png", 64, 64, 6, 1, 6, pAnimParam[5].sprites);		// 銃攻撃(移動しながら)
	CreateSpriteMatrix("Assets/Sprites/Player/beamsword-spritesheet.png", 84, 80, 5, 1, 5, pAnimParam[6].sprites);		// 剣攻撃
	CreateSpriteMatrix("Assets/Sprites/Player/chargeshot-spritesheet.png", 80, 64, 6, 1, 6, pAnimParam[7].sprites);		// チャージ攻撃(銃)
	// チャージ完了(銃)
	pAnimParam[8].sprites[0] = pAnimParam[7].sprites[3];
	pAnimParam[8].sprites[1] = pAnimParam[7].sprites[4];
	// チャージ発射(銃)
	//pAnimParam[9].sprites[0] = pAnimParam[7].sprites[1];
	//pAnimParam[9].sprites[1] = pAnimParam[7].sprites[2];
	pAnimParam[9].sprites[0] = pAnimParam[7].sprites[3];
	pAnimParam[9].sprites[1] = pAnimParam[7].sprites[4];
	pAnimParam[9].sprites[2] = pAnimParam[7].sprites[5];
	pAnimParam[9].sprites[3] = pAnimParam[7].sprites[5];	// わざと同じコマを２コマ入れる

	pArrow = CreateSprite("Assets/Sprites/Player/ingame_arrow.png");

	// アニメーションの間隔を同じにするため
	for (int i = 2; i < 8; i++)
	{
		pAnimParam[i].sprites[pAnimParam[i].total - 1] = pAnimParam[i].sprites[pAnimParam[i].total - 2];
	}

	// スプライトの中心位置を確定
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < pAnimParam[i].total; j++)
		{
			SetSpriteOrigin(pAnimParam[i].sprites[j], 31, 39);
		}
	}
	for (int j = 0; j < pAnimParam[6].total; j++)
	{
		SetSpriteOrigin(pAnimParam[6].sprites[j], 32, 55);
	}
	for (int j = 0; j < pAnimParam[7].total; j++)
	{
		SetSpriteOrigin(pAnimParam[7].sprites[j], 31, 39);
	}
	
	const PARAM* param = GetParameter();
	OBJECT* obj = &player.obj;
	obj->sprite = pAnimParam[0].sprites[0];
	obj->x = (TILE_SIZE * STAGE_SIZE_X + TILE_SIZE) / 2;
	obj->y = (TILE_SIZE * STAGE_SIZE_Y + TILE_SIZE) / 2;
	obj->vx = 0;
	obj->vy = 0;
	obj->speed = param->player.speed;
	obj->angle = 0;
	obj->isActive = true;
	obj->isRight = true;
	obj->state = SIdle;
	obj->hp = 35;

	player.speed2 = param->player.speed2;
	player.speed3 = param->player.speed3;
	player.shield = 35;
	player.hsv = { 0.f, 1.f, 1.f };
	
	player.isSword = true;
	player.swordPower = param->energyParams[0].swordPower;
	player.swordLength = param->energyParams[0].swordLength;
	player.swordAngle = param->energyParams[0].swordAngle;
	player.gunPower = param->energyParams[0].gunPower;
	player.gunScale = param->energyParams[0].gunScale;
	player.cSwordPower = param->energyParams[1].swordPower;
	player.cSwordLength = param->energyParams[1].swordLength;
	player.cSwordAngle = param->energyParams[1].swordAngle;
	player.cGunPower = param->energyParams[0].cGunPower;

	player.animator.owner = obj;
	player.animator.params = pAnimParam;
	player.animator.animID = 0;
	player.animator.animCnt = 0;

	outAnimCnt = 0.f;

	isLPress = false;
	isRPress = false;

	CHARGE_CHECK_TIME = param->player.CHARGE_CHECK_TIME;
	CHARGING_TIME = param->player.CHARGING_TIME;
	chargeTimer = 0.f;
	isCharging = false;

	invincibleTime = 0.f;
	MAX_INVINCIBLE_TIME = param->player.MAX_INVINCIBLE_TIME;

	coolTime = 0.f;
	MAX_COOL_TIME = param->player.MAX_COOL_TIME;
	AVOID_DIST = param->player.AVOID_DIST;
	
	blink_timer = 0.f;
	isBlinking = false;
	isLight = true;

	fan = { obj->x, obj->y, player.swordLength, player.swordAngle };
}

void Player_Finalize()
{
	for (int i = 0; i < 8; i++)
		ReleaseSpriteMatrix(pAnimParam[i].sprites, pAnimParam[i].total);
	ReleaseSprite(pArrow);

	// メモリ解放
	free(pSpritePool);
	pSpritePool = NULL;
}

void Player_Update()
{
	if (GetGameState() == GPlay) {
		/*<<<関数使用準備>>>*/
		OBJECT* obj = &player.obj;
		if (!obj->isActive) return;

		/*移動&攻撃の向きの入力*/
		PlayerMoveInput();

		// 回避
		if (coolTime <= 0.f && obj->state != SChargeAttack && obj->state != SDamaged && obj->state != SDead) {
			if ((IsKeyPress(VK_SPACE) || IsPadPressDirect(0, XINPUT_GAMEPAD_A))) {
				obj->state = SAvoid;
				player.hsv = hsv_default;
				isLPress = isRPress = isCharging = false;
				chargeTimer = 0;
			}
		}

		/*<<<状態切り替え>>>*/
		switch (obj->state) {
		case SIdle:			PlayerIdle();			break;
		case SMove:			PlayerMove();			break;
		case SDamaged:		PlayerDamaged();		break;
		case SDead:			PlayerDead();			break;
		case SAvoid:		PlayerAvoid();			break;
		case SAttack:		PlayerAttack();			break;
		case SCharging:		PlayerCharging();		break;
		case SChargeAttack:	PlayerChargeAttack();	break;
		}

		/*位置更新*/
		obj->x += obj->vx * DELTA_TIME;
		PlayerHitCheckX();
		obj->y += obj->vy * DELTA_TIME;
		PlayerHitCheckY();

		// 敵との当たり判定
		if (!PlayerIsInvincible()) {
			for (int i = 0; i < GetMaxSpawn(); i++) {
				ENEMY* enemy = GetEnemies()[i];
				if (!enemy->obj.isActive || enemy->obj.state == SDamaged || enemy->obj.state == SDead) continue;

				FRECT e_rect = GetEnemyRect(enemy);
				FRECT p_rect = GetPlayerRect();
				if (IsHit(&p_rect, &e_rect))
					Damage2Player(&player, enemy->hitPower);
			}
		}
	}

	// 無敵処理
	if (PlayerIsInvincible())
		invincibleTime -= DELTA_TIME;

	// クールタイム処理
	if (coolTime > 0)
		coolTime -= DELTA_TIME;

	Animate(&player.animator);
}

void Player_Render()
{
	OBJECT* obj = &player.obj;
	if (!obj->isActive) return;

	const Sprite* sprite = GetSprite(player.animator.params[player.animator.animID].sprites[0]);
	const CAMERA* cam = GetCamera();
	
	// 無敵テスト
	if (PlayerIsInvincible())
		SetSpriteColor(player.obj.sprite, D3DCOLOR_RGBA(255, 255, 255, 128));
	else
		SetSpriteColor(player.obj.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	// 描画処理
	BeginCustomShader(GetChangeHSVShader());
	// HSV変更
	SetHSVParam(player.hsv.hue, player.hsv.saturation, player.hsv.value);
	SetCustomShaderParam(GetHSVParam(), sizeof(*GetHSVParam()));
	if (obj->isRight == true) RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, 1.f, 1.f);
	else RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, -1.f, 1.f);
	EndCustomShader();

	RenderSpriteRot(pArrow, -38.f, 17.f, obj->x - cam->x, obj->y - cam->y, obj->angle, 1.f);

	//// 攻撃範囲テスト
	//if (player.animator.animID == 6) {
	//	if ((int)player.animator.animCnt == 1) {
	//		float ang = obj->angle;
	//		float rad = fan.angle * 3.14f / 180;
	//		for (float i = -rad / 2; i <= rad / 2; i += 0.04f) {
	//			for (int j = fan.radius / 2; j <= fan.radius; j += 2) {
	//				RenderPixel(obj->x + cosf(ang + i) * j - cam->x, obj->y + sinf(ang + i) * j - cam->y, D3DCOLOR_RGBA(255, 0, 0, 128));
	//			}
	//		}
	//	}
	//}
}

void PlayerIdle()
{
	AnimChange(&player.animator, 0);

	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = hsv_default;

	/*<<<Pad操作>>>*/
	if (IsPadConnect(0)) {
		// LBボタンが押されているかどうか
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		// LBボタンが離されたかどうか
		if (isLPress) {
			if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				obj->state = SAttack;
				player.isSword = true;
				isLPress = false;
				isCharging = false;
				chargeTimer = 0;
				AnimChange(&player.animator, 6);
				return;
			}
		}
		// RBボタンが押されているかどうか
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		// RBボタンが離されたかどうか
		if (isRPress) {
			if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				obj->state = SAttack;
				player.isSword = false;
				Fire(obj->x, obj->y, obj->angle, obj->isRight, SAttack);
				AnimChange(&player.animator, 4);

				isRPress = false;
				isCharging = false;
				chargeTimer = 0;
				return;
			}
		}
	}
	/*<<<マウス& キーボード操作>>>*/
	else {
		// 左クリックされているかどうか
		if (IsKeyPress(VK_LBUTTON)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		if (isLPress) {
			if (IsKeyRelease(VK_LBUTTON) || !IsKeyPress(VK_LBUTTON)) {
				obj->state = SAttack;
				player.isSword = true;
				isLPress = false;
				isCharging = false;
				chargeTimer = 0;
				AnimChange(&player.animator, 6);
				return;
			}
		}
		// 右クリックされているかどうか
		if (IsKeyPress(VK_RBUTTON)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		if (isRPress) {
			if (IsKeyRelease(VK_RBUTTON) || !IsKeyPress(VK_RBUTTON)) {
				obj->state = SAttack;
				player.isSword = false;
				Fire(obj->x, obj->y, obj->angle, obj->isRight, SAttack);
				AnimChange(&player.animator, 4);

				isRPress = false;
				isCharging = false;
				chargeTimer = 0;
				return;
			}
		}
	}

	// チャージ判定
	if (isCharging) {
		chargeTimer += DELTA_TIME;

		if (chargeTimer > CHARGE_CHECK_TIME) {
			obj->state = SCharging;
			isCharging = false;
			chargeTimer = 0;
			return;
		}
	}
}

void PlayerMove()
{
	AnimChange(&player.animator, 1);

	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = hsv_default;

	/*<<<Pad操作>>>*/
	if (IsPadConnect(0)) {
		// LBボタンが押されているかどうか
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		// LBボタンが離されたかどうか
		if (isLPress) {
			if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				obj->state = SAttack;
				player.isSword = true;
				isLPress = false;
				isCharging = false;
				chargeTimer = 0;
				AnimChange(&player.animator, 6);
				return;
			}
		}
		// RBボタンが押されているかどうか
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		// RBボタンが離されたかどうか
		if (isRPress) {
			if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				obj->state = SAttack;
				player.isSword = false;
				Fire(obj->x, obj->y, obj->angle, obj->isRight, SAttack);
				outAnimCnt = player.animator.animCnt;
				AnimChange(&player.animator, 5);
				player.animator.animCnt = outAnimCnt;
				outAnimCnt = 0.f;

				isRPress = false;
				isCharging = false;
				chargeTimer = 0;
				return;
			}
		}
	}
	else {
		// 左クリックされているかどうか
		if (IsKeyPress(VK_LBUTTON)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		if (isLPress) {
			if (IsKeyRelease(VK_LBUTTON) || !IsKeyPress(VK_LBUTTON)) {
				obj->state = SAttack;
				player.isSword = true;
				isLPress = false;
				isCharging = false;
				chargeTimer = 0;
				AnimChange(&player.animator, 6);
				return;
			}
		}
		// 右クリックされているかどうか
		if (IsKeyPress(VK_RBUTTON)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		if (isRPress) {
			if (IsKeyRelease(VK_RBUTTON) || !IsKeyPress(VK_RBUTTON)) {
				obj->state = SAttack;
				player.isSword = false;
				Fire(obj->x, obj->y, obj->angle, obj->isRight, SAttack);
				outAnimCnt = player.animator.animCnt;
				AnimChange(&player.animator, 5);
				player.animator.animCnt = outAnimCnt;
				outAnimCnt = 0.f;

				isRPress = false;
				isCharging = false;
				chargeTimer = 0;
				return;
			}
		}
	}

	if (obj->vx == 0 && obj->vy == 0)
		obj->state = SIdle;

	// チャージ判定
	if (isCharging) {
		chargeTimer += DELTA_TIME;

		if (chargeTimer > CHARGE_CHECK_TIME) {
			obj->state = SCharging;
			isCharging = false;
			chargeTimer = 0;
			return;
		}
	}
}

void PlayerAttack()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	float& cnt = player.animator.animCnt;
	ANIM_PARAM& param = player.animator.params[player.animator.animID];

	// 向き切り替え
	if (cosf(obj->angle) > 0)
		obj->isRight = true;	// 右
	else if (cosf(obj->angle) < 0)
		obj->isRight = false;	// 左

	// 剣
	if (player.isSword) {
		if (!IsSwordEffectActive())
			CreateSwordEffect(obj->x, obj->y, obj->angle);
		
		/* 攻撃判定 */
		if ((int)cnt == 1) {
			PlaySounds(SwordAttack);
			fan = { obj->x, obj->y, player.swordLength, player.swordAngle };

			for (int i = 0; i < GetMaxSpawn(); i++) {
				ENEMY* enemy = GetEnemies()[i];
				if (!enemy->obj.isActive || enemy->obj.state == SDamaged || enemy->obj.state == SDead) continue;

				FRECT e_rect = GetEnemyRect(enemy);
				if (IsHitRectWithFan(e_rect, fan, obj->angle))
					if (!CheckBetween(player.obj, enemy->obj))
						Damage(&enemy->obj, player.swordPower);
			}
		}

		if (cnt >= param.total - 1)
		{
			if (obj->vx == 0 && obj->vy == 0) obj->state = SIdle;
			else
			{
				obj->state = SMove;
				AnimChange(&player.animator, 1);
			}
		}
	}
	// 銃
	else {
		// 歩き
		if (obj->vx != 0 && obj->vy != 0) {
			float finishCnt = outAnimCnt + 2;		// outAnimCntから2コマ再生する
			if (finishCnt >= param.total - 1)
				finishCnt -= param.total - 1;

			if (cnt >= finishCnt) {
				outAnimCnt = player.animator.animCnt;
				obj->state = SMove;
				AnimChange(&player.animator, 1);
				player.animator.animCnt = outAnimCnt;
				outAnimCnt = 0.f;
			}
		}
		// 待機
		else {
			if (cnt >= param.total - 1)
				obj->state = SIdle;
		}
	}
}

void PlayerCharging()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	/*<<<Pad操作>>>*/
	if (IsPadConnect(0)) {
		// 剣
		if (player.isSword) {
			// LBボタンが押されている間チャージ
			if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				isLPress = true;
				isCharging = true;
			}

			// LBボタンが離された時、チャージ時間が一定時間経っていたらチャージ完了
			if (isLPress) {
				if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
					if (chargeTimer > CHARGING_TIME)
						obj->state = SChargeAttack;
					else
						obj->state = SIdle;

					isLPress = false;
					isCharging = false;
					chargeTimer = 0;
					player.hsv = hsv_default;
					return;
				}
			}
		}
		// 銃
		else {
			// RBボタンが押されている間チャージ
			if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				isRPress = true;
				isCharging = true;
			}

			// RBボタンが離された時、チャージ時間が一定時間経っていたらチャージ完了
			if (isRPress) {
				if (IsPadReleaseDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || !IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
					if (chargeTimer > CHARGING_TIME)
						obj->state = SChargeAttack;
					else
						obj->state = SIdle;

					isRPress = false;
					isCharging = false;
					chargeTimer = 0;
					player.hsv = hsv_default;
					return;
				}
			}
		}
	}
	/*<<<マウス& キーボード操作>>>*/
	else {
		// 剣
		if (player.isSword) {
			// 左クリックされている間チャージ
			if (IsKeyPress(VK_LBUTTON)) {
				isLPress = true;
				isCharging = true;
			}

			// 左クリックが離された時、チャージ時間が一定時間経っていたらチャージ完了
			if (isLPress) {
				if (IsKeyRelease(VK_LBUTTON) || !IsKeyPress(VK_LBUTTON)) {
					if (chargeTimer > CHARGING_TIME)
						obj->state = SChargeAttack;
					else
						obj->state = SIdle;

					isLPress = false;
					isCharging = false;
					chargeTimer = 0;
					player.hsv = hsv_default;
					return;
				}
			}
		}
		// 銃
		else {
			// 右クリックされている間チャージ
			if (IsKeyPress(VK_RBUTTON)) {
				isRPress = true;
				isCharging = true;
			}

			// 右クリックが離された時、チャージ時間が一定時間経っていたらチャージ完了
			if (isRPress) {
				if (IsKeyRelease(VK_RBUTTON) || !IsKeyPress(VK_RBUTTON)) {
					if (chargeTimer > CHARGING_TIME)
						obj->state = SChargeAttack;
					else
						obj->state = SIdle;

					isRPress = false;
					isCharging = false;
					chargeTimer = 0;
					player.hsv = hsv_default;
					return;
				}
			}
		}
	}

	// チャージ完了判定
	if (isCharging) {
		chargeTimer += DELTA_TIME;
		if (chargeTimer > CHARGING_TIME) {
			if (!isBlinking) {
				isBlinking = true;
				blink_timer = 0.f;
				isLight = true;
				player.hsv = { -1.f, 1.5f, 2.5f };
			}
			if(isBlinking)
				BlinkPlayer({ -1.f, 1.5f, 2.5f }, { -1.f, 1.5f, 1.3f });

			if (!player.isSword) {
				AnimChange(&player.animator, 8);
			}
			else {
				if (obj->vx == 0 && obj->vy == 0)
					AnimChange(&player.animator, 0);
				else
					AnimChange(&player.animator, 1);
			}
		}
		else {
			if (obj->vx == 0 && obj->vy == 0)
				AnimChange(&player.animator, 0);
			else
				AnimChange(&player.animator, 1);
		}
	}
}

void PlayerChargeAttack()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = { -1.f, 1.5f, 1.3f };
	isBlinking = false;

	// 向き切り替え
	if (cosf(obj->angle) > 0)
		obj->isRight = true;	// 右
	else if (cosf(obj->angle) < 0)
		obj->isRight = false;	// 左

	if (player.isSword) {
		if (!IsSwordEffectActive())
			CreateSwordEffect(obj->x, obj->y, obj->angle);

		AnimChange(&player.animator, 6);
		float& cnt = player.animator.animCnt;
		ANIM_PARAM& param = player.animator.params[player.animator.animID];

		/* 攻撃判定 */
		if (player.animator.animID == 6) {
			if ((int)cnt == 1) {
				PlaySounds(SwordAttack);
				fan = { obj->x, obj->y, player.cSwordLength, player.cSwordAngle };

				for (int i = 0; i < GetMaxSpawn(); i++) {
					ENEMY* enemy = GetEnemies()[i];
					if (!enemy->obj.isActive || enemy->obj.state == SDamaged || enemy->obj.state == SDead)
						continue;

				FRECT e_rect = GetEnemyRect(enemy);

					if (IsHitRectWithFan(e_rect, fan, obj->angle)) {
						if (!CheckBetween(player.obj, enemy->obj))
							Damage(&enemy->obj, player.cSwordPower);
					}
				}
			}
		}

		if (cnt >= param.total - 1)
		{
			obj->state = SIdle;
		}
	}
	else {
		AnimChange(&player.animator, 9);
		static bool isFired = false;
		float& cnt = player.animator.animCnt;
		ANIM_PARAM& param = player.animator.params[player.animator.animID];

		if ((int)cnt == param.total - 2 && isFired == false) {
			Fire(obj->x, obj->y, obj->angle, obj->isRight, SChargeAttack);
			isFired = true;
		}
		if (cnt >= param.total - 1) {
			isFired = false;
			obj->state = SIdle;
		}
	}
}

void PlayerAvoid()
{
	// 回避中は無敵にする
	invincibleTime = MAX_INVINCIBLE_TIME;

	AnimChange(&player.animator, 2);

	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	float& cnt = player.animator.animCnt;
	ANIM_PARAM& param = player.animator.params[player.animator.animID];
	if (cnt >= param.total - 1) {
		obj->state = SIdle;
		invincibleTime = 0.f;
		coolTime = MAX_COOL_TIME;	// クールタイム設定
	}
}

void PlayerDamaged()
{
	if (!PlayerIsInvincible())
		invincibleTime = MAX_INVINCIBLE_TIME;

	AnimChange(&player.animator, 3);
	player.hsv = hsv_default;

	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	float& cnt = player.animator.animCnt;
	ANIM_PARAM& param = player.animator.params[player.animator.animID];
	if (cnt >= param.total - 1)
	{
		obj->state = SIdle;
		chargeTimer = CHARGE_CHECK_TIME;
	}
}

void PlayerDead()
{
	OBJECT* obj = &player.obj;
	obj->vx = obj->vy = 0;
	obj->isActive = false;

	//float& cnt = player.animCnt;
	//ANIM_PARAM& param = player.params[player.animID];
	//if (cnt >= param.total - 1)
	//{
	//	obj->isActive = false;
	//}
}

void PlayerHitCheckX()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;
	const CAMERA* cam = GetCamera();

	/*<<<当たり判定>>>*/
	// プレイヤーの当たり矩形
	obj->hitRect = { obj->x - 14, obj->y, obj->x + 14, obj->y + 25 };

	// ステージ関連読み込み
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// 当たり判定処理軽減
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	if (x_start < 0) x_start = 0;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	if (y_start < 0) y_start = 0;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// ステージとの当たり判定
	for (int i = y_start; i < y_end; i++)
	{
		for (int j = x_start; j < x_end; j++)
		{
			if (hitTable[stage[j + i * STAGE_SIZE_X]] != 0)
			{
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile = { left, top, left + TILE_SIZE, top + TILE_SIZE };
				if (obj->vx > 0)	// 右
				{
					if (IsHitRight(&obj->hitRect, &tile))
					{
						obj->x = tile.left - (obj->hitRect.right - obj->x);
						obj->vx = 0;
					}
				}
				if (obj->vx < 0)	// 左
				{
					if (IsHitLeft(&obj->hitRect, &tile))
					{
						obj->x = tile.right + (obj->x - obj->hitRect.left);
						obj->vx = 0;
					}
				}
			}
		}
	}
}

void PlayerHitCheckY()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;
	const CAMERA* cam = GetCamera();

	/*<<<当たり判定>>>*/
	// プレイヤーの当たり矩形
	obj->hitRect = { obj->x - 14, obj->y, obj->x + 14, obj->y + 25 };

	// ステージ関連読み込み
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// 当たり判定処理軽減
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	if (x_start < 0) x_start = 0;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	if (y_start < 0) y_start = 0;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// ステージとの当たり判定
	for (int i = y_start; i < y_end; i++)
	{
		for (int j = x_start; j < x_end; j++)
		{
			if (hitTable[stage[j + i * STAGE_SIZE_X]] != 0)
			{
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile = { left, top, left + TILE_SIZE, top + TILE_SIZE };
				if (obj->vy < 0)	// 上
				{
					if (IsHitTop(&obj->hitRect, &tile))
					{
						obj->y = tile.bottom + (obj->y - obj->hitRect.top);
						obj->vy = 0;
					}
				}
				if (obj->vy > 0)	// 下
				{
					if (IsHitBottom(&obj->hitRect, &tile))
					{
						obj->y = tile.top - (obj->hitRect.bottom - obj->y);
						obj->vy = 0;
					}
				}
			}
		}
	}
}

void Enhance(ENERGY_PARAM* energyParam)
{
	int level = GetGameManager()->energyLevel;

	player.swordPower = energyParam[level - 1].swordPower;
	player.swordLength = energyParam[level - 1].swordLength;
	player.swordAngle = energyParam[level - 1].swordAngle;
	player.gunPower = energyParam[level - 1].gunPower;
	player.gunScale = energyParam[level - 1].gunScale;
	player.cSwordPower = energyParam[level].swordPower;
	player.cSwordLength = energyParam[level].swordLength;
	player.cSwordAngle = energyParam[level].swordAngle;
	player.cGunPower = energyParam[level - 1].cGunPower;
}

void PlayerMoveInput()
{
	/*<<<関数使用準備>>>*/
	OBJECT* obj = &player.obj;

	/*<<<プレイヤー操作用変数>>>*/
	float axisX = 0;
	float axisY = 0;
	float axisLength = 0;
	float angleX = 0;
	float angleY = 0;
	float angleLength = 0;
	static float prev_angle = 0.f;

	/*<<<Pad操作>>>*/
	if (IsPadConnect(0))
	{
		// 左スティック
		axisX = GetPadLeftStickX(0);
		axisY = -GetPadLeftStickY(0);
		// 右スティック
		angleX = GetPadRightStickX(0);
		angleY = -GetPadRightStickY(0);
	}
	else
	{
		/*<<<マウス& キーボード操作>>>*/
		if (IsKeyPress('D')) axisX = 1.0;
		if (IsKeyPress('A')) axisX = -1.0;
		if (IsKeyPress('S')) axisY = 1.0;
		if (IsKeyPress('W')) axisY = -1.0;

		// マウス位置取得
		int mx, my;
		RECT rect;
		GetMousePosition(&mx, &my);
		GetClientRect(GetWindowHandle(), &rect);
		angleX = mx - (rect.right + rect.left) / 2;
		angleY = my - (rect.bottom + rect.top) / 2;
	}


	/*<<<プレイヤー移動>>>*/
	// 入力補正
	axisLength = sqrtf(axisX * axisX + axisY * axisY);
	if (axisLength > 1.0f)
	{
		axisX /= axisLength;
		axisY /= axisLength;
	}
	angleLength = sqrtf(angleX * angleX + angleY * angleY);
	if (angleLength > 1.0f)
	{
		angleX /= angleLength;
		angleY /= angleLength;
	}

	if (axisX != 0 || axisY != 0) {
		prev_angle = atan2f(axisY, axisX);
	}

	// 速度更新
	switch (obj->state) {
	case SCharging:
		obj->vx = player.speed2 * axisX;
		obj->vy = player.speed2 * axisY;
		break;
	case SChargeAttack:
		obj->vx = player.speed3 * axisX;
		obj->vy = player.speed3 * axisY;
		break;
	case SAvoid:
		obj->vx = (obj->speed + AVOID_DIST) * cosf(prev_angle);
		obj->vy = (obj->speed + AVOID_DIST) * sinf(prev_angle);
		break;
	default:
		obj->vx = obj->speed * axisX;
		obj->vy = obj->speed * axisY;
		break;
	}

	// 向き切り替え
	if (obj->vx > 0) obj->isRight = true;	// 右
	if (obj->vx < 0) obj->isRight = false;	// 左
	// 射角取得
	//angleY *= -1;
	if (angleX != 0 || angleY != 0) obj->angle = atan2f(angleY, angleX);

	/*<<<ステート切り替え>>>*/
	if (obj->state == SIdle)
	{
		if (axisX != 0 || axisY != 0) obj->state = SMove;
	}
}

void Damage2Player(PLAYER* player, int damage)
{
	OBJECT* obj = &player->obj;

	AddDamageUI(obj->x, obj->y, damage);
	if (player->shield > 0) {
		PlaySounds(ShieldDamage);
		player->shield -= damage;

		if (player->shield < 0) {
			PlaySounds(ShieldBreak);
			damage = -player->shield;	// ダメージを超過分のダメージに更新
			player->shield = 0;			// シールドの耐久値を0にする
		}
		else {
			damage = 0;
		}
	}
	else PlaySounds(PlayerDamage);

	obj->hp -= damage;

	if (obj->hp <= 0)
		obj->state = SDead;
	else
		obj->state = SDamaged;

	ResetRecovery();
}

void BlinkPlayer(const HSV_PARAM& hsv_light, const HSV_PARAM& hsv_dark)
{
	const float BLINK_INTERVAL = 0.1f;

	if (isLight) {
		if (blink_timer > BLINK_INTERVAL) {
			blink_timer = 0.f;
			isLight = false;
			player.hsv = hsv_dark;
		}
	}
	else {
		if (blink_timer > BLINK_INTERVAL) {
			blink_timer = 0.f;
			isLight = true;
			player.hsv = hsv_light;
		}
	}

	blink_timer += DELTA_TIME;
}
