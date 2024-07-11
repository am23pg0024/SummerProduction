#include "UnlimitedLib/2DHelper.h"

// スプライト位置指定描画 回転拡縮付き 各軸のスケール指定可能ver.
void RenderSpriteEx(int index, float ox, float oy, float x, float y, float angle, float scax, float scay)
{
	SetSpriteOrigin(index, ox, oy);
	SetSpriteRotation(index, angle);
	SetSpriteScale(index, scax, scay);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}

// スプライト位置指定描画 回転拡縮付き 各軸のスケール指定可能ver.
void RenderSpriteEx(int index, int ox, int oy, int x, int y, float angle, float scax, float scay)
{
	RenderSpriteEx(index, (float)ox, (float)oy, (float)x, (float)y, angle, scax, scay);
}
