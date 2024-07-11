#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "Game.h"
#include "SoundManager.h"
#include "Button.h"
#include "OptionUI.h"

// �X�v���C�g
int preBG, howToPlay;
int preBackOff, preBackOn, preBackDecision;
int preStartOff, preStartOn, preStartDecision;
int preOptionOff, preOptionOn, preOptionDecision;
int tabOff, tabOn;
int preArrowOffL, preArrowOnL, preArrowDecisionL;
int preArrowOffR, preArrowOnR, preArrowDecisionR;
int padDescript, keyDescript;

// �{�^��
BUTTON preBackButton, preStartButton, preOptionButton;
BUTTON arrowLButton, arrowRButton;

OBJECT tab1, tab2;

MENU_ITEM preItems[3] = { MStart, MOption, MExit };
int curPreItemIndex;
int curPage;
int curArrow;	// 0:L 1:R

static int mx, my;				// �}�E�X�J�[�\���ʒu
static int prev_mx, prev_my;	// �O��̃}�E�X�J�[�\���ʒu

static bool isInputL;	// ���X�e�B�b�N���̓t���O
static bool isInputR;	// �E�X�e�B�b�N���̓t���O
static bool isCancel;	// �{�^������L�����Z���t���O
static bool isMouseOn;	// �}�E�X���{�^���ɏ���Ă��邩
int clickButton;	// �ǂ̃{�^�����N���b�N���Ă��邩

static int buttonAlpha;
static float buttonFadeRate;
static float buttonFadeSpeed;
static int arrowAlpha;
static float arrowFadeRate;
static float arrowFadeSpeed;
static bool isArrowLFadeOn;
static bool isArrowRFadeOn;
static bool isStartFadeOn;
static bool isOptionFadeOn;
static bool isBackFadeOn;
static bool isTransition;	// ��ʑJ�ڒ����ǂ���
static float blinkTimer;
static const float MAX_BLINK_TIME = 1.f;

void PreStart_Reset()
{
	isInputL = isInputR = isCancel = isMouseOn = false;
	clickButton = -1;

	buttonAlpha = arrowAlpha = 255;
	buttonFadeRate = arrowFadeRate = 1.0f;
	buttonFadeSpeed = arrowFadeSpeed = 0.0f;
	isArrowLFadeOn = isArrowRFadeOn = true;
	isStartFadeOn = isOptionFadeOn = isBackFadeOn = false;
	isTransition = false;
	blinkTimer = 0.f;
}

