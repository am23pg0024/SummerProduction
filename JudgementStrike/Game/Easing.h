#pragma once

float NoEasing(int time, float min, float max, float totalTime);				// イージングなし
float EaseAppearInQuad(float time, float min, float max, float totalTime);		// 加速(二乗)して出現
float EaseAppearInCubic(float time, float min, float max, float totalTime);		// 加速(三乗)して出現
float EaseAppearInQuart(float time, float min, float max, float totalTime);		// 加速(四乗)して出現
float EaseAppearOutQuad(float time, float min, float max, float totalTime);		// 減速(二乗)して出現
float EaseDisappearInQuad(float time, float min, float max, float totalTime);	// 加速(二乗)して消滅
float EaseDisappearInCubic(float time, float min, float max, float totalTime);	// 加速(三乗)して消滅
float EaseDisappearInQuart(float time, float min, float max, float totalTime);	// 加速(四乗)して消滅
float EaseDisappearOutQuad(float time, float min, float max, float totalTime);	// 減速(二乗)して消滅
float EaseAppearInOutPop(float time, float min, float max, float totalTime);	// 加速→減速(二乗)