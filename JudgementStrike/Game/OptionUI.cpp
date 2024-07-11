#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Game.h"
#include "Button.h"
#include "OptionUI.h"
#include "SoundManager.h"
#include "Easing.h"

MENU_ITEM optionItems[3] = { MBGM, MSE, MExit };
int curOptionItemIndex;

/*スプライト*/
int optionBG;			// 背景(後ろから二番目)
int optionSprite;		// オプションロゴ
int barOffSprite;		// 未選択時のオプションバー
int barOnSprite;		// 選択時のオプションバー
int exitSprite;			// Exitの表示するスプライト
int exitOffSprite;		// Exit未選択時バー
int exitOnSprite;		// Exit選択時バー
int exitDecisionSprite;	// Exit決定時バー
int bgmSprite;			// BGMの文字
int seSprite;			// SEの文字
int arrowOffSpriteL;		// 未選択時の矢印
int arrowOnSpriteL;		// 選択時の矢印
int arrowDecisionSpriteL;// 決定時の矢印
int arrowOffSpriteR;		// 未選択時の矢印
int arrowOnSpriteR;		// 選択時の矢印
int arrowDecisionSpriteR;// 決定時の矢印
int arrowBGMLSprite;	// BGMの左矢印
int arrowBGMRSprite;	// BGMの右矢印
int arrowSELSprite;		// SEの左矢印
int arrowSERSprite;		// SEの右矢印
int invisibleSprite;	//ボタン用(透明)
int numberSprite[10];	// 数字

static int mx, my;				// マウスカーソル位置
static int prev_mx, prev_my;	// 前回のマウスカーソル位置

static int alpha;
static float fadeRate;
static float fadeSpeed;
static bool isQuitFadeOn;
static bool isTransition;	// 画面遷移中かどうか
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

static float size;	// イージング用の変数

/*ボタン*/
BUTTON bgmButton, bgmLButton, bgmRButton, //bgm
       seButton,  seLButton,  seRButton,  //se
	   backButton;                        //back
static OBJECT* selectCursor;

static bool isInput;	// 左スティック入力フラグ
static bool isMouseOn;	// マウスがどこに乗っているか

void DrawBGMVolume(int x, int y, int volume);
void DrawSEVolume(int x, int y, int volume);