static bool SceneInit()
{
	// �X�v���C�g�ǂݍ���
	preBG = CreateSprite("Assets/Sprites/UI/PreStart/Front_background.png");
	howToPlay = CreateSprite("Assets/Sprites/UI/PreStart/Front_howtoplay.png");

	preBackOff = CreateSprite("Assets/Sprites/UI/PreStart/Front_back_off.png");
	preBackOn = CreateSprite("Assets/Sprites/UI/PreStart/Front_back_on.png");
	preBackDecision = CreateSprite("Assets/Sprites/UI/PreStart/Front_back_decision.png");
	preStartOff = CreateSprite("Assets/Sprites/UI/PreStart/Front_start_off.png");
	preStartOn = CreateSprite("Assets/Sprites/UI/PreStart/Front_start_on.png");
	preStartDecision = CreateSprite("Assets/Sprites/UI/PreStart/Front_start_decision.png");
	preOptionOff = CreateSprite("Assets/Sprites/UI/PreStart/Front_option_off.png");
	preOptionOn = CreateSprite("Assets/Sprites/UI/PreStart/Front_option_on.png");
	preOptionDecision = CreateSprite("Assets/Sprites/UI/PreStart/Front_option_decision.png");

	tabOff = CreateSprite("Assets/Sprites/UI/PreStart/Front_tab_off.png");
	tabOn = CreateSprite("Assets/Sprites/UI/PreStart/Front_tab_on.png");

	preArrowOffL = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_left_normal.png");
	preArrowOnL = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_left_on.png");
	preArrowDecisionL = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_left_decision.png");
	preArrowOffR = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_right_normal.png");
	preArrowOnR = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_right_on.png");
	preArrowDecisionR = CreateSprite("Assets/Sprites/UI/PreStart/Front_arrow_right_decision.png");

	padDescript = CreateSprite("Assets/Sprites/UI/PreStart/Front_controller.png");
	keyDescript = CreateSprite("Assets/Sprites/UI/PreStart/Front_keyboard.png");

	// ��_�ݒ�
	SetSpriteOrigin(preBG, 626, 257);
	SetSpriteOrigin(howToPlay, 138, 34);
	SetSpriteOrigin(preBackOff, 133, 43);
	SetSpriteOrigin(preBackOn, 133, 43);
	SetSpriteOrigin(preBackDecision, 133, 43);
	SetSpriteOrigin(preStartOff, 133, 43);
	SetSpriteOrigin(preStartOn, 133, 43);
	SetSpriteOrigin(preStartDecision, 133, 43);
	SetSpriteOrigin(preOptionOff, 133, 43);
	SetSpriteOrigin(preOptionOn, 133, 43);
	SetSpriteOrigin(preOptionDecision, 133, 43);
	SetSpriteOrigin(tabOff, 8, 8);
	SetSpriteOrigin(tabOn, 10, 10);
	SetSpriteOrigin(preArrowOffL, 62, 62);
	SetSpriteOrigin(preArrowOnL, 62, 62);
	SetSpriteOrigin(preArrowDecisionL, 62, 62);
	SetSpriteOrigin(preArrowOffR, 62, 62);
	SetSpriteOrigin(preArrowOnR, 62, 62);
	SetSpriteOrigin(preArrowDecisionR, 62, 62);
	SetSpriteOrigin(padDescript, 331, 238);
	SetSpriteOrigin(keyDescript, 451, 224);

	// �ʒu�ݒ�
	float offset = 13.5f;
	SetSpritePos(howToPlay, SCREEN_WIDTH / 2, offset + 34);
	SetSpritePos(preBG, SCREEN_WIDTH / 2, offset * 2 + 324);
	SetSpritePos(padDescript, SCREEN_WIDTH / 2, offset * 2 + 324);
	SetSpritePos(keyDescript, SCREEN_WIDTH / 2, offset * 2 + 324);

	// �{�^���ސݒ�
	arrowLButton = CreateButton(preArrowOffL, 100, offset * 2 + 324, 124, 124, true);
	arrowRButton = CreateButton(preArrowOffR, SCREEN_WIDTH - 100, offset * 2 + 324, 124, 124, true);
	preBackButton = CreateButton(preBackOff, 200, SCREEN_HEIGHT - offset - 43, 266, 85, true);
	preStartButton = CreateButton(preStartOff, SCREEN_WIDTH / 2, SCREEN_HEIGHT - offset - 43, 265, 85, true);
	preOptionButton = CreateButton(preOptionOff, SCREEN_WIDTH - 200, SCREEN_HEIGHT - offset - 43, 266, 85, true);

	tab1.sprite = tabOn;
	tab2.sprite = tabOff;
	tab1.x = 165.f;
	tab2.x = 165.f + 22.5f;
	tab1.y = tab2.y = 585.f;

	// �ϐ�������
	mx = my = prev_mx = prev_my = 0;

	curPreItemIndex = curPage = curArrow = 0;

	isInputL = isInputR = isCancel = isMouseOn = false;
	clickButton = -1;

	buttonAlpha = arrowAlpha = 255;
	buttonFadeRate = arrowFadeRate = 1.0f;
	buttonFadeSpeed = arrowFadeSpeed = 0.0f;
	isArrowLFadeOn = isArrowRFadeOn = true;
	isStartFadeOn = isOptionFadeOn = isBackFadeOn = false;
	isTransition = false;
	blinkTimer = 0.f;

	PlaySounds(Title);
	SetGameState(GPlay);

	return true;
}

