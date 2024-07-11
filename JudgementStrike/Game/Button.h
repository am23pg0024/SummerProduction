#pragma once

// メニュー項目選択用
enum MENU_ITEM {
	MStart,
	MResume,
	MRestart,
	MOption,
	MBGM,
	MSE,
	MExit,
	MYes,
	MNo,
	MArrowL,
	MArrowR,
};

// ボタン構造体
struct BUTTON {
	int sprite;				// ボタンのスプライトID
	float x, y;				// ボタン左上の座標
	float width, height;	// ボタンの幅と高さ
	bool isActive;	// アクティブなボタンかどうか（ステージセレクトで使えるかも？）
};

BUTTON CreateButton(int sprite, float x, float y, float width, float height, bool isActive);
void SetButtonActive(BUTTON& button, bool isActive);

// マウスカーソルがボタンに乗っているかを判定
bool IsMouseOnButton(BUTTON& button);

void RenderButton(BUTTON& button);
