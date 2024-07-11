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

BUTTON yesButton, noButton;	// �{�^��

static int alpha;
static float fadeRate;
static float fadeSpeed;
static int mx, my;				// �}�E�X�J�[�\���ʒu
static int prev_mx, prev_my;	// �O��̃}�E�X�J�[�\���ʒu
static bool isYesFadeOn;
static bool isNoFadeOn;
static bool isTransition;	// ��ʑJ�ڒ����ǂ���
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

MENU_ITEM gameOverItems[2] = { MYes, MNo };
int curGameOverItemIndex;

static bool isInput;	// ���X�e�B�b�N���̓t���O
static bool isMouseOn;	// �}�E�X���ǂ��ɏ���Ă��邩

static OBJECT* selectCursor;

void GameOver_Initialize()
{
	gameOverBG = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_background.png");	// �Q�[���I�[�o�[�w�i
	continueLogo = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_continue.png");	// �R���e�B�j���[���S
	gameOverLogo = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_rogo.png");		// �Q�[���I�[�o�[���S
	yesOff = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_off.png");			// ���I������yes
	yesOn = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_on.png");				// �I������yes
	yesDecision = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_yes_desicion.png");	// ���莞��yes
	noOff = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_off.png");				// ���I������no
	noOn = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_on.png");				// �I������no
	noDecision = CreateSprite("Assets/Sprites/UI/GameOver/Gameover_no_desicion.png");	// ���莞��no

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

	/*�_�ŗp�t�F�[�h*/
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

		// �}�E�X�J�[�\���ʒu�擾
		GetMousePosition(&mx, &my);

		// ���X�e�B�b�N���͎擾
		float axisY = GetPadLeftStickY(0);

		// ���j���[���ڑ���i�p�b�h&�L�[�{�[�h�j
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

		// ���X�e�B�b�N���̓t���O����
		if (!isInput) {
			if (axisY != 0)
				isInput = true;
		}
		else {
			if (axisY == 0)
				isInput = false;
		}

		// �J�[�\���ʒu�t���O����
		if (isMouseOn) {
			if (!IsMouseOnButton(yesButton) && !IsMouseOnButton(noButton))
				isMouseOn = false;
		}

		// ���j���[���ڑ���i�}�E�X�j
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
		case MYes:	// Resume�I����
			isYesFadeOn = true;
			yesButton.sprite = yesOn;

			/*�L�[����*/
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

			/*�}�E�X����*/
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
		case MNo:	// Restart�I����			
			isNoFadeOn = true;
			noButton.sprite = noOn;

			/*�L�[����*/
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

			/*�}�E�X����*/
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

	// �I���J�[�\���̓_��
	fadeRate += fadeSpeed * DELTA_TIME;
	alpha = fadeRate * 255;

	// �t�F�[�h�C��
	if (alpha <= 10) {
		alpha = 10;
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
			case MYes:	// Resume�I����
				Restart();
				break;
			case MNo:	// Restart�I����
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