#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "Button.h"
#include "PauseUI.h"
#include "SoundManager.h"

int gameOverBG;
int continueLogo;
int gameOverLogo;
int yesOff;
int yesOn;
int yesDecision;
int noOff;
int noOn;
int noDecision;

BUTTON yesButton, noButton;	// ボタン

static int alpha;
static float fadeRate;
static float fadeSpeed;
static int mx, my;				// マウスカーソル位置
static int prev_mx, prev_my;	// 前回のマウスカーソル位置
static bool isYesFadeOn;
static bool isNoFadeOn;
static bool isTransition;	// 画面遷移中かどうか
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

MENU_ITEM gameOverItems[2] = { MYes, MNo };
int curGameOverItemIndex;

static bool isInput;	// 左スティック入力フラグ
static bool isMouseOn;	// マウスがどこに乗っているか

static OBJECT* selectCursor;

void GameOver_Initialize()
{
	gameOverBG = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_background.png");	// ゲームオーバー背景
	continueLogo = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_continue.png");	// コンティニューロゴ
	gameOverLogo = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_rogo.png");		// ゲームオーバーロゴ
	yesOff = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_off.png");			// 未選択時のyes
	yesOn = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_on.png");				// 選択時のyes
	yesDecision = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_desicion.png");	// 決定時のyes
	noOff = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_off.png");				// 未選択時のno
	noOn = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_on.png");				// 選択時のno
	noDecision = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_desicion.png");	// 決定時のno

	SetSpriteOrigin(gameOverBG, 320, 205);
	SetSpriteOrigin(continueLogo, 169, 35);
	SetSpriteOrigin(gameOverLogo, 291, 66);
	SetSpriteOrigin(yesOff, 179, 54);
	SetSpriteOrigin(yesOn, 179, 54);
	SetSpriteOrigin(yesDecision, 179, 54);
	SetSpriteOrigin(noOff, 179, 54);
	SetSpriteOrigin(noOn, 179, 54);
	SetSpriteOrigin(noDecision, 179, 54);

	SetSpritePos(gameOverBG, SCREEN_WIDTH / 2, 457);
	SetSpritePos(continueLogo, SCREEN_WIDTH / 2, 316);
	SetSpritePos(gameOverLogo, SCREEN_WIDTH / 2, 126);
	SetSpritePos(yesOff, SCREEN_WIDTH / 2, 439);
	SetSpritePos(noOff, SCREEN_WIDTH / 2, 572);
	yesButton = CreateButton(yesOn, SCREEN_WIDTH / 2, 439, 358, 108, true);
	noButton = CreateButton(noOff, SCREEN_WIDTH / 2, 572, 358, 108, true);

	/*点滅用フェード*/
	alpha = 180;
	fadeRate = 0.0f;
	fadeSpeed = 0.0f;
	isYesFadeOn = true;
	isNoFadeOn = false;
	blinkTimer = 0.f;

	mx = my = prev_mx = prev_my = 0;

	curGameOverItemIndex = 0;
	selectCursor = GetSelectCursor();
}

void GameOver_Finalize()
{
	ReleaseSprite(gameOverBG);
	ReleaseSprite(continueLogo);
	ReleaseSprite(gameOverLogo);
	ReleaseSprite(yesOff);
	ReleaseSprite(yesOn);
	ReleaseSprite(yesDecision);
	ReleaseSprite(noOff);
	ReleaseSprite(noOn);
	ReleaseSprite(noDecision);
}

void GameOver_Update()
{
	if (!isTransition) {
		yesButton.sprite = yesOff;
		noButton.sprite = noOff;
		isYesFadeOn = false;
		isNoFadeOn = false;

		// マウスカーソル位置取得
		GetMousePosition(&mx, &my);

		// 左スティック入力取得
		float axisY = GetPadLeftStickY(0);

		// メニュー項目操作（パッド&キーボード）
		if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_UP) || (!isInput && axisY > 0)) {
			PlaySounds(MoveCursor);
			curGameOverItemIndex--;
			if (curGameOverItemIndex < 0)
				curGameOverItemIndex += 2;
		}
		if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_DOWN) || (!isInput && axisY < 0)) {
			PlaySounds(MoveCursor);
			curGameOverItemIndex++;
			if (curGameOverItemIndex > 1)
				curGameOverItemIndex %= 2;
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

		// カーソル位置フラグ処理
		if (isMouseOn) {
			if (!IsMouseOnButton(yesButton) && !IsMouseOnButton(noButton))
				isMouseOn = false;
		}

		// メニュー項目操作（マウス）
		if (mx != prev_mx || my != prev_my) {
			if (IsMouseOnButton(yesButton)) {
				curGameOverItemIndex = 0;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(noButton)) {
				curGameOverItemIndex = 1;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			prev_mx = mx;
			prev_my = my;
		}

		switch (gameOverItems[curGameOverItemIndex]) {
		case MYes:	// Resume選択時
			isYesFadeOn = true;
			yesButton.sprite = yesOn;

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				yesButton.sprite = yesDecision;
				isYesFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isYesFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}

			/*マウス操作*/
			if (IsMouseOnButton(yesButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					yesButton.sprite = yesDecision;
					isYesFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isYesFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
			break;
		case MNo:	// Restart選択時			
			isNoFadeOn = true;
			noButton.sprite = noOn;

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				noButton.sprite = noDecision;
				isNoFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isNoFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}

			/*マウス操作*/
			if (IsMouseOnButton(noButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					noButton.sprite = noDecision;
					isNoFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isNoFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
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

	if (isYesFadeOn)
		SetSpriteColor(yesButton.sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(yesButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isNoFadeOn)
		SetSpriteColor(noButton.sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(noButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (gameOverItems[curGameOverItemIndex]) {
			case MYes:	// Resume選択時
				Restart();
				break;
			case MNo:	// Restart選択時
				JumpScene(SetupTitleScene);
				StopSounds(Game);
				SetGameState(GPlay);
				break;
			}
			isTransition = false;
			blinkTimer = 0.f;
		}
	}
}
void GameOver_Render()
{
	RenderSprite(gameOverBG);
	RenderSprite(continueLogo);
	RenderSprite(gameOverLogo);
	RenderSprite(yesOff);
	RenderSprite(noOff);
	RenderButton(yesButton);
	RenderButton(noButton);
}