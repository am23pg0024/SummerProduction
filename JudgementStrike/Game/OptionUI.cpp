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

/*�X�v���C�g*/
int optionBG;			// �w�i(��납���Ԗ�)
int optionSprite;		// �I�v�V�������S
int barOffSprite;		// ���I�����̃I�v�V�����o�[
int barOnSprite;		// �I�����̃I�v�V�����o�[
int exitSprite;			// Exit�̕\������X�v���C�g
int exitOffSprite;		// Exit���I�����o�[
int exitOnSprite;		// Exit�I�����o�[
int exitDecisionSprite;	// Exit���莞�o�[
int bgmSprite;			// BGM�̕���
int seSprite;			// SE�̕���
int arrowOffSpriteL;		// ���I�����̖��
int arrowOnSpriteL;		// �I�����̖��
int arrowDecisionSpriteL;// ���莞�̖��
int arrowOffSpriteR;		// ���I�����̖��
int arrowOnSpriteR;		// �I�����̖��
int arrowDecisionSpriteR;// ���莞�̖��
int arrowBGMLSprite;	// BGM�̍����
int arrowBGMRSprite;	// BGM�̉E���
int arrowSELSprite;		// SE�̍����
int arrowSERSprite;		// SE�̉E���
int invisibleSprite;	//�{�^���p(����)
int numberSprite[10];	// ����

static int mx, my;				// �}�E�X�J�[�\���ʒu
static int prev_mx, prev_my;	// �O��̃}�E�X�J�[�\���ʒu

static int alpha;
static float fadeRate;
static float fadeSpeed;
static bool isQuitFadeOn;
static bool isTransition;	// ��ʑJ�ڒ����ǂ���
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

static float size;	// �C�[�W���O�p�̕ϐ�

/*�{�^��*/
BUTTON bgmButton, bgmLButton, bgmRButton, //bgm
       seButton,  seLButton,  seRButton,  //se
	   backButton;                        //back
static OBJECT* selectCursor;

static bool isInput;	// ���X�e�B�b�N���̓t���O
static bool isMouseOn;	// �}�E�X���ǂ��ɏ���Ă��邩

void DrawBGMVolume(int x, int y, int volume);
void DrawSEVolume(int x, int y, int volume);

void Option_Restart()
{
	/*�_�ŗp�t�F�[�h*/
	alpha = 255;
	fadeRate = 1.0f;
	fadeSpeed = 0.0f;

	mx = my = prev_mx = prev_my = 0;

	curOptionItemIndex = 0;
}

