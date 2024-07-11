#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "DebugScene.h"
#include "OptionUI.h"
#include "Game.h"
#include "SoundManager.h"
#include "ResultScene.h"
#include "PreStartScene.h"
#include "Button.h"
#include "2DHelperEx.h"
#include "HitCheck.h"

// スプライト
int titleLogo;
int startOff, startOn, startDecision;
int quitOff, quitOn, quitDecision;

static BUTTON startButton, quitButton;

MENU_ITEM titleItems[2] = { MStart, MExit };
int curTitleItemIndex;

static int mx, my;				// マウスカーソル位置
static int prev_mx, prev_my;	// 前回のマウスカーソル位置

static bool isInput;	// 左スティック入力フラグ
static bool isCancel;	// ボタン決定キャンセルフラグ
static bool isMouseOn;	// マウスがボタンに乗っているか
static bool isClick;	// ボタンクリックフラグ

// 明滅用変数
static int alpha;
static float fadeRate;
static float fadeSpeed;
static bool isStartFadeOn;
static bool isQuitFadeOn;
static bool isTransition;	// 画面遷移中かどうか
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;


static bool SceneInit()
{
	// スプライト読み込み＆設定
	titleLogo = CreateSprite("Assets/Sprites/UI/Title/Title_complete.png");
	startOff = CreateSprite("Assets/Sprites/UI/Title/Start_button_off.png");
	startOn = CreateSprite("Assets/Sprites/UI/Title/Start_button_on.png");
	startDecision = CreateSprite("Assets/Sprites/UI/Title/Start_button_decision.png");
	quitOff = CreateSprite("Assets/Sprites/UI/Title/Quit_button_off.png");
	quitOn = CreateSprite("Assets/Sprites/UI/Title/Quit_button_on.png");
	quitDecision = CreateSprite("Assets/Sprites/UI/Title/Quit_button_decision.png");

	SetSpriteOrigin(titleLogo, 480, 246);
	SetSpriteOrigin(startOff, 114, 41);
	SetSpriteOrigin(startOn, 114, 41);
	SetSpriteOrigin(startDecision, 114, 41);
	SetSpriteOrigin(quitOff, 114, 41);
	SetSpriteOrigin(quitOn, 114, 41);
	SetSpriteOrigin(quitDecision, 114, 41);

	SetSpritePos(titleLogo, SCREEN_WIDTH / 2, 250);
	SetSpritePos(startOn, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 197);
	SetSpritePos(startDecision, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 197);
	SetSpritePos(quitOn, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80);
	SetSpritePos(quitDecision, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80);

	// 変数初期化
	const Sprite* sp = GetSprite(startOff);
	startButton = CreateButton(startOn, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 197, sp->width, sp->height, true);
	sp = GetSprite(quitOff);
	quitButton = CreateButton(quitOff, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, sp->width, sp->height, true);

	mx = my = prev_mx = prev_my = 0;

	curTitleItemIndex = 0;

	isInput = isCancel = isMouseOn = isClick = false;

	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;
	isStartFadeOn = true;
	isQuitFadeOn = false;
	isTransition = false;
	blinkTimer = 0.f;

	PlaySounds(Title);
	SetGameState(GPlay);

	return true;
}

static void SceneFinal()
{
	ReleaseSprite(titleLogo);
	ReleaseSprite(startOff);
	ReleaseSprite(startOn);
	ReleaseSprite(startDecision);
	ReleaseSprite(quitOff);
	ReleaseSprite(quitOn);
	ReleaseSprite(quitDecision);
}

