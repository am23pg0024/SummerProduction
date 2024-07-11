#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "Button.h"
#include "PauseUI.h"
#include "SoundManager.h"

int pauseBackSprite;
int pauseSprite;
int backHexSprite;
int backLightSprite;
int backLightDecisionSprite;

static int alpha;
static float fadeRate;
static float fadeSpeed;
static int mx, my;				// マウスカーソル位置
static int prev_mx, prev_my;	// 前回のマウスカーソル位置
static bool isSelectFadeOn;
static bool isTransition;	// 画面遷移中かどうか
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

MENU_ITEM pauseItems[4] = { MResume, MRestart, MOption, MExit };
int curPauseItemIndex;

static bool isInput;	// 左スティック入力フラグ
static bool isMouseOn;	// マウスがどこに乗っているか

// ボタン
BUTTON resumeButton, restartButton, optionButton, quitButton;

static OBJECT* selectCursor;
void Pause_Restart()
{
	/*点滅用フェード*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;

	mx = my = prev_mx = prev_my = 0;

	curPauseItemIndex = 0;
}

void Pause_Initialize()
{
	/*画像読み込み*/
	pauseSprite = CreateSprite("Assets/Sprites/Pause/Pause_rogo.png");						// ポーズロゴ
	pauseBackSprite = CreateSprite("Assets/Sprites/Pause/Pause_background.png");			// ポーズ画面背景
	resumeButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_resume_game.png");		// 再開
	restartButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_restart.png");			// 再スタート
	optionButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_option.png");			// オプション
	quitButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_quit.png");				// 終了
	backLightSprite = CreateSprite("Assets/Sprites/Pause/Pause_backLight_nomal.png");	// バックライト
	backLightDecisionSprite = CreateSprite("Assets/Sprites/Pause/Pause_backlight_desicion.png");// バックライト(押してるとき)
	backHexSprite = CreateSprite("Assets/Sprites/Pause/Pause_backlight_hex.png");					// 横の六角形

	SetSpriteOrigin(resumeButton.sprite, 167, 34);
	SetSpriteOrigin(restartButton.sprite, 84, 34);
	SetSpriteOrigin(optionButton.sprite, 72, 34);
	SetSpriteOrigin(quitButton.sprite, 44, 34);
	SetSpriteOrigin(backHexSprite, 21, 25);
	SetSpriteOrigin(backLightSprite, 243, 34);
	SetSpriteOrigin(backLightDecisionSprite, 243, 34);

	/*ボタン作成*/
	resumeButton = CreateButton(resumeButton.sprite, SCREEN_WIDTH / 2, 305, 486, 68, true);
	restartButton = CreateButton(restartButton.sprite, SCREEN_WIDTH / 2, resumeButton.y + 94, 320, 68, true);
	optionButton = CreateButton(optionButton.sprite, SCREEN_WIDTH / 2, restartButton.y + 94, 296, 68, true);
	quitButton = CreateButton(quitButton.sprite, SCREEN_WIDTH / 2, optionButton.y + 94, 240, 68, true);

	/*点滅用フェード*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;
	isSelectFadeOn = true;
	blinkTimer = 0.f;

	mx = my = prev_mx = prev_my = 0;

	curPauseItemIndex = 0;
	selectCursor = GetSelectCursor();
	SetSelectCursorSprite(backLightSprite);
}

void Pause_Finalize()
{
	ReleaseSprite(pauseSprite);
	ReleaseSprite(pauseBackSprite);
	ReleaseSprite(resumeButton.sprite);
	ReleaseSprite(optionButton.sprite);
	ReleaseSprite(restartButton.sprite);
	ReleaseSprite(quitButton.sprite);
	ReleaseSprite(backLightSprite);
	ReleaseSprite(backLightDecisionSprite);
	ReleaseSprite(backHexSprite);
}

void Pause_Update()
{
	if (!isTransition) {
		// マウスカーソル位置取得
		GetMousePosition(&mx, &my);

		// 左スティック入力取得
		float axisY = GetPadLeftStickY(0);

		// メニュー項目操作（パッド&キーボード）
		if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_UP) || (!isInput && axisY > 0)) {
			PlaySounds(MoveCursor);
			curPauseItemIndex--;
			if (curPauseItemIndex < 0)
				curPauseItemIndex += 4;
		}
		if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_DOWN) || (!isInput && axisY < 0)) {
			PlaySounds(MoveCursor);
			curPauseItemIndex++;
			if (curPauseItemIndex > 3)
				curPauseItemIndex %= 4;
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
			if (!IsMouseOnButton(resumeButton) && !IsMouseOnButton(optionButton) && !IsMouseOnButton(restartButton) && !IsMouseOnButton(quitButton))
				isMouseOn = false;
		}

		// メニュー項目操作（マウス）
		if (mx != prev_mx || my != prev_my) {
			if (IsMouseOnButton(resumeButton)) {
				curPauseItemIndex = 0;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(restartButton)) {
				curPauseItemIndex = 1;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(optionButton)) {
				curPauseItemIndex = 2;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(quitButton)) {
				curPauseItemIndex = 3;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			prev_mx = mx;
			prev_my = my;
		}

		isSelectFadeOn = true;

		switch (pauseItems[curPauseItemIndex]) {
		case MResume:	// Resume選択時
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, resumeButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, resumeButton.x, resumeButton.y);

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				SetSelectCursorSprite(backLightDecisionSprite);
				SetSpriteScale(selectCursor->sprite, resumeButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
				SetSpritePos(selectCursor->sprite, resumeButton.x, resumeButton.y);
				isSelectFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isSelectFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}

			/*マウス操作*/
			if (IsMouseOnButton(resumeButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					SetSelectCursorSprite(backLightDecisionSprite);
					SetSpriteScale(selectCursor->sprite, resumeButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
					SetSpritePos(selectCursor->sprite, resumeButton.x, resumeButton.y);
					isSelectFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isSelectFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
			break;
		case MRestart:	// Restart選択時
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, restartButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, restartButton.x, restartButton.y);

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				SetSelectCursorSprite(backLightDecisionSprite);
				SetSpriteScale(selectCursor->sprite, restartButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
				SetSpritePos(selectCursor->sprite, restartButton.x, restartButton.y);
				isSelectFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isSelectFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}

			/*マウス操作*/
			if (IsMouseOnButton(restartButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					SetSelectCursorSprite(backLightDecisionSprite);
					SetSpriteScale(selectCursor->sprite, restartButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
					SetSpritePos(selectCursor->sprite, restartButton.x, restartButton.y);
					isSelectFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isSelectFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
			break;
		case MOption:	// Option選択時
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, optionButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, optionButton.x, optionButton.y);

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				SetSelectCursorSprite(backLightDecisionSprite);
				SetSpriteScale(selectCursor->sprite, optionButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
				SetSpritePos(selectCursor->sprite, optionButton.x, optionButton.y);
				isSelectFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isSelectFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}

			/*マウス操作*/
			if (IsMouseOnButton(optionButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					SetSelectCursorSprite(backLightDecisionSprite);
					SetSpriteScale(selectCursor->sprite, optionButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
					SetSpritePos(selectCursor->sprite, optionButton.x, optionButton.y);
					isSelectFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isSelectFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
			break;
		case MExit:	// Exit選択時
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, quitButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, quitButton.x, quitButton.y);

			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				SetSelectCursorSprite(backLightDecisionSprite);
				SetSpriteScale(selectCursor->sprite, quitButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
				SetSpritePos(selectCursor->sprite, quitButton.x, quitButton.y);
				isSelectFadeOn = false;
				alpha = 255;
				fadeRate = 1.0f;
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isSelectFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				StopSounds(Game);
				PlaySounds(ButtonDecision);
			}
			/*マウス操作*/
			if (IsMouseOnButton(quitButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					SetSelectCursorSprite(backLightDecisionSprite);
					SetSpriteScale(selectCursor->sprite, quitButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
					SetSpritePos(selectCursor->sprite, quitButton.x, quitButton.y);
					isSelectFadeOn = false;
					alpha = 255;
					fadeRate = 1.0f;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isSelectFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					StopSounds(Game);
					PlaySounds(ButtonDecision);
				}
			}
			break;
		}

		// ゲーム画面に戻る
		if (IsKeyPush(VK_ESCAPE) || IsPadPush(0, XINPUT_GAMEPAD_START) || IsPadRelease(0, XINPUT_GAMEPAD_A)) {
			PlaySounds(ButtonCancel);
			SetGameState(GPlay);

			curPauseItemIndex = 0;
			alpha = 255;
			fadeRate = 1.f;
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

	if(isSelectFadeOn)
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (pauseItems[curPauseItemIndex]) {
			case MResume:	// Resume選択時
				SetGameState(GPlay);
				SetSelectCursorSprite(backLightSprite);
				break;
			case MRestart:	// Restart選択時
				Restart();
				SetSelectCursorSprite(backLightSprite);
				break;
			case MOption:	// Option選択時
				SetGameState(GOption);
				SetSelectCursorSprite(backLightSprite);
				alpha = 255;
				fadeRate = 1.f;
				fadeSpeed = 0.f;
				break;
			case MExit:		// QuitGame選択時
				JumpScene(SetupTitleScene);
				SetGameState(GPlay);				
 				SetSelectCursorSprite(backLightSprite);
				break;
			}
			isTransition = false;
			blinkTimer = 0.f;
		}
	}
}


void Pause_Render()
{
	RenderSpritePos(pauseSprite, 502, 80);

	RenderSpritePos(pauseBackSprite, 297, 242);

	RenderSprite(selectCursor->sprite);

	switch (pauseItems[curPauseItemIndex])	// 六角形表示
	{
	case MResume:
		RenderSpritePos(backHexSprite, resumeButton.x - 167 - 59, resumeButton.y - 25);
		RenderSpritePos(backHexSprite, resumeButton.x + 167 + 17, resumeButton.y - 25);
		break;
	case MRestart:
		RenderSpritePos(backHexSprite, restartButton.x - 84 - 59, restartButton.y - 25);
		RenderSpritePos(backHexSprite, restartButton.x + 84 + 17, restartButton.y - 25);
		break;
	case MOption:
		RenderSpritePos(backHexSprite, optionButton.x - 72 - 59, optionButton.y - 25);
		RenderSpritePos(backHexSprite, optionButton.x + 72 + 17, optionButton.y - 25);
		break;
	case MExit:
		RenderSpritePos(backHexSprite, quitButton.x - 44 - 59, quitButton.y - 25);
		RenderSpritePos(backHexSprite, quitButton.x + 44 + 17, quitButton.y - 25);
		break;
	}

	RenderButton(resumeButton);
	RenderButton(restartButton);
	RenderButton(optionButton);
	RenderButton(quitButton);
}
