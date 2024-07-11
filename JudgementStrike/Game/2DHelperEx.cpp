#include "UnlimitedLib/2DHelper.h"

// �X�v���C�g�ʒu�w��`�� ��]�g�k�t�� �e���̃X�P�[���w��\ver.
void RenderSpriteEx(int index, float ox, float oy, float x, float y, float angle, float scax, float scay)
{
	SetSpriteOrigin(index, ox, oy);
	SetSpriteRotation(index, angle);
	SetSpriteScale(index, scax, scay);
	SetSpritePos(index, x, y);
	RenderSprite(index);
}

// �X�v���C�g�ʒu�w��`�� ��]�g�k�t�� �e���̃X�P�[���w��\ver.
void RenderSpriteEx(int index, int ox, int oy, int x, int y, float angle, float scax, float scay)
{
	RenderSpriteEx(index, (float)ox, (float)oy, (float)x, (float)y, angle, scax, scay);
}
