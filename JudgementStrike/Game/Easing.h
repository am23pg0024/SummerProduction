#pragma once

float NoEasing(int time, float min, float max, float totalTime);				// �C�[�W���O�Ȃ�
float EaseAppearInQuad(float time, float min, float max, float totalTime);		// ����(���)���ďo��
float EaseAppearInCubic(float time, float min, float max, float totalTime);		// ����(�O��)���ďo��
float EaseAppearInQuart(float time, float min, float max, float totalTime);		// ����(�l��)���ďo��
float EaseAppearOutQuad(float time, float min, float max, float totalTime);		// ����(���)���ďo��
float EaseDisappearInQuad(float time, float min, float max, float totalTime);	// ����(���)���ď���
float EaseDisappearInCubic(float time, float min, float max, float totalTime);	// ����(�O��)���ď���
float EaseDisappearInQuart(float time, float min, float max, float totalTime);	// ����(�l��)���ď���
float EaseDisappearOutQuad(float time, float min, float max, float totalTime);	// ����(���)���ď���
float EaseAppearInOutPop(float time, float min, float max, float totalTime);	// ����������(���)