void Option_Initialize()
{
	/*�摜�ǂݍ���*/
	optionBG = CreateSprite("Assets/Sprites/Option/Background_02.png");					// �w�i(��납���Ԗ�)
	optionSprite = CreateSprite("Assets/Sprites/Option/Option_rogo.png");							// �I�v�V����
	bgmButton.sprite = CreateSprite("Assets/Sprites/Option/Option_ber_off.png");					// ���I������BGM�o�[
	seButton.sprite = CreateSprite("Assets/Sprites/Option/Option_ber_off.png");						// ���I������SE�o�[
	barOnSprite = CreateSprite("Assets/Sprites/Option/Option_ber_on.png");							// �I�����̃I�v�V�����o�[
	exitOffSprite = CreateSprite("Assets/Sprites/Option/Exit_button_off.png");					// Exit���I�����o�[
	exitOnSprite = CreateSprite("Assets/Sprites/Option/Exit_button_on.png");						// Exit�I�����o�[
	exitDecisionSprite = CreateSprite("Assets/Sprites/Option/Exit_button_desicion.png");			// Exit�I�����o�[
	bgmSprite = CreateSprite("Assets/Sprites/Option/Option_ber_bgm.png");							// BGM�̕���
	seSprite = CreateSprite("Assets/Sprites/Option/Option_ber_se.png");								// SE�̕���
	arrowOffSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_off.png");					// ���I�����̖��
	arrowOnSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_on_nomal.png");				// �I�����̖��
	arrowDecisionSpriteL = CreateSprite("Assets/Sprites/Option/Option_attow_on_desicion.png");		// ���莞�̖��				
	arrowOffSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_off.png");					// ���I�����̖��
	arrowOnSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_on_nomal.png");				// �I�����̖��
	arrowDecisionSpriteR = CreateSprite("Assets/Sprites/Option/Option_attow_on_desicion.png");		// ���莞�̖��				
	CreateSpriteMatrix("Assets/Sprites/Option/Option_numbers.png", 25, 42, 10, 1, 10, numberSprite);// ����


	/*�X�v���C�g�̒��S�ݒ�*/
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

	/*�{�^���쐬*/
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

	/*�_�ŗp�t�F�[�h*/
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
	// ���̃C�[�W���O�p
	size += DELTA_TIME;
	if (size > 1.f)
		size = 0;

	// ���ʎ擾�p
	const VOLUME* vol = GetVolume();

	if (!isTransition) {
		// �}�E�X�J�[�\���ʒu�擾
		GetMousePosition(&mx, &my);

		// ���X�e�B�b�N���͎擾
		float axisY = GetPadLeftStickY(0);

		// ���j���[���ڑ���i�p�b�h&�L�[�{�[�h�j
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
			if (!IsMouseOnButton(bgmButton) && !IsMouseOnButton(seButton) && !IsMouseOnButton(backButton))
				isMouseOn = false;
		}

		// ���j���[���ڑ���i�}�E�X�j
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
		case MBGM:	// BGM�I����
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

			//����
			if (vol->BGM > 0) {
				/*�L�[����*/
				if (IsKeyPress(VK_LEFT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					bgmLButton.sprite = arrowDecisionSpriteL;
					SetSpriteScale(bgmLButton.sprite, 1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_LEFT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(BGM, -10);
				}
				/*�}�E�X����*/
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

			//����
			if (vol->BGM < 100) {
				/*�L�[����*/
				if (IsKeyPress(VK_RIGHT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					bgmRButton.sprite = arrowDecisionSpriteR;
					SetSpriteScale(bgmRButton.sprite, -1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_RIGHT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(BGM, 10);
				}
				/*�}�E�X����*/
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
		case MSE:	// SE�I����
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

			//����
			if (vol->SE > 0) {
				/*�L�[����*/
				if (IsKeyPress(VK_LEFT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					seLButton.sprite = arrowDecisionSpriteL;
					SetSpriteScale(seLButton.sprite, 1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_LEFT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_LEFT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(SE, -10);
				}
				/*�}�E�X����*/
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

			//����
			if (vol->SE < 100) {
				/*�L�[����*/
				if (IsKeyPress(VK_RIGHT) || IsPadPress(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					seRButton.sprite = arrowDecisionSpriteR;
					SetSpriteScale(seRButton.sprite, -1.f / 2, 1.f / 2);
				}
				if (IsKeyRelease(VK_RIGHT) || IsPadRelease(0, XINPUT_GAMEPAD_DPAD_RIGHT)) {
					PlaySounds(ButtonDecision);
					SetSoundVolume(SE, 10);
				}
				/*�}�E�X����*/
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
		case MExit:	// Exit�I����
			isQuitFadeOn = true;

			bgmLButton.sprite = arrowOffSpriteL;
			bgmRButton.sprite = arrowOffSpriteR;
			seLButton.sprite = arrowOffSpriteL;
			seRButton.sprite = arrowOffSpriteR;
			SetSelectCursorSprite(exitOnSprite);
			//SetSpriteScale(selectCursor->sprite, 1.0f, 1.0f);
			SetSpritePos(selectCursor->sprite, backButton.x, backButton.y);

			// ���݂̃V�[���ɉ�����1�O�̉�ʂɖ߂�
			/*�L�[����*/
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
			/*�}�E�X����*/
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

		// ���݂̃V�[���ɉ�����1�O�̉�ʂɖ߂�
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

	if (isQuitFadeOn)
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, alpha));
	else
		SetSpriteColor(selectCursor->sprite, D3DCOLOR_RGBA(255, 255, 255, 255));	

	if (isTransition) {
		blinkTimer += DELTA_TIME;
		if (blinkTimer > MAX_BLINK_TIME) {
			switch (optionItems[curOptionItemIndex]) {
			case MExit:		// QuitGame�I����
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

	// ���ʒ����̓K�p
	Sounds_Update();
}

void Option_Render()
{
	const VOLUME* vol = GetVolume();

	RenderBG();

	/*��납���Ԗڂ̔w�i*/
	RenderSpritePos(optionBG, 61, 34);

	/*�I�v�V�������S*/
	RenderSpritePos(optionSprite, SCREEN_WIDTH / 2 - 385, 80);

	/*�e�{�^���̉��n*/
	RenderButton(bgmButton);
	RenderButton(seButton);
	RenderButton(backButton);

	/*�J�[�\���I����*/
	RenderSprite(selectCursor->sprite);

	/*BGM����*/	
	RenderSprite(bgmSprite);

	/*SE����*/		
	RenderSprite(seSprite);

	// BGM����
	DrawBGMVolume(SCREEN_WIDTH / 2 + 80, 258, vol->BGM);
	// SE����
	DrawBGMVolume(SCREEN_WIDTH / 2 + 80, 408, vol->SE);

	static float size = 0;
	size += DELTA_TIME;
	if (size > 1.f)
		size = 0;

	/*�{�^��*/
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
	while (volume)		// �J��菈��
	{
		RenderSpritePos(numberSprite[volume % 10], x, y);
		x -= 25;
		volume /= 10;
	};
}

void DrawSEVolume(int x, int y, int volume)
{
	if (volume == 0) RenderSpritePos(numberSprite[0], x, y);
	while (volume)		// �J��菈��
	{
		RenderSpritePos(numberSprite[volume % 10], x, y);
		x -= 25;
		volume /= 10;
	};
}