#pragma once
#include "Game.h"

// �A�j���[�V�������
struct ANIM_PARAM {
	int* sprites;	// �A�j���[�V�����̊e�R�}
	int total;		// ���X�v���C�g��
	int fps;		// �t���[�����[�g
	bool loop;		// ���[�v���邩�ǂ���
};

// �A�j���[�^�[(OBJECT�n�\���̂Ɏ�������R���|�[�l���g)
struct ANIMATOR {
	OBJECT* owner;		// �N�̃A�j���[�^�[��
	ANIM_PARAM* params;	// �A�j���[�V�������

	int animID;		// �A�j���[�V�����ԍ�
	float animCnt;	// �A�j���[�V�����J�E���g
};

struct ANIM_OBJECT {
	OBJECT obj;
	ANIMATOR animator;
};

void Animate(ANIMATOR* animator);
void AnimChange(ANIMATOR* animator, int id);
