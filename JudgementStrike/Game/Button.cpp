#include "UnlimitedLib/UnlimitedLib.h"
#include "Button.h"
#include "Game.h"

void SetButtonActive(BUTTON& button, bool isActive) { button.isActive = isActive; }

BUTTON CreateButton(int sprite, float x, float y, float width, float height, bool isActive)
{
    BUTTON button = { sprite, x, y, width, height, isActive };
    return button;
}

// マウスカーソルがボタンに乗っているかを判定
bool IsMouseOnButton(BUTTON& button)
{
    if (!button.isActive) return false;

    int mx, my;
    GetMousePosition(&mx, &my);

    VECTOR offset = *GetOffset();
    const Sprite* sp = GetSprite(button.sprite);

    if (mx >= (button.x - button.width / 2) * offset.x &&
        mx <= (button.x + button.width / 2) * offset.x &&
        my >= (button.y - button.height / 2) * offset.y &&
        my <= (button.y + button.height / 2) * offset.y)
    {
        return true;
    }

    return false;
}

void RenderButton(BUTTON& button)
{
    const Sprite* sp = GetSprite(button.sprite);
    RenderSpriteRot(button.sprite, sp->cx, sp->cy, button.x, button.y, sp->angle, sp->scax);
}
