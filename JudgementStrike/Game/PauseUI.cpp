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
static int mx, my;				// �}�E�X�J�[�\���ʒu
static int prev_mx, prev_my;	// �O��̃}�E�X�J�[�\���ʒu
static bool isSelectFadeOn;
static bool isTransition;	// ��ʑJ�ڒ����ǂ���
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

MENU_ITEM pauseItems[4] = { MResume, MRestart, MOption, MExit };
int curPauseItemIndex;

static bool isInput;	// ���X�e�B�b�N���̓t���O
static bool isMouseOn;	// �}�E�X���ǂ��ɏ���Ă��邩

// �{�^��
BUTTON resumeButton, restartButton, optionButton, quitButton;

static OBJECT* selectCursor;
void Pause_Restart()
{
	/*�_�ŗp�t�F�[�h*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;

	mx = my = prev_mx = prev_my = 0;

	curPauseItemIndex = 0;
}

void Pause_Initialize()
{
	/*�摜�ǂݍ���*/
	pauseSprite = CreateSprite("Assets/Sprites/Pause/Pause_rogo.png");						// �|�[�Y���S
	pauseBackSprite = CreateSprite("Assets/Sprites/Pause/Pause_background.png");			// �|�[�Y��ʔw�i
	resumeButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_resume_game.png");		// �ĊJ
	restartButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_restart.png");			// �ăX�^�[�g
	optionButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_option.png");			// �I�v�V����
	quitButton.sprite = CreateSprite("Assets/Sprites/Pause/Pause_quit.png");				// �I��
	backLightSprite = CreateSprite("Assets/Sprites/Pause/Pause_backLight_nomal.png");	// �o�b�N���C�g
	backLightDecisionSprite = CreateSprite("Assets/Sprites/Pause/Pause_backlight_desicion.png");// �o�b�N���C�g(�����Ă�Ƃ�)
	backHexSprite = CreateSprite("Assets/Sprites/Pause/Pause_backlight_hex.png");					// ���̘Z�p�`

	SetSpriteOrigin(resumeButton.sprite, 167, 34);
	SetSpriteOrigin(restartButton.sprite, 84, 34);
	SetSpriteOrigin(optionButton.sprite, 72, 34);
	SetSpriteOrigin(quitButton.sprite, 44, 34);
	SetSpriteOrigin(backHexSprite, 21, 25);
	SetSpriteOrigin(backLightSprite, 243, 34);
	SetSpriteOrigin(backLightDecisionSprite, 243, 34);

	/*�{�^���쐬*/
	resumeButton = CreateButton(resumeButton.sprite, SCREEN_WIDTH / 2, 305, 486, 68, true);
	restartButton = CreateButton(restartButton.sprite, SCREEN_WIDTH / 2, resumeButton.y + 94, 320, 68, true);
	optionButton = CreateButton(optionButton.sprite, SCREEN_WIDTH / 2, restartButton.y + 94, 296, 68, true);
	quitButton = CreateButton(quitButton.sprite, SCREEN_WIDTH / 2, optionButton.y + 94, 240, 68, true);

	/*�_�ŗp�t�F�[�h*/
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
		// �}�E�X�J�[�\���ʒu�擾
		GetMousePosition(&mx, &my);

		// ���X�e�B�b�N���͎擾
		float axisY = GetPadLeftStickY(0);

		// ���j���[���ڑ���i�p�b�h&�L�[�{�[�h�j
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
			if (!IsMouseOnButton(resumeButton) && !IsMouseOnButton(optionButton) && !IsMouseOnButton(restartButton) && !IsMouseOnButton(quitButton))
				isMouseOn = false;
		}

		// ���j���[���ڑ���i�}�E�X�j
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
		case MResume:	// Resume�I����
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, resumeButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, resumeButton.x, resumeButton.y);

			/*�L�[����*/
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

			/*�}�E�X����*/
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
		case MRestart:	// Restart�I����
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, restartButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, restartButton.x, restartButton.y);

			/*�L�[����*/
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

			/*�}�E�X����*/
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
		case MOption:	// Option�I����
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, optionButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, optionButton.x, optionButton.y);

			/*�L�[����*/
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

			/*�}�E�X����*/
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
		case MExit:	// Exit�I����
			SetSelectCursorSprite(backLightSprite);
			SetSpriteScale(selectCursor->sprite, quitButton.width / GetSpriteTextureWidth(selectCursor->sprite), 1.f);
			SetSpritePos(selectCursor->sprite, quitButton.x, quitButton.y);

			/*�L�[����*/
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
			/*�}�E�X����*/
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

		// �Q�[����ʂɖ߂�
		if (IsKeyPush(VK_ESCAPE) || IsPadPush(0, XINPUT_GAMEPAD_START) || IsPadRelease(0, XINPUT_GAMEPAD_A)) {
			PlaySounds(ButtonCancel);
			SetGameState(GPlay);

			curPauseItemIndex = 0;
			alpha = 255;
			fadeRate = 1.f;
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

	if(isSelectFadeOn)
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (pauseItems[curPauseItemIndex]) {
			case MResume:	// Resume�I����
				SetGameState(GPlay);
				SetSelectCursorSprite(backLightSprite);
				break;
			case MRestart:	// Restart�I����
				Restart();
				SetSelectCursorSprite(backLightSprite);
				break;
			case MOption:	// Option�I����
				SetGameState(GOption);
				SetSelectCursorSprite(backLightSprite);
				alpha = 255;
				fadeRate = 1.f;
				fadeSpeed = 0.f;
				break;
			case MExit:		// QuitGame�I����
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

	switch (pauseItems[curPauseItemIndex])	// �Z�p�`�\��
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
