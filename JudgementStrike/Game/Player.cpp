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

float outAnimCnt;	// �����������e�U���̃A�j���[�V�����J�ڗp�ϐ�

static bool isLPress;		// ���U���{�^�����̓t���O
static bool isRPress;		// �e�U���{�^�����̓t���O

float CHARGE_CHECK_TIME;	// �`���[�W�U���Ɣ��肳��鎞��
float CHARGING_TIME;		// �`���[�W�ɂ����鎞��
float chargeTimer;			// �`���[�W�֘A�̃^�C�}�[
bool isCharging;			// �`���[�W���t���O

float invincibleTime;		// ���G����
float MAX_INVINCIBLE_TIME;	// ���G���Ԃ̍ő�l

float coolTime;			// ����̃N�[���^�C��
float MAX_COOL_TIME;	// ����̃N�[���^�C���̍ő�l
float AVOID_DIST;		// ����œ�������

static float blink_timer;	// ���Ŏ���
static bool isBlinking;		// ���Ńt���O
static bool isLight;		// ���Ńt���O(�����Ă��邩�ǂ���)

ANIM_PARAM pAnimParam[] = {
	{ NULL, 4    , 10, true  },		// 0. ����/����
	{ NULL, 6    , 10, true  },		// 1. �ړ�
	{ NULL, 3 + 1, 10, false },		// 2. ���
	{ NULL, 3 + 1, 10, false },		// 3. ��_���[�W
	{ NULL, 2 + 1, 10, false },		// 4. �e�U��(�ҋ@)
	{ NULL, 6 + 1, 10, false },		// 5. �e�U��(�ړ����Ȃ���)
	{ NULL, 5 + 1, 20, false },		// 6. ���U��
	{ NULL, 6 + 1, 10, false },		// 7. �`���[�W�U��(�e)
	{ NULL, 2	 , 10, true  },		// 8. �`���[�W����(�e)
	{ NULL, 4	 , 10, false },		// 9. �`���[�W����(�e)
};

static int* pSpritePool;	// �A�j���[�V�����X�v���C�g�v�[��
int pArrow;	// �U�����������p�}�[�J�[

// �U������p
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
	// �������m�ہ�����
	if (pSpritePool == NULL) {
		int size = 0;
		for (int i = 0; i <= 9; i++)
			size += pAnimParam[i].total;

		// �������m�ہ��[��������
		pSpritePool = (int*)malloc(sizeof(int) * size);
		ZeroMemory(pSpritePool, sizeof(int) * size);

		// ����������
		for (int i = 0; i <= 9; i++) {
			if (i == 0)
				pAnimParam[i].sprites = pSpritePool;
			else
				pAnimParam[i].sprites = pAnimParam[i - 1].sprites + pAnimParam[i - 1].total;
		}
	}

	CreateSpriteMatrix("Assets/Sprites/Player/idle-spritesheet.png", 64, 64, 4, 1, 4, pAnimParam[0].sprites);			// ����
	CreateSpriteMatrix("Assets/Sprites/Player/move-spritesheet.png", 64, 64, 6, 1, 6, pAnimParam[1].sprites);			// �ړ�
	CreateSpriteMatrix("Assets/Sprites/Player/jump-spritesheet.png", 64, 64, 3, 1, 3, pAnimParam[2].sprites);			// ���
	CreateSpriteMatrix("Assets/Sprites/Player/damage-spritesheet.png", 64, 64, 3, 1, 3, pAnimParam[3].sprites);			// ��_���[�W
	CreateSpriteMatrix("Assets/Sprites/Player/robot-shoot-spritesheet.png", 64, 64, 2, 1, 2, pAnimParam[4].sprites);	// �e�U��(�ҋ@)
	CreateSpriteMatrix("Assets/Sprites/Player/moveshoot-spritesheet.png", 64, 64, 6, 1, 6, pAnimParam[5].sprites);		// �e�U��(�ړ����Ȃ���)
	CreateSpriteMatrix("Assets/Sprites/Player/beamsword-spritesheet.png", 84, 80, 5, 1, 5, pAnimParam[6].sprites);		// ���U��
	CreateSpriteMatrix("Assets/Sprites/Player/chargeshot-spritesheet.png", 80, 64, 6, 1, 6, pAnimParam[7].sprites);		// �`���[�W�U��(�e)
	// �`���[�W����(�e)
	pAnimParam[8].sprites[0] = pAnimParam[7].sprites[3];
	pAnimParam[8].sprites[1] = pAnimParam[7].sprites[4];
	// �`���[�W����(�e)
	//pAnimParam[9].sprites[0] = pAnimParam[7].sprites[1];
	//pAnimParam[9].sprites[1] = pAnimParam[7].sprites[2];
	pAnimParam[9].sprites[0] = pAnimParam[7].sprites[3];
	pAnimParam[9].sprites[1] = pAnimParam[7].sprites[4];
	pAnimParam[9].sprites[2] = pAnimParam[7].sprites[5];
	pAnimParam[9].sprites[3] = pAnimParam[7].sprites[5];	// �킴�Ɠ����R�}���Q�R�}�����

	pArrow = CreateSprite("Assets/Sprites/Player/ingame_arrow.png");

	// �A�j���[�V�����̊Ԋu�𓯂��ɂ��邽��
	for (int i = 2; i < 8; i++)
	{
		pAnimParam[i].sprites[pAnimParam[i].total - 1] = pAnimParam[i].sprites[pAnimParam[i].total - 2];
	}

	// �X�v���C�g�̒��S�ʒu���m��
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

	// ���������
	free(pSpritePool);
	pSpritePool = NULL;
}

