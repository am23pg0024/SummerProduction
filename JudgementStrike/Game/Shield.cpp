#include "UnlimitedLib/UnlimitedLib.h"
#include "Game.h"
#include "Animation.h"
#include "2DHelperEx.h"
#include "Player.h"
#include "Camera.h"
#include "ui.h"
#include "Parameter.h"
#include "Shield.h"

ANIM_OBJECT shield;
float recoveryTimer;		// �񕜃^�C�}�[
float recoveryStartTime;	// �񕜂��n�܂�܂ł̎���
float recoveryInterval;		// �񕜂���C���^�[�o��
int recoveryNum;			// ��x�ɉ񕜂���V�[���h��
bool isRecovering;			// �񕜒��t���O

ANIM_PARAM sAnimParam = { NULL, 8, 10, true };

static int* sSpritePool;	// �A�j���[�V�����X�v���C�g�v�[��

void RecoverShield();

void ResetRecovery() {
	recoveryTimer = 0.f;
	isRecovering = false;
}

void Shield_Restart()
{
	shield.obj.x = shield.obj.y = 0;

	const PARAM* param = GetParameter();
	recoveryTimer = 0.f;
	recoveryStartTime = param->shieldRecovery.startTime;
	recoveryInterval = param->shieldRecovery.interval;
	recoveryNum = param->shieldRecovery.num;
	isRecovering = false;
}

void Shield_Initialize()
{
	// �������m��
	sAnimParam.sprites = (int*)malloc(sizeof(int) * sAnimParam.total);

	CreateSpriteMatrix("Assets/Sprites/player/shield-spritesheet.png", 96, 96, 8, 1, 8, sAnimParam.sprites);			// �V�[���h

	// �V�[���h�̒��S�ݒ�
	for (int i = 0; i < sAnimParam.total; i++)
	{
		SetSpriteOrigin(sAnimParam.sprites[i], 48, 48);
	}

	shield.obj.x = shield.obj.y = 0;
	shield.animator.owner = &shield.obj;
	shield.animator.params = &sAnimParam;
	shield.animator.animID = 0;
	shield.animator.animCnt = 0;

	const PARAM* param = GetParameter();
	recoveryTimer = 0.f;
	recoveryStartTime = param->shieldRecovery.startTime;
	recoveryInterval = param->shieldRecovery.interval;
	recoveryNum = param->shieldRecovery.num;
	isRecovering = false;
}

void Shield_Finalize()
{

	ReleaseSpriteMatrix(sAnimParam.sprites, sAnimParam.total);

	// ���������
	free(sSpritePool);
	sSpritePool = NULL;
}

void Shield_Update()
{
	OBJECT* obj = &shield.obj;
	const PLAYER* player = GetPlayer();

	if (!player->obj.isActive) return;

	obj->x = player->obj.x;
	obj->y = player->obj.y;

	if (GetGameState() == GPlay)
		RecoverShield();

	Animate(&shield.animator);
}

void Shield_Render()
{
	OBJECT* obj = &shield.obj;

	if (!GetPlayer()->obj.isActive) return;

	const Sprite* sprite = GetSprite(shield.animator.params[shield.animator.animID].sprites[0]);
	const CAMERA* cam = GetCamera();

	if (GetPlayer()->shield > 0)
		RenderSpriteEx(obj->sprite, sprite->cx, sprite->cy, obj->x - cam->x, obj->y - cam->y, 0, 1.f, 1.f);
}


void RecoverShield()
{
	int& shield = GetPlayer()->shield;

	if (shield < 35) {
		// �񕜒��łȂ���
		if (!isRecovering && recoveryTimer >= recoveryStartTime)
			isRecovering = true;

		// �񕜒��̎�
		if (isRecovering && recoveryTimer >= recoveryInterval) {
			shield += recoveryNum;
			if (shield >= 35) {
				shield = 35;
				isRecovering = false;
			}
			recoveryTimer = 0.f;
		}

		recoveryTimer += DELTA_TIME;
	}
}