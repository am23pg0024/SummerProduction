#pragma once
#include "Game.h"

// アニメーション情報
struct ANIM_PARAM {
	int* sprites;	// アニメーションの各コマ
	int total;		// 総スプライト数
	int fps;		// フレームレート
	bool loop;		// ループするかどうか
};

// アニメーター(OBJECT系構造体に持たせるコンポーネント)
struct ANIMATOR {
	OBJECT* owner;		// 誰のアニメーターか
	ANIM_PARAM* params;	// アニメーション情報

	int animID;		// アニメーション番号
	float animCnt;	// アニメーションカウント
};

struct ANIM_OBJECT {
	OBJECT obj;
	ANIMATOR animator;
};

void Animate(ANIMATOR* animator);
void AnimChange(ANIMATOR* animator, int id);