void Player_Update()
{
	if (GetGameState() == GPlay) {
		/*<<<�֐��g�p����>>>*/
		OBJECT* obj = &player.obj;
		if (!obj->isActive) return;

		/*�ړ�&�U���̌����̓���*/
		PlayerMoveInput();

		// ���
		if (coolTime <= 0.f && obj->state != SChargeAttack && obj->state != SDamaged && obj->state != SDead) {
			if ((IsKeyPress(VK_SPACE) || IsPadPressDirect(0, XINPUT_GAMEPAD_A))) {
				obj->state = SAvoid;
				player.hsv = hsv_default;
				isLPress = isRPress = isCharging = false;
				chargeTimer = 0;
			}
		}

		/*<<<��Ԑ؂�ւ�>>>*/
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

		/*�ʒu�X�V*/
		obj->x += obj->vx * DELTA_TIME;
		PlayerHitCheckX();
		obj->y += obj->vy * DELTA_TIME;
		PlayerHitCheckY();

		// �G�Ƃ̓����蔻��
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

	// ���G����
	if (PlayerIsInvincible())
		invincibleTime -= DELTA_TIME;

	// �N�[���^�C������
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
	
	// ���G�e�X�g
	if (PlayerIsInvincible())
		SetSpriteColor(player.obj.sprite, D3DCOLOR_RGBA(255, 255, 255, 128));
	else
		SetSpriteColor(player.obj.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	// �`�揈��
	BeginCustomShader(GetChangeHSVShader());
	// HSV�ύX
	SetHSVParam(player.hsv.hue, player.hsv.saturation, player.hsv.value);
	SetCustomShaderParam(GetHSVParam(), sizeof(*GetHSVParam()));
	if (obj->isRight == true) RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, 1.f, 1.f);
	else RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, -1.f, 1.f);
	EndCustomShader();

	RenderSpriteRot(pArrow, -38.f, 17.f, obj->x - cam->x, obj->y - cam->y, obj->angle, 1.f);

	//// �U���͈̓e�X�g
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

	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = hsv_default;

	/*<<<Pad����>>>*/
	if (IsPadConnect(0)) {
		// LB�{�^����������Ă��邩�ǂ���
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		// LB�{�^���������ꂽ���ǂ���
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
		// RB�{�^����������Ă��邩�ǂ���
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		// RB�{�^���������ꂽ���ǂ���
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
	/*<<<�}�E�X& �L�[�{�[�h����>>>*/
	else {
		// ���N���b�N����Ă��邩�ǂ���
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
		// �E�N���b�N����Ă��邩�ǂ���
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

	// �`���[�W����
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

	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = hsv_default;

	/*<<<Pad����>>>*/
	if (IsPadConnect(0)) {
		// LB�{�^����������Ă��邩�ǂ���
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			player.isSword = true;
			isLPress = true;
			isCharging = true;
		}
		// LB�{�^���������ꂽ���ǂ���
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
		// RB�{�^����������Ă��邩�ǂ���
		if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			player.isSword = false;
			isRPress = true;
			isCharging = true;
		}
		// RB�{�^���������ꂽ���ǂ���
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
		// ���N���b�N����Ă��邩�ǂ���
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
		// �E�N���b�N����Ă��邩�ǂ���
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

	// �`���[�W����
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
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	float& cnt = player.animator.animCnt;
	ANIM_PARAM& param = player.animator.params[player.animator.animID];

	// �����؂�ւ�
	if (cosf(obj->angle) > 0)
		obj->isRight = true;	// �E
	else if (cosf(obj->angle) < 0)
		obj->isRight = false;	// ��

	// ��
	if (player.isSword) {
		if (!IsSwordEffectActive())
			CreateSwordEffect(obj->x, obj->y, obj->angle);
		
		/* �U������ */
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
	// �e
	else {
		// ����
		if (obj->vx != 0 && obj->vy != 0) {
			float finishCnt = outAnimCnt + 2;		// outAnimCnt����2�R�}�Đ�����
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
		// �ҋ@
		else {
			if (cnt >= param.total - 1)
				obj->state = SIdle;
		}
	}
}

void PlayerCharging()
{
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	/*<<<Pad����>>>*/
	if (IsPadConnect(0)) {
		// ��
		if (player.isSword) {
			// LB�{�^����������Ă���ԃ`���[�W
			if (IsPadPressDirect(0, XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				isLPress = true;
				isCharging = true;
			}

			// LB�{�^���������ꂽ���A�`���[�W���Ԃ���莞�Ԍo���Ă�����`���[�W����
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
		// �e
		else {
			// RB�{�^����������Ă���ԃ`���[�W
			if (IsPadPressDirect(0, XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				isRPress = true;
				isCharging = true;
			}

			// RB�{�^���������ꂽ���A�`���[�W���Ԃ���莞�Ԍo���Ă�����`���[�W����
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
	/*<<<�}�E�X& �L�[�{�[�h����>>>*/
	else {
		// ��
		if (player.isSword) {
			// ���N���b�N����Ă���ԃ`���[�W
			if (IsKeyPress(VK_LBUTTON)) {
				isLPress = true;
				isCharging = true;
			}

			// ���N���b�N�������ꂽ���A�`���[�W���Ԃ���莞�Ԍo���Ă�����`���[�W����
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
		// �e
		else {
			// �E�N���b�N����Ă���ԃ`���[�W
			if (IsKeyPress(VK_RBUTTON)) {
				isRPress = true;
				isCharging = true;
			}

			// �E�N���b�N�������ꂽ���A�`���[�W���Ԃ���莞�Ԍo���Ă�����`���[�W����
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

	// �`���[�W��������
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
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	player.hsv = { -1.f, 1.5f, 1.3f };
	isBlinking = false;

	// �����؂�ւ�
	if (cosf(obj->angle) > 0)
		obj->isRight = true;	// �E
	else if (cosf(obj->angle) < 0)
		obj->isRight = false;	// ��

	if (player.isSword) {
		if (!IsSwordEffectActive())
			CreateSwordEffect(obj->x, obj->y, obj->angle);

		AnimChange(&player.animator, 6);
		float& cnt = player.animator.animCnt;
		ANIM_PARAM& param = player.animator.params[player.animator.animID];

		/* �U������ */
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
	// ��𒆂͖��G�ɂ���
	invincibleTime = MAX_INVINCIBLE_TIME;

	AnimChange(&player.animator, 2);

	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	float& cnt = player.animator.animCnt;
	ANIM_PARAM& param = player.animator.params[player.animator.animID];
	if (cnt >= param.total - 1) {
		obj->state = SIdle;
		invincibleTime = 0.f;
		coolTime = MAX_COOL_TIME;	// �N�[���^�C���ݒ�
	}
}

void PlayerDamaged()
{
	if (!PlayerIsInvincible())
		invincibleTime = MAX_INVINCIBLE_TIME;

	AnimChange(&player.animator, 3);
	player.hsv = hsv_default;

	/*<<<�֐��g�p����>>>*/
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
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;
	const CAMERA* cam = GetCamera();

	/*<<<�����蔻��>>>*/
	// �v���C���[�̓������`
	obj->hitRect = { obj->x - 14, obj->y, obj->x + 14, obj->y + 25 };

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// �����蔻�菈���y��
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	if (x_start < 0) x_start = 0;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	if (y_start < 0) y_start = 0;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// �X�e�[�W�Ƃ̓����蔻��
	for (int i = y_start; i < y_end; i++)
	{
		for (int j = x_start; j < x_end; j++)
		{
			if (hitTable[stage[j + i * STAGE_SIZE_X]] != 0)
			{
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile = { left, top, left + TILE_SIZE, top + TILE_SIZE };
				if (obj->vx > 0)	// �E
				{
					if (IsHitRight(&obj->hitRect, &tile))
					{
						obj->x = tile.left - (obj->hitRect.right - obj->x);
						obj->vx = 0;
					}
				}
				if (obj->vx < 0)	// ��
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
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;
	const CAMERA* cam = GetCamera();

	/*<<<�����蔻��>>>*/
	// �v���C���[�̓������`
	obj->hitRect = { obj->x - 14, obj->y, obj->x + 14, obj->y + 25 };

	// �X�e�[�W�֘A�ǂݍ���
	const int* stage = GetStage();
	const int* hitTable = GetHitTable();

	// �����蔻�菈���y��
	int x_start = (obj->x - TILE_SIZE * 2) / TILE_SIZE;
	if (x_start < 0) x_start = 0;
	int x_end = (obj->x + TILE_SIZE * 2) / TILE_SIZE;
	if (x_end > STAGE_SIZE_X) x_end = STAGE_SIZE_X;
	int y_start = (obj->y - TILE_SIZE * 2) / TILE_SIZE;
	if (y_start < 0) y_start = 0;
	int y_end = (obj->y + TILE_SIZE * 2) / TILE_SIZE;
	if (y_end > STAGE_SIZE_Y) y_end = STAGE_SIZE_Y;

	// �X�e�[�W�Ƃ̓����蔻��
	for (int i = y_start; i < y_end; i++)
	{
		for (int j = x_start; j < x_end; j++)
		{
			if (hitTable[stage[j + i * STAGE_SIZE_X]] != 0)
			{
				int left = j * TILE_SIZE;
				int top = i * TILE_SIZE;
				FRECT tile = { left, top, left + TILE_SIZE, top + TILE_SIZE };
				if (obj->vy < 0)	// ��
				{
					if (IsHitTop(&obj->hitRect, &tile))
					{
						obj->y = tile.bottom + (obj->y - obj->hitRect.top);
						obj->vy = 0;
					}
				}
				if (obj->vy > 0)	// ��
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
	/*<<<�֐��g�p����>>>*/
	OBJECT* obj = &player.obj;

	/*<<<�v���C���[����p�ϐ�>>>*/
	float axisX = 0;
	float axisY = 0;
	float axisLength = 0;
	float angleX = 0;
	float angleY = 0;
	float angleLength = 0;
	static float prev_angle = 0.f;

	/*<<<Pad����>>>*/
	if (IsPadConnect(0))
	{
		// ���X�e�B�b�N
		axisX = GetPadLeftStickX(0);
		axisY = -GetPadLeftStickY(0);
		// �E�X�e�B�b�N
		angleX = GetPadRightStickX(0);
		angleY = -GetPadRightStickY(0);
	}
	else
	{
		/*<<<�}�E�X& �L�[�{�[�h����>>>*/
		if (IsKeyPress('D')) axisX = 1.0;
		if (IsKeyPress('A')) axisX = -1.0;
		if (IsKeyPress('S')) axisY = 1.0;
		if (IsKeyPress('W')) axisY = -1.0;

		// �}�E�X�ʒu�擾
		int mx, my;
		RECT rect;
		GetMousePosition(&mx, &my);
		GetClientRect(GetWindowHandle(), &rect);
		angleX = mx - (rect.right + rect.left) / 2;
		angleY = my - (rect.bottom + rect.top) / 2;
	}


	/*<<<�v���C���[�ړ�>>>*/
	// ���͕␳
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

	// ���x�X�V
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

	// �����؂�ւ�
	if (obj->vx > 0) obj->isRight = true;	// �E
	if (obj->vx < 0) obj->isRight = false;	// ��
	// �ˊp�擾
	//angleY *= -1;
	if (angleX != 0 || angleY != 0) obj->angle = atan2f(angleY, angleX);

	/*<<<�X�e�[�g�؂�ւ�>>>*/
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
			damage = -player->shield;	// �_���[�W�𒴉ߕ��̃_���[�W�ɍX�V
			player->shield = 0;			// �V�[���h�̑ϋv�l��0�ɂ���
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