static void SceneUpdate()
{
	// Dキーでデバッグモードへ
	if (IsKeyPush('D')) {
		PlaySounds(ButtonCancel);
		StopSounds(Title);
		JumpScene(SetupDebugScene);
	}

	// ボタン決定前
	if (!isTransition) {
		/* パッド操作処理 */
		// 左スティック入力取得
		float axisY = GetPadLeftStickY(0);

		// メニュー項目操作（パッド&キーボード）
		if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_UP) || (!isInput && axisY > 0)) {
			// ボタン決定キャンセル
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B))
				isCancel = true;

			PlaySounds(MoveCursor);
			alpha = 255;
			fadeRate = 1.0f;
			curTitleItemIndex--;
			if (curTitleItemIndex < 0)
				curTitleItemIndex += 2;
		}
		if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_DOWN) || (!isInput && axisY < 0)) {
			// ボタン決定キャンセル
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B))
				isCancel = true;

			PlaySounds(MoveCursor);
			alpha = 255;
			fadeRate = 1.0f;
			curTitleItemIndex++;
			if (curTitleItemIndex > 1)
				curTitleItemIndex %= 2;
		}

		// 左スティック入力フラグ処理
		if (!isInput) {
			if (axisY != 0)
				isInput = true;
		}
		else {
			if (axisY == 0)
				isInput = false;
		}

		/* マウス操作処理 */
		// マウスカーソル位置取得
		GetMousePosition(&mx, &my);

		// カーソル位置フラグ処理
		if (isMouseOn)
			if (!IsMouseOnButton(startButton) && !IsMouseOnButton(quitButton))
				isMouseOn = false;

		// メニュー項目操作（マウス）
		if (mx != prev_mx || my != prev_my || IsKeyPress(VK_LBUTTON)) {
			if (!isMouseOn) {
				if (IsMouseOnButton(startButton)) {
					isMouseOn = true;
					if (!isClick) {
						curTitleItemIndex = 0;
						alpha = 255;
						fadeRate = 1.0f;
					}
					if (curTitleItemIndex == 0)
						PlaySounds(MoveCursor);
				}
				else if (IsMouseOnButton(quitButton)) {
					isMouseOn = true;
					if (!isClick) {
						curTitleItemIndex = 1;
						alpha = 255;
						fadeRate = 1.0f;
					}
					if (curTitleItemIndex == 1)
						PlaySounds(MoveCursor);
				}
			}
			prev_mx = mx;
			prev_my = my;
		}

		startButton.sprite = startOff;
		quitButton.sprite = quitOff;
		isStartFadeOn = false;
		isQuitFadeOn = false;

		if (!IsKeyPress(VK_LBUTTON))
			isClick = false;
		if (IsKeyRelease(VK_LBUTTON))
			isMouseOn = false;

		// 各メニュー項目選択時処理
		switch (titleItems[curTitleItemIndex]) {
		case MStart:	// Start選択時
			isStartFadeOn = true;
			startButton.sprite = startOn;

			// パッド・キー操作
			if (!isCancel) {
				if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
					startButton.sprite = startDecision;
					isStartFadeOn = false;
				}
			}
			// 選択
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				if (!isCancel) {
					isStartFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
					StopSounds(Title);
				}
				else {
					isCancel = false;
				}
			}

			// マウス操作
			if (IsMouseOnButton(startButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					startButton.sprite = startDecision;
					isStartFadeOn = false;
					isClick = true;
					alpha = 255;
					fadeRate = 1.0f;
				}
				// 選択
				if (IsKeyRelease(VK_LBUTTON)) {
					isStartFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
					StopSounds(Title);
				}
			}
			break;
		case MExit:	// QuitGame選択時
			isQuitFadeOn = true;
			quitButton.sprite = quitOn;

			// パッド・キー操作
			if (!isCancel) {
				if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
					quitButton.sprite = quitDecision;
					isQuitFadeOn = false;
				}
			}
			// 選択
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				if (!isCancel) {
					isQuitFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
					StopSounds(Title);
				}
				else {
					isCancel = false;
				}
			}

			// マウス操作
			if (IsMouseOnButton(quitButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					quitButton.sprite = quitDecision;
					isQuitFadeOn = false;
					isClick = true;
					alpha = 255;
					fadeRate = 1.0f;
				}
				// 選択
				if (IsKeyRelease(VK_LBUTTON)) {
					isQuitFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
					StopSounds(Title);
				}
			}
			break;
		}
	}
	// ボタン決定後
	else {
		switch (titleItems[curTitleItemIndex]) {
		case MStart:	// Start選択時
			isStartFadeOn = true;
			startButton.sprite = startOn;
			break;
		case MExit:		// QuitGame選択時
			isQuitFadeOn = true;
			quitButton.sprite = quitOn;
			break;
		}
	}

	// 選択カーソルの点滅
	fadeRate += fadeSpeed * DELTA_TIME;
	alpha = fadeRate * 255;
	// フェードイン
	if (alpha <= 10) {
		alpha = 10;
		if (!isTransition)
			fadeSpeed = 0.8f;
		else
			fadeSpeed = 10.f;
	}
	// フェードアウト
	if (alpha >= 255) {
		alpha = 255;
		if (!isTransition)
			fadeSpeed = -0.6f;
		else
			fadeSpeed = -10.f;
	}

	if (isStartFadeOn)
		SetSpriteColor(startButton.sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(startButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isQuitFadeOn)
		SetSpriteColor(quitButton.sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(quitButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (titleItems[curTitleItemIndex]) {
			case MStart:	// Start選択時
				JumpScene(SetupPreStartScene);
				break;
			case MExit:	// QuitGame選択時
				PostQuitMessage(0);
				break;
			}
		}
	}
}

static void SceneRender()
{
	RenderBG();
	RenderSprite(titleLogo);
	RenderButton(startButton);
	RenderButton(quitButton);
}

// ゲームシーンのセットアップ
SceneProc SetupTitleScene()
{
	SceneProc proc{
		"Title",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}