static void SceneFinal()
{
	ReleaseSprite(preBG);
	ReleaseSprite(howToPlay);
	ReleaseSprite(preBackOff);
	ReleaseSprite(preBackOn);
	ReleaseSprite(preBackDecision);
	ReleaseSprite(preStartOff);
	ReleaseSprite(preStartOn);
	ReleaseSprite(preStartDecision);
	ReleaseSprite(preOptionOff);
	ReleaseSprite(preOptionOn);
	ReleaseSprite(preOptionDecision);
	ReleaseSprite(tabOff);
	ReleaseSprite(tabOn);
	ReleaseSprite(preArrowOffL);
	ReleaseSprite(preArrowOnL);
	ReleaseSprite(preArrowDecisionL);
	ReleaseSprite(preArrowOffR);
	ReleaseSprite(preArrowOnR);
	ReleaseSprite(preArrowDecisionR);
	ReleaseSprite(padDescript);
	ReleaseSprite(keyDescript);
}

static void SceneUpdate()
{
	switch (GetGameState()) {
	case GPlay:
		// �{�^������O
		if (!isTransition) {
			/* �p�b�h���쏈�� */
			// ���X�e�B�b�N���͎擾
			float axisXL = GetPadLeftStickX(0);
			float axisXR = GetPadRightStickX(0);
			float axisY = GetPadLeftStickY(0);

			// ���j���[���ڑ���i�p�b�h&�L�[�{�[�h�j
			// �y�[�W����
			if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || (!isInputR && axisXR < 0)) {
				curArrow = 0;
				arrowAlpha = 255;
				arrowFadeRate = 1.f;
			}
			if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || (!isInputR && axisXR > 0)) {
				curArrow = 1;
				arrowAlpha = 255;
				arrowFadeRate = 1.f;
			}

			// ���j���[����
			if (IsKeyPush(VK_LEFT) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_LEFT) || (!isInputL && axisXL < 0)) {
				// �{�^������L�����Z��
				if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B))
					isCancel = true;

				PlaySounds(MoveCursor);
				buttonAlpha = 255;
				buttonFadeRate = 1.0f;
				curPreItemIndex--;
				if (curPreItemIndex < 0)
					curPreItemIndex += 3;
			}
			if (IsKeyPush(VK_RIGHT) || IsPadPush(0, XINPUT_GAMEPAD_DPAD_RIGHT) || (!isInputL && axisXL > 0)) {
				// �{�^������L�����Z��
				if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B))
					isCancel = true;

				PlaySounds(MoveCursor);
				buttonAlpha = 255;
				buttonFadeRate = 1.0f;
				curPreItemIndex++;
				if (curPreItemIndex > 2)
					curPreItemIndex %= 3;
			}

			/* �}�E�X���쏈�� */
			// �}�E�X�J�[�\���ʒu�擾
			GetMousePosition(&mx, &my);

			// �J�[�\���ʒu�t���O����
			if (isMouseOn) {
				if (!IsMouseOnButton(preStartButton) &&
					!IsMouseOnButton(preOptionButton) &&
					!IsMouseOnButton(preBackButton) &&
					!IsMouseOnButton(arrowLButton) &&
					!IsMouseOnButton(arrowRButton))
				{
					isMouseOn = false;
				}
			}

			// ���j���[���ڑ���i�}�E�X�j
			if (/*mx != prev_mx || my != prev_my || IsKeyPress(VK_LBUTTON)*/1) {
				if (!isMouseOn) {
					if (IsMouseOnButton(preStartButton)) {
						isMouseOn = true;
						if (clickButton == -1) {
							curPreItemIndex = 0;
							buttonAlpha = 255;
							buttonFadeRate = 1.0f;
						}
						if (curPreItemIndex == 0 || clickButton == MStart)
							PlaySounds(MoveCursor);
					}
					else if (IsMouseOnButton(preOptionButton)) {
						isMouseOn = true;
						if (clickButton == -1) {
							curPreItemIndex = 1;
							buttonAlpha = 255;
							buttonFadeRate = 1.0f;
						}
						if (curPreItemIndex == 1 || clickButton == MOption)
							PlaySounds(MoveCursor);
					}
					else if (IsMouseOnButton(preBackButton)) {
						isMouseOn = true;
						if (clickButton == -1) {
							curPreItemIndex = 2;
							buttonAlpha = 255;
							buttonFadeRate = 1.0f;
						}
						if (curPreItemIndex == 2 || clickButton == MExit)
							PlaySounds(MoveCursor);
					}
					else if (IsMouseOnButton(arrowLButton)) {
						isMouseOn = true;
						if (clickButton == -1) {
							curArrow = 0;
							arrowAlpha = 255;
							arrowFadeRate = 1.0f;
						}
						if (curArrow == 0 || clickButton == MArrowL)
							PlaySounds(MoveCursor);
					}
					else if (IsMouseOnButton(arrowRButton)) {
						isMouseOn = true;
						if (clickButton == -1) {
							curArrow = 1;
							arrowAlpha = 255;
							arrowFadeRate = 1.0f;
						}
						if (curArrow == 1 || clickButton == MArrowR)
							PlaySounds(MoveCursor);
					}
				}
				prev_mx = mx;
				prev_my = my;
			}

			preStartButton.sprite = preStartOff;
			preOptionButton.sprite = preOptionOff;
			preBackButton.sprite = preBackOff;
			arrowLButton.sprite = preArrowOffL;
			arrowRButton.sprite = preArrowOffR;
			isStartFadeOn = isOptionFadeOn = isBackFadeOn = false;
			isArrowLFadeOn = isArrowRFadeOn = true;

			if (!IsKeyPress(VK_LBUTTON))
				clickButton = -1;
			//if (IsKeyRelease(VK_LBUTTON))
			//	isMouseOn = false;

			// ���I��������
			switch (curArrow) {
			case 0:
				arrowLButton.sprite = preArrowOnL;
				isArrowLFadeOn = false;

				// �p�b�h�E�L�[����
				// �I��
				if (IsKeyPush(VK_UP) || IsPadPush(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || (!isInputR && axisXR < 0)) {
					PlaySounds(ButtonDecision);
					arrowAlpha = 255;
					arrowFadeRate = 1.f;
					curPage--;
					if (curPage < 0)
						curPage += 2;
				}
				// �I��
				if (IsKeyPress(VK_UP) || IsPadPress(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || axisXR < 0) {
					arrowLButton.sprite = preArrowDecisionL;
				}

				// �}�E�X����
				if (IsMouseOnButton(arrowLButton)) {
					arrowLButton.sprite = preArrowOnL;

					// �I��
					if (IsKeyPush(VK_LBUTTON)) {
						curPage--;
						if (curPage < 0)
							curPage += 2;
						arrowAlpha = 255;
						arrowFadeRate = 1.f;
						PlaySounds(ButtonDecision);
					}
					// �I��
					if (IsKeyPress(VK_LBUTTON)) {
						arrowLButton.sprite = preArrowDecisionL;
						if (clickButton == -1)
							clickButton = MArrowL;
					}
				}

				break;
			case 1:
				arrowRButton.sprite = preArrowOnR;
				isArrowRFadeOn = false;

				// �p�b�h�E�L�[����
				// �I��
				if (IsKeyPush(VK_DOWN) || IsPadPush(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || (!isInputR && axisXR > 0)) {
					PlaySounds(ButtonDecision);
					arrowAlpha = 255;
					arrowFadeRate = 1.f;
					curPage++;
					if (curPage > 1)
						curPage %= 2;
				}
				// �I��
				if (IsKeyPress(VK_DOWN) || IsPadPress(0, XINPUT_GAMEPAD_RIGHT_SHOULDER) || axisXR > 0) {
					arrowRButton.sprite = preArrowDecisionR;
				}

				// �}�E�X����
				if (IsMouseOnButton(arrowRButton)) {
					arrowRButton.sprite = preArrowOnR;

					// �I��
					if (IsKeyPush(VK_LBUTTON)) {
						curPage++;
						if (curPage > 1)
							curPage %= 2;
						arrowAlpha = 255;
						arrowFadeRate = 1.f;
						PlaySounds(ButtonDecision);
					}
					// �I��
					if (IsKeyPress(VK_LBUTTON)) {
						arrowRButton.sprite = preArrowDecisionR;
						if (clickButton == -1)
							clickButton = MArrowR;
					}
				}
				break;
			}

			// �e���j���[���ڑI��������
			switch (preItems[curPreItemIndex]) {
			case MStart:	// Start�I����
				preStartButton.sprite = preStartOn;
				isStartFadeOn = true;

				// �p�b�h�E�L�[����
				if (!isCancel) {
					if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
						preStartButton.sprite = preStartDecision;
						isStartFadeOn = false;
					}
				}
				// �I��
				if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
					if (!isCancel) {
						isStartFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
						StopSounds(Title);
					}
					else {
						isCancel = false;
					}
				}

				// �}�E�X����
				if (IsMouseOnButton(preStartButton)) {
					preStartButton.sprite = preStartOn;

					if (IsKeyPress(VK_LBUTTON)) {
						preStartButton.sprite = preStartDecision;
						isStartFadeOn = false;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						if (clickButton == -1)
							clickButton = MStart;
					}
					// �I��
					if (IsKeyRelease(VK_LBUTTON)) {
						isStartFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
						StopSounds(Title);
					}
				}
				break;
			case MOption:	// Option�I����
				preOptionButton.sprite = preOptionOn;
				isOptionFadeOn = true;

				// �p�b�h�E�L�[����
				if (!isCancel) {
					if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
						preOptionButton.sprite = preOptionDecision;
						isOptionFadeOn = false;
					}
				}
				// �I��
				if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
					if (!isCancel) {
						isOptionFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
					}
					else {
						isCancel = false;
					}
				}

				// �}�E�X����
				if (IsMouseOnButton(preOptionButton)) {
					preOptionButton.sprite = preOptionOn;

					if (IsKeyPress(VK_LBUTTON)) {
						preOptionButton.sprite = preOptionDecision;
						isOptionFadeOn = false;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						if (clickButton == -1)
							clickButton = MOption;
					}
					// �I��
					if (IsKeyRelease(VK_LBUTTON)) {
						isOptionFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
					}
				}
				break;
			case MExit:	// QuitGame�I����
				preBackButton.sprite = preBackOn;
				isBackFadeOn = true;

				// �p�b�h�E�L�[����
				if (!isCancel) {
					if (IsKeyPress(VK_RETURN) || IsPadPress(0, XINPUT_GAMEPAD_B)) {
						preBackButton.sprite = preBackDecision;
						isBackFadeOn = false;
					}
				}
				// �I��
				if (IsKeyRelease(VK_RETURN) || IsPadRelease(0, XINPUT_GAMEPAD_B)) {
					if (!isCancel) {
						isBackFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
						StopSounds(Title);
					}
					else {
						isCancel = false;
					}
				}

				// �}�E�X����
				if (IsMouseOnButton(preBackButton)) {
					preBackButton.sprite = preBackOn;

					if (IsKeyPress(VK_LBUTTON)) {
						preBackButton.sprite = preBackDecision;
						isBackFadeOn = false;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						if (clickButton == -1)
							clickButton = MExit;
					}
					// �I��
					if (IsKeyRelease(VK_LBUTTON)) {
						isBackFadeOn = true;
						isTransition = true;
						buttonAlpha = 255;
						buttonFadeRate = 1.0f;
						buttonFadeSpeed = 10.f;
						PlaySounds(ButtonDecision);
						StopSounds(Title);
					}
				}
				break;
			}

			// �X�e�B�b�N���̓t���O����
			if (!isInputL) {
				if (axisXL != 0)
					isInputL = true;
			}
			else {
				if (axisXL == 0)
					isInputL = false;
			}
			if (!isInputR) {
				if (axisXR != 0)
					isInputR = true;
			}
			else {
				if (axisXR == 0)
					isInputR = false;
			}
		}
		// �{�^�������
		else {
			switch (preItems[curPreItemIndex]) {
			case MStart:	// Start�I����
				preStartButton.sprite = preStartOn;
				break;
			case MOption:	// Start�I����
				preOptionButton.sprite = preOptionOn;
				break;
			case MExit:		// QuitGame�I����
				preBackButton.sprite = preBackOn;
				break;
			}
		}

		// �I���J�[�\���̓_��
		// �{�^��
		buttonFadeRate += buttonFadeSpeed * DELTA_TIME;
		buttonAlpha = buttonFadeRate * 255;
		// �t�F�[�h�C��
		if (buttonAlpha <= 10) {
			buttonAlpha = 10;
			if (!isTransition)
				buttonFadeSpeed = 0.8f;
			else
				buttonFadeSpeed = 10.f;
		}
		// �t�F�[�h�A�E�g
		if (buttonAlpha >= 255) {
			buttonAlpha = 255;
			if (!isTransition)
				buttonFadeSpeed = -0.6f;
			else
				buttonFadeSpeed = -10.f;
		}

		// ���
		arrowFadeRate += arrowFadeSpeed * DELTA_TIME;
		arrowAlpha = arrowFadeRate * 255;
		// �t�F�[�h�C��
		if (arrowAlpha <= 10) {
			arrowAlpha = 10;
			arrowFadeSpeed = 0.8f;
		}
		// �t�F�[�h�A�E�g
		if (arrowAlpha >= 255) {
			arrowAlpha = 255;
			arrowFadeSpeed = -0.6f;
		}

		if (isArrowLFadeOn)
			SetSpriteColor(arrowLButton.sprite, D3DCOLOR_RGBA(255, 255, 255, arrowAlpha));
		else
			SetSpriteColor(arrowLButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

		if (isArrowRFadeOn)
			SetSpriteColor(arrowRButton.sprite, D3DCOLOR_RGBA(255, 255, 255, arrowAlpha));
		else
			SetSpriteColor(arrowRButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

		if (isStartFadeOn)
			SetSpriteColor(preStartButton.sprite, D3DCOLOR_RGBA(255, 255, 255, buttonAlpha));
		else
			SetSpriteColor(preStartButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

		if (isOptionFadeOn)
			SetSpriteColor(preOptionButton.sprite, D3DCOLOR_RGBA(255, 255, 255, buttonAlpha));
		else
			SetSpriteColor(preOptionButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

		if (isBackFadeOn)
			SetSpriteColor(preBackButton.sprite, D3DCOLOR_RGBA(255, 255, 255, buttonAlpha));
		else
			SetSpriteColor(preBackButton.sprite, D3DCOLOR_RGBA(255, 255, 255, 255));

		if (isTransition) {
			blinkTimer += DELTA_TIME;
			if (blinkTimer > MAX_BLINK_TIME) {
				PreStart_Reset();

				switch (preItems[curPreItemIndex]) {
				case MStart:	// Start�I����
					JumpScene(SetupGameScene);
					break;
				case MOption:	// Option�I����
					SetGameState(GOption);
					break;
				case MExit:	// Back�I����
					JumpScene(SetupTitleScene);
					break;
				}
			}
		}
		break;
	case GOption:/*�I�v�V������*/
		Option_Update();
		break;
	}
}

static void SceneRender()
{
	RenderBG();

	switch (GetGameState()) {
	case GPlay:
		RenderBG();
		RenderSprite(howToPlay);
		RenderSprite(preBG);
		RenderButton(arrowLButton);
		RenderButton(arrowRButton);
		RenderButton(preBackButton);
		RenderButton(preStartButton);
		RenderButton(preOptionButton);

		tab1.sprite = tabOff;
		tab2.sprite = tabOff;
		switch (curPage) {
		case 0:
			RenderSprite(keyDescript);
			tab1.sprite = tabOn;
			break;
		case 1:
			RenderSprite(padDescript);
			tab2.sprite = tabOn;
			break;
		}
		SetSpritePos(tab1.sprite, tab1.x, tab1.y);
		RenderSprite(tab1.sprite);
		SetSpritePos(tab2.sprite, tab2.x, tab2.y);
		RenderSprite(tab2.sprite);
		break;
	case GOption:
		Option_Render();
		break;
	}
}

SceneProc SetupPreStartScene()
{
	SceneProc proc{
		"PreStart",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}
