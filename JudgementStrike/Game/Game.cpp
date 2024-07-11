#include "UnlimitedLib/UnlimitedLib.h"
#include "Game.h"
#if USE_IMGUI
#include "imgui/imgui.h"
#endif

#include "UnlimitedLib/UnlimitedLibExtension.h"

#include "SceneManager.h"
#include "TitleScene.h"
#include "DebugScene.h"
#include "GameScene.h"
//#include "MemoryPool.h"
#include "Stage.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "OptionUI.h"
#include "UI.h"
#include <math.h>

HSV_PARAM hsvParam = { 0 };

extern float spawnTime;
extern int enemyNum;
extern float chargeTimer;			// チャージ関連のタイマー
extern float coolTime;

static GAME_STATE gState;

OBJECT selectCursor;
int bgSprite;	// 背景

static VECTOR offset;	// デフォルトの画面サイズとのズレの倍率

// シェーダー
int changeHSVShader;


/* 関数宣言 */
const GAME_STATE GetGameState() { return gState; }
void SetGameState(GAME_STATE state) { gState = state; }

OBJECT* GetSelectCursor() { return &selectCursor; }

void SetSelectCursorSprite(int sprite)
{
	selectCursor.sprite = sprite;
	selectCursor.width = GetSpriteTextureWidth(selectCursor.sprite);
	selectCursor.height = GetSpriteTextureHeight(selectCursor.sprite);
}
VECTOR* GetOffset() { return &offset; }

int GetChangeHSVShader() { return changeHSVShader; }
HSV_PARAM* GetHSVParam() { return &hsvParam; }
void SetHSVParam(float hue, float saturation, float value)
{
	hsvParam.hue = hue;
	hsvParam.saturation = saturation;
	hsvParam.value = value;
}

// 初期化
void Initialize()
{
	EnableDDSReplace(true);

	// 選択カーソルのスプライト読み込み
	selectCursor.sprite = -1;
	CreateSprite("Assets/Sprites/UI/Background_01.png");

	//InitSpritePool(sizeof(int) * 512);
	Stage_Initialize();
	Sounds_Initialize();
	Option_Initialize();

	changeHSVShader = LoadCustomShader("Assets/Shader/2D.hlsl", "PSChangeHSV");

	offset = { 0.f, 0.f };

	InitSceneManager();
	JumpScene(SetupTitleScene);
}

// 終了処理
void Finalize()
{
	ReleaseSprite(selectCursor.sprite);

	Stage_Finalize();
	Sounds_Finalize();
	Option_Finalize();

	ReleaseCustomShader(changeHSVShader);
	FinalSceneManager();
	//FinalSpritePool();
}

#if USE_IMGUI
// デバッグメニュー描画(デバッグモード時のみ使用可能)
void DebugMenu(void)
{
	ImGui::Begin("Debug");
	//ImGui::Text("EnergyGauge: %d", GetGameManager()->energyGauge);
	//ImGui::Text("EnergyLevel: %d", GetGameManager()->energyLevel);
	//ImGui::Text("Kill: %d", GetGameManager()->killCnt);
	ImGui::Text("EnemyNum: %d", enemyNum);
	ImGui::Text("SpawnTime: %f", spawnTime);
	ImGui::Text("ChargeTimer: % f", chargeTimer);
	ImGui::Text("Wave: %d", GetGameManager()->wave);
	ImGui::Text("Volume BGM: %.0f", GetVolume()->BGM * 100);
	ImGui::Text("Volume SE: %.0f", GetVolume()->SE * 100);
	ImGui::Text("CoolTime: %f", coolTime);
	ImGui::End();
}
#endif

// メインループ
void GameMain()
{
	// 画面サイズのズレを計算
	RECT rect;
	GetClientRect(GetWindowHandle(), &rect);
	offset = { (rect.right - rect.left) / (float)SCREEN_WIDTH, (rect.bottom - rect.top) / (float)SCREEN_HEIGHT };

	UpdateSceneManager();
	RenderSceneManager();

#if USE_IMGUI
	DebugMenu();
#endif
}

void RenderBG()
{
	RenderSpritePos(bgSprite, 0, 0);
}

float GetDistance(const OBJECT* a, const OBJECT* b)
{
	float x = a->x - b->x;
	float y = a->y - b->y;
	return sqrtf(x * x + y * y);
}

void Swap(float& a, float& b)
{
	float tmp = a;
	a = b;
	b = tmp;
}

void Damage(OBJECT* obj, int damage)
{
	AddDamageUI(obj->x, obj->y, damage);
	obj->hp -= damage;

	if (obj->hp <= 0)
		obj->state = SDead;
	else
		obj->state = SDamaged;
}
