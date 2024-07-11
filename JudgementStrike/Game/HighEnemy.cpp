#include "UnlimitedLib/UnlimitedLib.h"
#include "EnemyManager.h"
#include "Player.h"
#include "HitCheck.h"
#include <math.h>

// �U������(�~�Ƌ�`)
static CIRCLE at_circle;
static FRECT at_rect;

void HighEnemy_Initialize()
{
}

void HighEnemy_Finalize()
{
}

void HighEnemy_Update()
{
}

void HighEnemy_Render()
{
}

void HighEnemyIdle(ENEMY* enemy)
{
	AnimChange(&enemy->animator, 0);

	OBJECT* obj = &enemy->obj;
	obj->vx = obj->vy = 0;

	if (GetGameState() == GPlay) {
		// �v���C���[�ƈ�苗���ȉ��ɂȂ�����
		if (GetDistance(obj, &GetPlayer()->obj) < 70) {
			obj->state = SAttack;	// Attack��
			AnimChange(&enemy->animator, 2);
		}
		else {
			obj->state = SMove;	// Move��
		}
	}
}

void HighEnemyMove(ENEMY* enemy)
{
}

void HighEnemyAttack(ENEMY* enemy)
{
	OBJECT* obj = &enemy->obj;
	obj->vx = obj->vy = 0;

	float& cnt = enemy->animator.animCnt;
	ANIM_PARAM& param = enemy->animator.params[enemy->animator.animID];
	PLAYER* player = GetPlayer();

	/* �U������ */
	if (player->obj.isActive && player->obj.state != SDamaged && player->obj.state != SDead) {
		if (!PlayerIsInvincible()) {
			FRECT p_rect = GetPlayerRect();
			int i = obj->isRight ? 1 : -1;	// ���E�Ή�

			// �t���[�����ƂɍU�������ς���
			if (enemy->animator.animID == 2) {
				if ((int)cnt == 5) {
					at_circle = { obj->x + 1 * i, obj->y - 2, 33 };
					if (IsHitRectWithCircle(p_rect, at_circle))
						if (!CheckBetween(*obj, player->obj))
							Damage2Player(player, enemy->power);
				}
				else if ((int)cnt == 7) {
					at_circle = { obj->x + 8 * i, obj->y - 49, 33 };
					if (IsHitRectWithCircle(p_rect, at_circle))
						if (!CheckBetween(*obj, player->obj))
							Damage2Player(player, enemy->power);
				}
				else if ((int)cnt == 12) {
					at_rect = { obj->x + 28 * i, obj->y - 49, obj->x + 81 * i, obj->y + 22 };
					if (i < 0) Swap(at_rect.left, at_rect.right);
					if (IsHit(&p_rect, &at_rect))
						if (!CheckBetween(*obj, player->obj))
							Damage2Player(player, enemy->power);
				}
			}
			else if (enemy->animator.animID == 3) {
				if ((int)cnt == 6) {
					at_rect = { obj->x + 8 * i, obj->y - 30, obj->x + 81 * i, obj->y + 22 };
					if (i < 0) Swap(at_rect.left, at_rect.right);
					if (IsHit(&p_rect, &at_rect))
						if (!CheckBetween(*obj, player->obj))
							Damage2Player(player, enemy->power);
				}
			}
			else if (enemy->animator.animID == 4) {
				if ((int)cnt == 6) {
					at_rect = { obj->x + 9 * i, obj->y - 29, obj->x + 71 * i, obj->y + 8 };
					if (i < 0) Swap(at_rect.left, at_rect.right);
					if (IsHit(&p_rect, &at_rect))
						if (!CheckBetween(*obj, player->obj))
							Damage2Player(player, enemy->power);
				}
			}
		}
	}

	// �A�j���[�V�����J�ڏ���
	if (cnt >= param.total - 1)
	{
		if (enemy->animator.animID == 2)
			AnimChange(&enemy->animator, 3);
		else if (enemy->animator.animID == 3)
			AnimChange(&enemy->animator, 4);
		else if (enemy->animator.animID == 4) {
			// �v���C���[�ƈ�苗���ȏ�ɂȂ�����
			if (GetDistance(obj, &player->obj) >= 70)
				obj->state = SMove;	// Move��
			else
				obj->state = SIdle;	// ����Ă��Ȃ�������Idle��

			// �U�����I���ƃv���C���[�̕��Ɍ�������
			float angle = atan2f(player->obj.y - obj->y, player->obj.x - obj->x);
			if (cosf(angle) > 0)
				obj->isRight = true;
			else
				obj->isRight = false;
		}
	}
}
