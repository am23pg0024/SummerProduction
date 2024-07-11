#pragma once

#define UIMAX 13
#define DRAWDAMAGEMAX 1000

enum UINAME {
	Hp,				// プレイヤーのHP
	Shield,			// プレイヤーのシールド
	StatusBarBlack,	// ステータスバーの黒背景
	StatusBarFrame,	// ステータスバーのフレーム
	Energy,			// 電力ゲージ
	EnergyFrame,	// エネルギーゲージのフレーム
	Timelimit,		// タイムリミット
	TimelimitBack,	// タイムリミット表示の背景
	Kill,			// キル数
	KillBack,		// キル数表示の背景
	Combo,			// コンボ数
	HighScore,		// ハイスコア
	Score,			// 現在のスコア
};

struct UI {
	int sprite;		// 画像保持
	float x, y;		// 座標
	float ox, oy;	// 中心設定
	float count;	// 秒数やレベル等のカウント
	bool isActive;
	float timer;
};

void UI_Initialize();
void UI_Finalize();
void UI_Update();
void UI_Render();
void UI_Restart();

void AddDamageUI(float x, float y, int dmg);