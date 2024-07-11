#include "UnlimitedLib/UnlimitedLib.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "Game.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Stage.h"
#include "Bullet.h"
#include "Camera.h"
#include "Shield.h"
#include "GameManager.h"
#include "UI.h"
#include "Animation.h"
#include "Effect.h"
#include "PauseUI.h"
#include "OptionUI.h"
#include "EnemyManager.h"
#include "Explosion.h"
#include "SoundManager.h"
#include "Parameter.h"
#include "GameOverUI.h"
#include "GameClearUI.h"

#define MAX_KILL 1000	// ゲームクリアに必要なキル数

int gameSprite;
int gameClearSprite;
int gameOverSprite;
int fadeSprite;

static int alpha;
static float fadeRate;
static float fadeSpeed;
int alertCnt;	// ラッシュウェーブ時の警告用
bool isAlert;
bool isAlertComplete;

// Debug
bool isInvincible;

void Restart()
{
	// 初期化
	GameManager_Initialize();
	Player_Restart();
	EnemyManager_Restart();
	Bullet_Restart();
	Shield_Restart();
	Effect_Restart();
	Pause_Restart();
	Option_Restart();
	Explosion_Restart();
	UI_Restart();

	InitStage();
	InitCamera();

	// ゲーム状態設定
	SetGameState(GPlay);
	PlaySounds(Game);

	alpha = 0;
	fadeRate = 0.0f;
	alertCnt = 0;
	isAlert = isAlertComplete = false;
	isInvincible = false;
}

