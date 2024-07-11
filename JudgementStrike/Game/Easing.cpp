#include "UnlimitedLib/UnlimitedLib.h"
#include "Easing.h"
#include <math.h>

float NoEasing(int time, float min, float max, float totalTime)	// イージングなし
{
	if (time < totalTime)
	{	
		return max * time / totalTime + min;
	}
	else return max;
}

float EaseAppearInQuad(float time, float min, float max, float totalTime)	// 加速(二乗)
{
	if (time < totalTime)
	{
		max -= min;
		time /= totalTime;
		return max * time * time + min;
	}
	else return max; 
}

float EaseAppearInCubic(float time, float min, float max, float totalTime)	// 加速(三乗)
{
	if (time < totalTime)
	{
		max -= min;
		time /= totalTime;
		return max * time * time * time + min;
	}
	else return max;
}

float EaseAppearInQuart(float time, float min, float max, float totalTime)	// 加速(四乗)
{
	if (time < totalTime)
	{
		max -= min;
		time /= totalTime;
		return max * time * time * time * time + min;
	}
	else return max;
}

float EaseAppearOutQuad(float time, float min, float max, float totalTime)	// 減速(二乗)
{
	if (time < totalTime) 
	{
		max -= min;
		time /= totalTime;
		return -max * time * (time - 2) + min;
	}
	else return max;
}

float EaseDisappearInQuad(float time, float min, float max, float totalTime)	// 加速(二乗)して消滅
{
	if (time < totalTime)
	{
		min -= max;
		time /= totalTime;
		return min * time * time + max;
	}
	else return min;
}
float EaseDisappearInCubic(float time, float min, float max, float totalTime)	// 加速(三乗)して消滅
{
	if (time < totalTime)
	{
		min -= max;
		time /= totalTime;
		return min * time * time * time + max;
	}
	else return min;
}
float EaseDisappearInQuart(float time, float min, float max, float totalTime)	// 加速(四乗)して消滅
{
	if (time < totalTime)
	{
		min -= max;
		time /= totalTime;
		return min * time * time * time * time + max;
	}
	else return min;
}
float EaseDisappearOutQuad(float time, float min, float max, float totalTime)	// 減速(二乗)して消滅
{
	if (time < totalTime)
	{
		min -= max;
		time /= totalTime;
		return -min* time * (time - 2) + max;
	}
	else return min;
}


float EaseAppearInOutPop(float time, float min, float max, float totalTime)	// 加速→減速(二乗)
{
	if (time < totalTime / 3)
	{
		return EaseAppearOutQuad(time, min, max, totalTime / 3);
	}
	else if (time < (totalTime / 3) * 2)
	{
		return max;
	}
	else
	{
		return EaseDisappearInQuad(time - ((totalTime / 3) * 2), min, max, totalTime / 3);
	}
}