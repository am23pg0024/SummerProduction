#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "Button.h"
#include "PauseUI.h"
#include "SoundManager.h"
#include "ResultScene.h"

int gameClearBG;
int gameClearLogo;
int nextSprite;

static int alpha;
static float fadeRate;
static float fadeSpeed;

static bool isNextFadeOn;
static bool isTransition;	// ��ʑJ�ڒ����ǂ���
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

void GameClear_Initialize()
{
	gameClearBG = CreateSprite("Assets/Sprites/UI/GameClear/Gameclear_background.png");	// �Q�[���N���A�w�i
	gameClearLogo = CreateSprite("Assets/Sprites/UI/GameClear/Gameclear_rogo.png");		// �Q�[���N���A�̃��S
	nextSprite = CreateSprite("Assets/Sprites/UI/GameClear/Gameclear_next.png");		// ���ɐi�ރL�[�\��

	SetSpriteOrigin(gameClearBG, 434, 306);
	SetSpriteOrigin(gameClearLogo, 357, 61);
	SetSpriteOrigin(nextSprite, 136, 28);

	SetSpritePos(gameClearBG, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	SetSpritePos(gameClearLogo, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 30);
	SetSpritePos(nextSprite, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 170);

	/*�_�ŗp�t�F�[�h*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;
	isNextFadeOn = true;
	blinkTimer = 0.f;
}

void GameClear_Finalize()
{
	ReleaseSprite(gameClearBG);
	ReleaseSprite(gameClearLogo);
	ReleaseSprite(nextSprite);
}

void GameClear_Update()
{
	// SPACE, ENTER, B�{�^���Ń��U���g��ʂ�
	if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
		isNextFadeOn = false;
		alpha = 255;
		fadeRate = 1.0f;
	}
	if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
		isNextFadeOn = true;
		isTransition = true;
		alpha = 255;
		fadeRate = 1.0f;
		fadeSpeed = 10.f;
		StopSounds(Game);
		PlaySounds(ButtonDecision);
	}

	// �I���J�[�\���̓_��
	fadeRate += fadeSpeed * DELTA_TIME;
	alpha = fadeRate * 255;

	// �t�F�[�h�C��
	if (alpha <= 80) {
		alpha = 80;
		if (!isTransition)
			fadeSpeed = 0.8f;
		else
			fadeSpeed = 10.f;
	}
	// �t�F�[�h�A�E�g
	if (alpha >= 255) {
		alpha = 255;
		if (!isTransition)
			fadeSpeed = -0.6f;
		else
			fadeSpeed = -10.f;
	}

	if (isNextFadeOn)
		SetSpriteColor(nextSprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(nextSprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			StopSounds(Game);
			JumpScene(SetupResultScene);
			isTransition = false;
			blinkTimer = 0.f;
		}
	}
}

void GameClear_Render() 
{
	RenderSprite(gameClearBG);
	RenderSprite(gameClearLogo);
	RenderSprite(nextSprite);
}