static bool SceneInit()
{
	// スプライト読み込み
	gameSprite = CreateSprite("Assets/Sprites/Temp/gamemain.png");
	gameClearSprite = CreateSprite("Assets/Sprites/Temp/gameclear.png");
	gameOverSprite = CreateSprite("Assets/Sprites/Temp/gameover.png");
	fadeSprite =CreateSprite("Assets/Sprites/Temp/white.png");
	SetSpriteSize(gameOverSprite, SCREEN_WIDTH, SCREEN_HEIGHT);
	SetSpriteSize(fadeSprite, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	alpha = 0;
	fadeRate = 0.0f;
	alertCnt = 0;
	isAlert = isAlertComplete = false;
	isInvincible = false;

	// 各種パラメーター読み込み
	LoadParam();

	// 初期化
	GameManager_Initialize();
	Player_Initialize();
	EnemyManager_Initialize();
	Bullet_Initialize();
	Shield_Initialize();
	UI_Initialize();
	Effect_Initialize();
	Pause_Initialize();
	Explosion_Initialize();
	GameOver_Initialize();
	GameClear_Initialize();

	InitStage();
	InitCamera();

	// ゲーム状態設定
	SetGameState(GPlay);
	PlaySounds(Game);

	return true;
}

static void SceneFinal()
{
	ReleaseSprite(gameSprite);
	ReleaseSprite(gameClearSprite);
	ReleaseSprite(gameOverSprite);
	ReleaseSprite(fadeSprite);

	GameManager_Finalize();
	Player_Finalize();
	EnemyManager_Finalize();
	Bullet_Finalize();
	CameraFinalize();
	Shield_Finalize();
	UI_Finalize();
	Effect_Finalize();
	Pause_Finalize();
	Explosion_Finalize();
	GameOver_Finalize();
	GameClear_Finalize();
}

static void SceneUpdate()
{
	// ゲーム状態で場合分け
	switch (GetGameState()) {
	case GPlay:/*ゲームプレイ時*/
		// Debug
		if (IsKeyPush('1')) isInvincible = true;
		if (IsKeyPush('2')) Damage2Player(GetPlayer(), 20);
		if (IsKeyPush('3')) AddKillCount(50);
		if (IsKeyPress('4')) SubTimeLimit(1.f);
		if (IsKeyPress('5')) AddEnergy(1);
		if (IsKeyPress('6')) AddComboCount(1);

		if (isInvincible) PlayerSetInvincible(1.f);

		//ポーズ画面
		if (IsKeyPush(VK_ESCAPE) || IsPadPush(0, XINPUT_GAMEPAD_START)) {
			SetGameState(GPause);
			return;
		}

		// ゲームクリア
		if (GetGameManager()->timeLimit <= 0) {
			SetGameState(GClear);
			PlayerSetInvincible(1.f);

			// 敵を全滅させる
			for (int i = 0; i < GetMaxSpawn(); i++)
				GetEnemies()[i]->obj.state = SDead;

			return;
		}
		// ゲームオーバー
		if (!GetPlayer()->obj.isActive) {
			SetGameState(GGameover);
			StopSounds(Game);
			PlaySounds(GameOver);
			return;
		}

		// 更新
		Player_Update();
		EnemyManager_Update();
		Bullet_Update();
		CameraUpdate();
		Shield_Update();
		GameManager_Update();
		Effect_Update();
		Explosion_Update();
		UI_Update();

		if (!isAlert && !isAlertComplete && GetGameManager()->wave == 4)
			isAlert = true;

		if (isAlert) {
			SetSpriteColor(fadeSprite, D3DCOLOR_RGBA(255, 0, 0, alpha));

			fadeRate += fadeSpeed * DELTA_TIME;
			alpha = fadeRate * 255;
			if (alpha >= 140) {
				alertCnt++;
				alpha = 140;
				fadeSpeed = -0.8f;
			}
			else if (alpha <= 0) {
				if (alertCnt >= 2) {
					isAlert = false;
					isAlertComplete = true;
				}
				alpha = 0;
				fadeSpeed = 0.6f;
			}
		}

		break;
	case GPause:/*ポーズ時*/
		Pause_Update();
		break;
	case GOption:/*オプション時*/
		Option_Update();
		break;
	case GClear:/*ゲームクリア時*/
		Player_Update();
		EnemyManager_Update();
		Bullet_Update();
		Shield_Update();
		Effect_Update();
		Explosion_Update();
		UI_Update();

		/*↓フェードインに合わせる*/
		fadeSpeed = 0.6f;
		fadeRate += fadeSpeed * DELTA_TIME;
		alpha = fadeRate * 255;
		if (alpha >= 140) {
			alpha = 140;
			GameClear_Update();
		}
		break;
	case GGameover:/*ゲームオーバー時*/
		Player_Update();
		EnemyManager_Update();
		Bullet_Update();
		Shield_Update();
		Effect_Update();
		Explosion_Update();
		UI_Update();

		/*↓フェードインに合わせる*/
		fadeSpeed = 0.3f;
		fadeRate += fadeSpeed * DELTA_TIME;
		alpha = fadeRate * 255;
		if (alpha >= 150) {
			alpha = 150;
			GameOver_Update();
		}
		break;
	}
}

static void SceneRender()
{
	Stage_Render();
	EnemyManager_Render();
	Explosion_Render();
	Player_Render();
	Bullet_Render();
	Shield_Render();
	UI_Render();
	Effect_Render();

	// ゲーム状態で場合分け
	switch (GetGameState()) {
	case GPlay:
		if (isAlert && !isAlertComplete)
			RenderSprite(fadeSprite);
		break;
	case GPause:/*ポーズ時*/
		SetSpriteColor(fadeSprite, D3DCOLOR_RGBA(0, 0, 0, 140));
		RenderSprite(fadeSprite);
		Pause_Render();
		break;
	case GOption:/*オプション時*/
		Option_Render();
		break;
	case GClear:/*ゲームクリア時*/
		/*フェードイン(光点)*/
		SetSpriteColor(fadeSprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
		RenderSprite(fadeSprite);
		if (alpha >= 140) {
			//ゲームクリア画面
			GameClear_Render();
		}
		break;
	case GGameover:/*ゲームオーバー時*/
		/*フェードイン(暗転)*/
		SetSpriteColor(fadeSprite, D3DCOLOR_RGBA(0, 0, 0, alpha));
		RenderSprite(fadeSprite);
		if (alpha >= 150) {
			//ゲームオーバー画面
			GameOver_Render();
		}
		break;
	}
}

// ゲームシーンのセットアップ
SceneProc SetupGameScene()
{
	SceneProc proc{
		"Game",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}
