#pragma once

#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

#define DELTA_TIME (1.f / 60)

// ゲームの状態
enum GAME_STATE {
	GPlay,		// ゲームプレイ
	GClear,		// ゲームクリア
	GGameover,	// ゲームオーバー
	GPause,		// ポーズ
	GOption,	// オプション画面
};

// オブジェクトが持つ状態
enum STATE {
	SIdle,			// 待機
	SMove,			// 移動
	SAttack,		// 攻撃
	SCharging,		// チャージ中
	SChargeAttack,	// チャージ攻撃
	SAvoid,			// 回避
	SDamaged,		// 被ダメージ
	SDead,			// 死亡
};

struct VECTOR {
	float x, y;
};

struct FRECT {
    float left, top, right, bottom;
};

struct CIRCLE {
	float x, y;		// 中心座標
	float radius;	// 半径
};

struct FAN {
	float x, y; // 扇の中心
	float radius;    // 扇の半径
	float angle;     // 扇の範囲(角度)
};

struct OBJECT {
	int sprite;			// スプライト
	int width, height;	// スプライトサイズ

	float x, y;		// 座標
	float vx, vy;	// 速度
	float speed;	// 速さ
	float angle;	// 角度
	bool isActive;	// 生存フラグ
	bool isRight;	// 右を向いているかどうか
	STATE state;	// 状態
	FRECT hitRect;	// 当たり判定矩形

	int hp;		// 体力
};

// HSVパラメータ
struct HSV_PARAM {
	float hue;			// 色相
	float saturation;	// 彩度
	float value;		// 輝度
	float dummy3;
};

void Initialize();
void Finalize();
void GameMain();

void RenderBG();

const GAME_STATE GetGameState();
void SetGameState(GAME_STATE state);
OBJECT* GetSelectCursor();
void SetSelectCursorSprite(int sprite);
VECTOR* GetOffset();
int GetChangeHSVShader();
HSV_PARAM* GetHSVParam();
void SetHSVParam(float hue, float saturation, float value);

float GetDistance(const OBJECT* a, const OBJECT* b);
void Swap(float& a, float& b);

// 対象オブジェクトにダメージを与える
void Damage(OBJECT* obj, int damage);
