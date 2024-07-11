#include "Animation.h"

// アニメーションを再生する
void Animate(ANIMATOR* animator)
{
	float& cnt = animator->animCnt;
	ANIM_PARAM& param = animator->params[animator->animID];

	cnt += param.fps * DELTA_TIME;
	if (cnt >= param.total) {
		if (param.loop)
			cnt = 0;
		else
			cnt = param.total - 1;
	}

	animator->owner->sprite= param.sprites[(int)cnt];
}

// アニメーションを切り替える
void AnimChange(ANIMATOR* animator, int id)
{
	if (id == animator->animID) return;

	animator->animID = id;
	animator->animCnt = 0;
}