void Option_Restart()
{
	/*点滅用フェード*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;

	mx = my = prev_mx = prev_my = 0;

	curOptionItemIndex = 0;
}

void Option_Initialize()
{
	/*画像読み込み*/
	optionBG = CreateSprite("Assets/Sprites/Option/Background_02.png");					// 背景(後ろから二番目)
	optionSprite = CreateSprite("Assets/Sprites/Option/Option_rogo.png");							// オプション
	bgmButton.sprite = CreateSprite("Assets/Sprites/Option/Option_ber_off.png");					// 未選択時のBGMバー
	seButton.sprite = CreateSprite("Assets/Sprites/Option/Option_ber_off.png");						// 未選択時のSEバー
	barOnSprite = CreateSprite("Assets/Sprites/Option/Option_ber_on.png");							// 選択時のオプションバー
	exitOffSprite = CreateSprite("Assets/Sprites/Option/Exit_button_off.png");					// Exit未選択時バー
	exitOnSprite = CreateSprite("Assets/Sprites/Option/Exit_button_on.png");						// Exit選択時バー
	exitDecisionSprite = CreateSprite("Assets/Sprites/Option/Exit_button_desicion.png");			// Exit選択時バー
	bgmSprite = CreateSprite("Assets/Sprites/Option/Option_ber_bgm.png");							// BGMの文字
	seSprite = CreateSprite("Assets/Sprites/Option/Option_ber_se.png");								// SEの文字
	arrowOffSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_off.png");					// 未選択時の矢印
	arrowOnSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_on_nomal.png");				// 選択時の矢印
	arrowDecisionSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_on_desicion.png");		// 決定時の矢印				
	arrowOffSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_off.png");					// 未選択時の矢印
	arrowOnSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_on_nomal.png");				// 選択時の矢印
	arrowDecisionSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_on_desicion.png");		// 決定時の矢印				
	CreateSpriteMatrix("Assets/Sprites/Option/Option_numbers.png", 25, 42, 10, 1, 10, numberSprite);// 数字


	/*スプライトの中心設定*/
	SetSpriteOrigin(bgmButton.sprite, 324, 51);
	SetSpriteOrigin(seButton.sprite, 324, 51);
	SetSpriteOrigin(barOnSprite, 324, 51);
	SetSpriteOrigin(arrowOffSpriteL, 46, 52);
	SetSpriteOrigin(arrowOnSpriteL, 46, 52);
	SetSpriteOrigin(arrowDecisionSpriteL, 50, 57);
	SetSpriteOrigin(arrowOffSpriteR, 46, 52);
	SetSpriteOrigin(arrowOnSpriteR, 46, 52);
	SetSpriteOrigin(arrowDecisionSpriteR, 50, 57);
	SetSpriteOrigin(bgmSprite, 40, 27);
	SetSpriteOrigin(seSprite, 24, 27);
	SetSpriteOrigin(exitOffSprite, 133, 43);
	SetSpriteOrigin(exitOnSprite, 133, 43);
	SetSpriteOrigin(exitDecisionSprite, 133, 43);

	/*ボタン作成*/
	bgmButton = CreateButton(bgmButton.sprite, SCREEN_WIDTH / 2, 284, 648, 108, true);
	bgmLButton = CreateButton(arrowOffSpriteL, SCREEN_WIDTH / 2 - 30, 280, 48, 52, true);
	bgmRButton = CreateButton(arrowOffSpriteR, SCREEN_WIDTH / 2 + 200, 280, bgmLButton.width, bgmLButton.height, true);
	seButton = CreateButton(seButton.sprite, SCREEN_WIDTH / 2, 434, 648, 108, true);
	seLButton = CreateButton(arrowOffSpriteL, SCREEN_WIDTH / 2 - 30, 430, bgmLButton.width, bgmLButton.height, true);
	seRButton = CreateButton(arrowOffSpriteR, SCREEN_WIDTH / 2 + 200, 430, bgmLButton.width, bgmLButton.height, true);
	backButton = CreateButton(exitOffSprite, SCREEN_WIDTH / 2, 574, 266, 88, true);

	SetSpritePos(bgmSprite, SCREEN_WIDTH / 2 - 190, 280);
	SetSpritePos(seSprite, SCREEN_WIDTH / 2 - 190, 430);

	SetSpriteScale(arrowOffSpriteR, -1.f / 2, 1.f / 2);
	SetSpriteScale(arrowOnSpriteR, -1.f / 2, 1.f / 2);
	SetSpriteScale(arrowDecisionSpriteR, -1.f / 2, 1.f / 2);

	/*点滅用フェード*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;
	isQuitFadeOn = true;
	blinkTimer = 0.f;
	size = 0;

	mx = my = prev_mx = prev_my = 0;

	curOptionItemIndex = 0;
	selectCursor = GetSelectCursor();

	SetSelectCursorSprite(barOnSprite);
}

void Option_Finalize()
{
	ReleaseSprite(optionBG);
	ReleaseSprite(optionSprite);
	ReleaseSprite(bgmButton.sprite);
	ReleaseSprite(seButton.sprite);
	ReleaseSprite(barOnSprite);
	ReleaseSprite(exitOffSprite);
	ReleaseSprite(exitOnSprite);
	ReleaseSprite(exitDecisionSprite);
	ReleaseSprite(bgmSprite);
	ReleaseSprite(seSprite);
	ReleaseSprite(arrowOffSpriteL);
	ReleaseSprite(arrowOnSpriteL);
	ReleaseSprite(arrowDecisionSpriteL);
	ReleaseSprite(arrowOffSpriteR);
	ReleaseSprite(arrowOnSpriteR);
	ReleaseSprite(arrowDecisionSpriteR);
	ReleaseSpriteMatrix(numberSprite, 10);
}

void Option_Update()
{
	// 矢印のイージング用
	size += DELTA_TIME;
	if (size > 1.f)
		size = 0;

	// 音量取得用
	const VOLUME* vol = GetVolume();

	if (!isTransition) {
		// マウスカーソル位置取得
		GetMousePosition(&mx, &my);

		// 左スティック入力取得
		float axisY = GetPadLeftStickY(0);

		// メニュー項目操作（パッド&キーボード）
		if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_UP) || (!isInput && axisY > 0)) {
			PlaySounds(MoveCursor);
			curOptionItemIndex--;
			if (curOptionItemIndex < 0)
				curOptionItemIndex += 3;
		}
		if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_DOWN) || (!isInput && axisY < 0)) {
			PlaySounds(MoveCursor);
			curOptionItemIndex++;
			if (curOptionItemIndex > 2)
				curOptionItemIndex %= 3;
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
			if (!IsMouseOnButton(bgmButton) && !IsMouseOnButton(seButton) && !IsMouseOnButton(backButton))
				isMouseOn = false;
		}

		// メニュー項目操作（マウス）
		if (mx != prev_mx || my != prev_my) {
			if (IsMouseOnButton(bgmButton)) {
				curOptionItemIndex = 0;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(seButton)) {
				curOptionItemIndex = 1;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			else if (IsMouseOnButton(backButton)) {
				curOptionItemIndex = 2;
				if (!isMouseOn) {
					PlaySounds(MoveCursor);
					isMouseOn = true;
				}
			}
			prev_mx = mx;
			prev_my = my;
		}

		switch (optionItems[curOptionItemIndex]) {
		case MBGM:	// BGM選択時
			bgmLButton.sprite = arrowOnSpriteL;
			bgmRButton.sprite = arrowOnSpriteR;
			seLButton.sprite = arrowOffSpriteL;
			seRButton.sprite = arrowOffSpriteR;
			SetSelectCursorSprite(barOnSprite);
			//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);

			SetSpritePos(selectCursor->sprite, bgmButton.x, bgmButton.y);
			SetSpriteScale(bgmLButton.sprite, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2);
			SetSpriteScale(bgmRButton.sprite, -EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2);
			SetSpriteScale(seLButton.sprite, 1.f / 2, 1.f / 2);
			SetSpriteScale(seRButton.sprite, -1.f / 2, 1.f / 2);

			//減少
			if (vol->BGM > 0) {
				/*キー操作*/
				if (IsKeyPress(VK_LEFT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					bgmLButton.sprite = arrowDecisionSpriteL;
					SetSpriteScale(bgmLButton.sprite, 1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_LEFT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(BGM, -10);
				}
				/*マウス操作*/
				if (IsMouseOnButton(bgmLButton)) {
					if (IsKeyPress(VK_LBUTTON)) {
						bgmLButton.sprite = arrowDecisionSpriteL;
						SetSpriteScale(bgmLButton.sprite, 1.f / 2, 1.f / 2);
					}
					if (IsKeyRelease(VK_LBUTTON)) {
						PlaySounds(ButtonDecision);
						SetSoundVolume(BGM, -10);
					}
				}
			}

			//増加
			if (vol->BGM < 100) {
				/*キー操作*/
				if (IsKeyPress(VK_RIGHT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					bgmRButton.sprite = arrowDecisionSpriteR;
					SetSpriteScale(bgmRButton.sprite, -1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_RIGHT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(BGM, 10);
				}
				/*マウス操作*/
				if (IsMouseOnButton(bgmRButton)) {
					if (IsKeyPress(VK_LBUTTON)) {
						bgmRButton.sprite = arrowDecisionSpriteR;
						SetSpriteScale(bgmRButton.sprite, -1.f / 2, 1.f / 2);
					}
					if (IsKeyRelease(VK_LBUTTON)) {
						PlaySounds(ButtonDecision);
						SetSoundVolume(BGM, 10);
					}
				}
			}
			break;
		case MSE:	// SE選択時
			bgmLButton.sprite = arrowOffSpriteL;
			bgmRButton.sprite = arrowOffSpriteR;
			seLButton.sprite = arrowOnSpriteL;
			seRButton.sprite = arrowOnSpriteR;
			SetSelectCursorSprite(barOnSprite);
			//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);
			SetSpritePos(selectCursor->sprite, seButton.x, seButton.y);

			SetSpriteScale(bgmLButton.sprite, 1.f / 2, 1.f / 2);
			SetSpriteScale(bgmRButton.sprite, -1.f / 2, 1.f / 2);
			SetSpriteScale(seLButton.sprite, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2);
			SetSpriteScale(seRButton.sprite, -EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2, EaseAppearInOutPop(size, 0.9f, 1.0f, 1.0f) / 2);

			//減少
			if (vol->SE > 0) {
				/*キー操作*/
				if (IsKeyPress(VK_LEFT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					seLButton.sprite = arrowDecisionSpriteL;
					SetSpriteScale(seLButton.sprite, 1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_LEFT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(SE, -10);
				}
				/*マウス操作*/
				if (IsMouseOnButton(seLButton)) {
					if (IsKeyPress(VK_LBUTTON)) {
						seLButton.sprite = arrowDecisionSpriteL;
						SetSpriteScale(seLButton.sprite, 1.f / 2, 1.f / 2);
					}
					if (IsKeyRelease(VK_LBUTTON)) {
						PlaySounds(ButtonDecision);
						SetSoundVolume(SE, -10);
					}
				}
			}

			//増加
			if (vol->SE < 100) {
				/*キー操作*/
				if (IsKeyPress(VK_RIGHT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					seRButton.sprite = arrowDecisionSpriteR;
					SetSpriteScale(seRButton.sprite, -1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_RIGHT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(SE, 10);
				}
				/*マウス操作*/
				if (IsMouseOnButton(seRButton)) {
					if (IsKeyPress(VK_LBUTTON)) {
						seRButton.sprite = arrowDecisionSpriteR;
						SetSpriteScale(seRButton.sprite, -1.f / 2, 1.f / 2);
					}
					if (IsKeyRelease(VK_LBUTTON)) {
						PlaySounds(ButtonDecision);
						SetSoundVolume(SE, 10);
					}
				}
			}
			break;
		case MExit:	// Exit選択時
			isQuitFadeOn = true;

			bgmLButton.sprite = arrowOffSpriteL;
			bgmRButton.sprite = arrowOffSpriteR;
			seLButton.sprite = arrowOffSpriteL;
			seRButton.sprite = arrowOffSpriteR;
			SetSelectCursorSprite(exitOnSprite);
			//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);
			SetSpritePos(selectCursor->sprite, backButton.x, backButton.y);

			// 現在のシーンに応じて1つ前の画面に戻る
			/*キー操作*/
			if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
				SetSelectCursorSprite(exitDecisionSprite);
				isQuitFadeOn = false;
				//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);
				SetSpritePos(selectCursor->sprite, backButton.x, backButton.y);
			}
			if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
				isQuitFadeOn = true;
				isTransition = true;
				alpha = 255;
				fadeRate = 1.0f;
				fadeSpeed = 10.f;
				PlaySounds(ButtonDecision);
			}
			/*マウス操作*/
			if (IsMouseOnButton(backButton)) {
				if (IsKeyPress(VK_LBUTTON)) {
					SetSelectCursorSprite(exitDecisionSprite);
					//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);
					SetSpritePos(selectCursor->sprite, backButton.x, backButton.y);
					isQuitFadeOn = false;
				}
				if (IsKeyRelease(VK_LBUTTON)) {
					isQuitFadeOn = true;
					isTransition = true;
					alpha = 255;
					fadeRate = 1.0f;
					fadeSpeed = 10.f;
					PlaySounds(ButtonDecision);
				}
			}
			break;
		}

		// 現在のシーンに応じて1つ前の画面に戻る
		if (IsKeyPush(VK_ESCAPE) || IsPadPush(0, XINPUT_GAMEPAD_START) || IsPadPush(0, XINPUT_GAMEPAD_A)) {
			const char* scene_name = GetSceneName();
			if (strcmp(scene_name, "PreStart") == 0) {
				PlaySounds(ButtonCancel);
				SetGameState(GPlay);
			}
			else if (strcmp(scene_name, "Game") == 0) {
				PlaySounds(ButtonCancel);
				SetGameState(GPause);
			}
			curOptionItemIndex = 0;
			alpha = 255;
			fadeRate = 1.f;
			fadeSpeed = 0.f;
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

	if (isQuitFadeOn)
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, 255));	

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (optionItems[curOptionItemIndex]) {
			case MExit:		// QuitGame選択時
				const char* scene_name = GetSceneName();
				if (strcmp(scene_name, "PreStart") == 0) 
					SetGameState(GPlay);
				else if (strcmp(scene_name, "Game") == 0)
					SetGameState(GPause);
				break;
			}
			isTransition = false;
			blinkTimer = 0.f;
			curOptionItemIndex = 0;
			alpha = 255;
			fadeRate = 1.f;
			fadeSpeed = 0.f;
		}
	}

	// 音量調整の適用
	Sounds_Update();
}

void Option_Render()
{
	const VOLUME* vol = GetVolume();

	RenderBG();

	/*後ろから二番目の背景*/
	RenderSpritePos(optionBG, 61, 34);

	/*オプションロゴ*/
	RenderSpritePos(optionSprite, SCREEN_WIDTH / 2 - 385, 80);

	/*各ボタンの下地*/
	RenderButton(bgmButton);
	RenderButton(seButton);
	RenderButton(backButton);

	/*カーソル選択時*/
	RenderSprite(selectCursor->sprite);

	/*BGM文字*/	
	RenderSprite(bgmSprite);

	/*SE文字*/		
	RenderSprite(seSprite);

	// BGM音量
	DrawBGMVolume(SCREEN_WIDTH / 2 + 80, 258, vol->BGM);
	// SE音量
	DrawBGMVolume(SCREEN_WIDTH / 2 + 80, 408, vol->SE);

	static float size = 0;
	size += DELTA_TIME;
	if (size > 1.f)
		size = 0;

	/*ボタン*/
	if(vol->BGM > 0)
		RenderButton(bgmLButton);
	if (vol->BGM < 100)
		RenderButton(bgmRButton);
	if (vol->SE > 0)
		RenderButton(seLButton);
	if (vol->SE < 100)
		RenderButton(seRButton);
}

void DrawBGMVolume(int x, int y, int volume)
{
	if (volume == 0) RenderSpritePos(numberSprite[0], x, y);
	while (volume)		// 繰上り処理
	{
		RenderSpritePos(numberSprite[volume % 10], x, y);
		x -= 25;
		volume /= 10;
	};
}

void DrawSEVolume(int x, int y, int volume)
{
	if (volume == 0) RenderSpritePos(numberSprite[0], x, y);
	while (volume)		// 繰上り処理
	{
		RenderSpritePos(numberSprite[volume % 10], x, y);
		x -= 25;
		volume /= 10;
	};
}