#include "UnlimitedLib/UnlimitedLib.h"
#include "Game.h"
#include "Animation.h"
#include "Player.h"
#include "GameManager.h"
#include "UI.h"
#include "EnemyManager.h"
#include "2DHelperEx.h"
#include "Camera.h"
#include "Easing.h"

UI UIData[UIMAX];			// ��ʓ���UI
UI damage[DRAWDAMAGEMAX];	// ��ʂɃ_���[�W���f��
int damageNumber[10];		// �_���[�W�̐����摜
int comboNumber[10];		// �R���{�̐����摜
int timeNumber[10];			// �^�C�����~�b�g�̐����摜
int timeNumberDot;			// �R����
int comboSprite;			// COMBO!�̕���

int MaruMonicaFont;
int DroidFont;

float beforeEnergy;
 float easingTimer;	// �C�[�W���O�p�̕ϐ�
 bool easing;		// �C�[�W���O���邩�ǂ���
 bool changeTimer;	// �C�[�W���O�^�C�}�[�̎��Ԃ�ύX���邩
 bool levelUp;		// �d�̓Q�[�W�����������ǂ���

void DrawTimeLimit(int x, int y, float num, int Font);
void DrawCount(int x, int y, int num, int Font);
void DrawEnergyNum(int x, int y, int num, int Font);
void DrawEnergyGauge(int energyLevel, float energyGauge, float maxGauge);
void DrawStatus(int HP, int Shield);
void DrawCombo(int x, int y, int num, int Font);
void DrawHighScore(int x, int y, int num, int Font);
void DrawScore(int x, int y, int num, int Font);
void DrawDamage(int x, int y, int num, int Font, int type);
void UpdateDamage();
void EasingUpdate();

void UI_Restart() 
{
	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		damage[i].x = 0;
		damage[i].y = 0;
		damage[i].count = 0;
		damage[i].isActive = false;
	}

	// �C�[�W���O
	beforeEnergy = 0;
	easingTimer = 0;
	easing = false;
	changeTimer = false;
	levelUp = false;
}

void UI_Initialize()
{
	 /*�摜�ǂݍ���*/
	UIData[Hp].sprite = CreateSprite("Assets/Sprites/UI/HP_memory.png");						// HP
	UIData[Shield].sprite = CreateSprite("Assets/Sprites/UI/shield_memory.png");				// �V�[���h
	UIData[StatusBarBlack].sprite = CreateSprite("Assets/Sprites/UI/HP_bar_black.png");			// HP�o�[�̍��w�i
	UIData[StatusBarFrame].sprite = CreateSprite("Assets/Sprites/UI/HP_bar_frame.png");			// HP�o�[�̃t���[��
	UIData[Energy].sprite = CreateSprite("Assets/Sprites/UI/energy_gauge.png");					// �d�̓Q�[�W
	UIData[EnergyFrame].sprite = CreateSprite("Assets/Sprites/UI/energy_frame.png");			// �G�l���M�[�Q�[�W�̃t���[��
	UIData[TimelimitBack].sprite = CreateSprite("Assets/Sprites/UI/Time_background.png");		// �^�C�����~�b�g�̔w�i
	UIData[KillBack].sprite = CreateSprite("Assets/Sprites/UI/Kill_background.png");			// �L�����̔w�i
	CreateSpriteMatrix("Assets/Sprites/UI/Damage_number.png", 22, 37, 10, 1, 10, damageNumber);	// �_���[�W����
	CreateSpriteMatrix("Assets/Sprites/UI/Combo_numbers.png", 26, 40, 10, 1, 10, comboNumber);	// �R���{����
	CreateSpriteMatrix("Assets/Sprites/UI/Time_number.png", 22, 38, 10, 1, 10, timeNumber);		// �^�C�����~�b�g����
	timeNumberDot = CreateSprite("Assets/Sprites/UI/Time_number_dot.png");
	comboSprite = CreateSprite("Assets/Sprites/UI/Combo_letter.png");

	MaruMonicaFont  = CreateFont("Assets/Font/x12y16pxMaruMonica/x12y16pxMaruMonica.tfd");		// MaruMonicaFont
	DroidFont = CreateFont("Assets/Font/DroidSans/DroidSans.tfd");								// DroidFont

	/*HP�f�[�^*/
	UIData[Hp].x = 44;
	UIData[Hp].y = 50;
	UIData[Hp].ox = 11;
	UIData[Hp].oy = 11;

	/*�V�[���h�f�[�^*/
	UIData[Shield].x = 44;
	UIData[Shield].y = 92;
	UIData[Shield].ox = 11;
	UIData[Shield].oy = 11;

	/*�X�e�[�^�X�g���w�i*/
	UIData[StatusBarBlack].x = 190;
	UIData[StatusBarBlack].y = 75;
	UIData[StatusBarBlack].ox = 307;
	UIData[StatusBarBlack].oy = 92;

	/*�X�e�[�^�X�g*/
	UIData[StatusBarFrame].x = 190;
	UIData[StatusBarFrame].y = 75;
	UIData[StatusBarFrame].ox = 311;
	UIData[StatusBarFrame].oy = 88;

	/*�d�̓Q�[�W�g*/
	UIData[EnergyFrame].x = SCREEN_WIDTH - 200;
	UIData[EnergyFrame].y = SCREEN_HEIGHT / 8 + 20;
	UIData[EnergyFrame].ox = 263;
	UIData[EnergyFrame].oy = 55;

	/*�d�̓Q�[�W�f�[�^*/
	UIData[Energy].x = SCREEN_WIDTH - 169;
	UIData[Energy].y = SCREEN_HEIGHT / 8 + 31;
	UIData[Energy].ox = 263;
	UIData[Energy].oy = 55;
	UIData[Energy].count = 0;

	/*�^�C�����~�b�g�f�[�^*/
	UIData[Timelimit].x = SCREEN_WIDTH / 2 + 40;
	UIData[Timelimit].y = 32;

	/*�^�C�����~�b�g�w�i*/
	UIData[TimelimitBack].x = SCREEN_WIDTH / 2 - 96;
	UIData[TimelimitBack].y = -13;

	/*�L�����f�[�^*/
	UIData[Kill].x = 150;
	UIData[Kill].y = SCREEN_HEIGHT - 42;

	/*�L�����w�i*/
	UIData[KillBack].x = 0;
	UIData[KillBack].y = SCREEN_HEIGHT - 48;

	/*�R���{���f�[�^*/
	UIData[Combo].x = SCREEN_WIDTH - 100;
	UIData[Combo].y = SCREEN_HEIGHT / 5 + 10;

	/*�n�C�X�R�A�f�[�^*/
	UIData[HighScore].x = SCREEN_WIDTH - 30;
	UIData[HighScore].y = SCREEN_HEIGHT / 15 - 31;

	/*���݂̃X�R�A�f�[�^*/
	UIData[Score].x = SCREEN_WIDTH - 30;
	UIData[Score].y = SCREEN_HEIGHT / 15;

	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		damage[i].x = 0;
		damage[i].y = 0;
		damage[i].count = 0;
		damage[i].isActive = false;
	}

	// �C�[�W���O
	beforeEnergy = 0;
	easingTimer = 0;
	easing = false;
	changeTimer = false;
	levelUp = false;
}

void UI_Finalize()
{
	ReleaseSprite(UIData[Hp].sprite);
	ReleaseSprite(UIData[Shield].sprite);
	ReleaseSprite(UIData[StatusBarBlack].sprite);
	ReleaseSprite(UIData[StatusBarFrame].sprite);
	ReleaseSprite(UIData[Energy].sprite);
	ReleaseSprite(UIData[EnergyFrame].sprite);
	ReleaseSprite(UIData[TimelimitBack].sprite);
	ReleaseSprite(UIData[KillBack].sprite);
	ReleaseSpriteMatrix(damageNumber, 10);
	ReleaseSpriteMatrix(comboNumber, 10);
	ReleaseSpriteMatrix(timeNumber, 10);
	ReleaseSprite(timeNumberDot);
	ReleaseSprite(comboSprite);

	ReleaseFont(MaruMonicaFont);
	ReleaseFont(DroidFont);
}

void UI_Update()
{
	UpdateDamage();
	EasingUpdate();
}

void UI_Render()
{
	/*���擾*/
	const PLAYER* player = GetPlayer();
	const GAME_MANAGER* gm = GetGameManager();
	const CAMERA* cam = GetCamera();

	/*�eUI�\��*/
	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		if (damage[i].isActive == false) continue;
		if (damage[i].x < cam->x || cam->x + SCREEN_WIDTH < damage[i].x ||
			damage[i].y < cam->y || cam->y + SCREEN_HEIGHT < damage[i].y)
			continue;
		DrawDamage(damage[i].x - cam->x, damage[i].y - cam->y, damage[i].count, DroidFont, i);	// �_���[�W�\�L
	}
	DrawStatus(player->obj.hp, player->shield);														// �X�e�[�^�X
	DrawEnergyGauge(gm->energyLevel, (float)gm->energyGauge, (float)gm->maxGauge);					// �d�̓Q�[�W
	DrawTimeLimit(UIData[Timelimit].x, UIData[Timelimit].y, gm->timeLimit * 100, MaruMonicaFont);	// �^�C�����~�b�g
	DrawEnergyNum(SCREEN_WIDTH - 404, SCREEN_HEIGHT / 7 + 5, gm->energyLevel, MaruMonicaFont);		// �d�̓��x��
	RenderSpritePos(UIData[KillBack].sprite, UIData[KillBack].x, UIData[KillBack].y);				// �L�����w�i
	DrawCount(UIData[Kill].x, UIData[Kill].y, gm->killCnt, MaruMonicaFont);							// �L����
	if (gm->comboCnt > 0)
		DrawCombo(UIData[Combo].x, UIData[Combo].y, gm->comboCnt, MaruMonicaFont);					// �R���{��
	DrawHighScore(UIData[HighScore].x, UIData[HighScore].y, gm->highScore, MaruMonicaFont);			// �n�C�X�R�A
	DrawScore(UIData[Score].x, UIData[Score].y, gm->score, MaruMonicaFont);							// �X�R�A
}

void DrawTimeLimit(int x, int y, float num, int Font)
{
	if (num < 0) num = 0;
	int minutes = (num / 100) / 60;
	int seconds = ((int)num / 100) % 60;
	int mseconds = num - (((minutes * 60) + seconds) * 100);

	RenderSpritePos(UIData[TimelimitBack].sprite, UIData[TimelimitBack].x, UIData[TimelimitBack].y);	

	/*�摜���g���Ă̎��ԕ\��*/
	/*�傫���ݒ�*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 22, 38);
	}
	// �R���}�ȉ��\��
	if (mseconds == 0) {
		RenderSpritePos(timeNumber[0], x + 90, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 90, y);
		x -= 22;
	}
	else if (mseconds < 10) {
		RenderSpritePos(timeNumber[mseconds % 10], x + 90, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 90, y);
		x -= 22;
	}
	else {
		RenderSpritePos(timeNumber[mseconds % 10], x + 90, y);
		mseconds /= 10;
		x -= 22;
		RenderSpritePos(timeNumber[mseconds % 10], x + 90, y);
		x -= 22;
	};
	// �b�\��
	if (seconds == 0) {
		RenderSpritePos(timeNumber[0], x + 60, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 60, y);
		x -= 22;
	}
	else if (seconds < 10) {
		RenderSpritePos(timeNumber[seconds % 10], x + 60, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 60, y);
		x -= 22;
	}
	else {
		RenderSpritePos(timeNumber[seconds % 10], x + 60, y);
		seconds /= 10;
		x -= 22;
		RenderSpritePos(timeNumber[seconds % 10], x + 60, y);
		x -= 22;
	};

	RenderSpritePos(timeNumberDot, x + 64, y + 5);

	// ���\��
	if (minutes == 0) {
		RenderSpritePos(timeNumber[0], x + 30, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 30, y);
		x -= 22;
	}
	else if (minutes < 10) {
		RenderSpritePos(timeNumber[minutes % 10], x + 30, y);
		x -= 22;
		RenderSpritePos(timeNumber[0], x + 30, y);
		x -= 22;
	}
	else {
		RenderSpritePos(timeNumber[minutes % 10], x + 30, y);
		minutes /= 10;
		x -= 22;
		RenderSpritePos(timeNumber[minutes % 10], x + 30, y);
		x -= 22;
	};

	//SetFontScale(Font, 0.7f);
	///*���\��*/
	//if (minutes < 10) RenderFormatText(Font, x - 50, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", minutes);
	//else if (minutes == 0) RenderFormatText(Font, x - (60 * 0.7), y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", minutes);
	//else RenderFormatText(Font, x - (60 * 0.7), y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", minutes);
	//
	///*:�\��*/
	//RenderFormatText(Font, x, y - 3, D3DCOLOR_RGBA(171, 225, 250, 255), ":");

	///*�b�\��*/
	//if (seconds < 10) RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", seconds);
	//else if (seconds == 0) RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", seconds);
	//else RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", seconds);

	///*�R���}�ȉ��̕\��*/
	//if (mseconds < 10) RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", mseconds);
	//else if (mseconds == 0) RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", mseconds);
	//else RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", mseconds);
}

void DrawCount(int x, int y, int num, int Font)
{
	/*�傫���ݒ�*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 22, 37);
	}

	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// �J��菈��
	{
		RenderSpritePos(timeNumber[num % 10], x, y);
		x -= 22;
		num /= 10;
	};

	//SetFontScale(Font, 0.7f);
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawEnergyNum(int x, int y, int num, int Font)
{
	const GAME_MANAGER* gm = GetGameManager();
	float sizeX;
	float sizeY;

	/*�傫���ݒ�*/
	if (easing == true) {
		sizeX = EaseAppearInOutPop(easingTimer, 1.f, 1.5f, 0.5f);
		sizeY = EaseAppearInOutPop(easingTimer, 1.f, 1.5f, 0.5f);
		for (int i = 0; i < 10; i++) {
			SetSpriteOrigin(timeNumber[i], 11, 19);
			SetSpriteScale(timeNumber[i], sizeX, sizeY);
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			SetSpriteOrigin(timeNumber[i], 11, 19);
			SetSpriteScale(timeNumber[i], 1.f, 1.f);
		}
	}

	while (num)		// �J��菈��
	{
		SetSpritePos(timeNumber[num % 10], x, y);
		RenderSprite(timeNumber[num % 10]);
		x -= 22;
		num /= 10;
	};
}

void DrawEnergyGauge(int energyLevel, float energyGauge, float maxGauge)
{
	/*�d�̓Q�[�W�̉��n*/
	RenderSpriteRot(UIData[EnergyFrame].sprite, UIData[EnergyFrame].ox, UIData[EnergyFrame].oy, UIData[EnergyFrame].x, UIData[EnergyFrame].y, 0, 0.8f);

	/*�d�̓Q�[�W�̒��g�̕���*/
	switch (energyLevel)
	{
	case 4:
		UIData[Energy].count = 1;
		break;
	default:
		UIData[Energy].count = energyGauge / maxGauge;
		break;
	}
	float gauge_width = UIData[Energy].count * 464;
	SetSpriteUV(UIData[Energy].sprite, 0, 0, gauge_width, 76);
	SetSpriteSize(UIData[Energy].sprite, gauge_width, 76);
	RenderSpriteRot(UIData[Energy].sprite, UIData[Energy].ox, UIData[Energy].oy, UIData[Energy].x, UIData[Energy].y, 0, 0.8f);
}

void DrawStatus(int HP, int shield)
{
	RenderSpriteRot(UIData[StatusBarBlack].sprite, UIData[StatusBarBlack].ox, UIData[StatusBarBlack].oy, UIData[StatusBarBlack].x, UIData[StatusBarBlack].y, 0, 1.f);	// ���n�̍��w�i
	for (int i = 0; i < HP; i++)
	{
		RenderSpriteRot(UIData[Hp].sprite, UIData[Hp].ox , UIData[Hp].oy, UIData[Hp].x + (8.7 * i), UIData[Hp].y, 0, 1.f);	// HP�̕\��
	}
	for (int j = 0; j < shield; j++)
	{
		RenderSpriteRot(UIData[Shield].sprite, UIData[Shield].ox, UIData[Shield].oy, UIData[Shield].x + (8.7 * j), UIData[Shield].y, 0, 1.f);	// �V�[���h�̑ϋv�l�̕\��
	}
	RenderSpriteRot(UIData[StatusBarFrame].sprite, UIData[StatusBarFrame].ox, UIData[StatusBarFrame].oy, UIData[StatusBarFrame].x, UIData[StatusBarFrame].y, 0, 1.f);	// �t���[������
}

void DrawCombo(int x, int y, int num, int Font)
{
	/*COMBO!�\��*/
	RenderSpritePos(comboSprite, x - 80, y + 45);
	/*�X�R�A�\��*/
	if (num == 0) RenderSpritePos(comboNumber[0], x, y);
	while (num)		// �J��菈��
	{
		RenderSpritePos(comboNumber[num % 10], x, y);
		x -= 20;
		num /= 10;
	};

	///*�傫���ݒ�*/
	//SetFontScale(Font, 1.f);
	///*COMBO!�\��*/
	//RenderFormatText(Font, x - 40, y + 40, D3DCOLOR_RGBA(171, 225, 250, 255), "COMBO!");
	///*�R���{���\��*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawHighScore(int x, int y, int num, int Font)
{
	/*�傫���ݒ�*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 18, 20);
	}

	/*�X�R�A�\��*/
	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// �J��菈��
	{
		RenderSpritePos(timeNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*�傫���ݒ�*/
	//SetFontScale(Font, 0.7f);

	///*�n�C�X�R�A�\��*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawScore(int x, int y, int num, int Font)
{
	/*�傫���ݒ�*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 18, 20);
	}

	/*�X�R�A�\��*/
	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// �J��菈��
	{
		RenderSpritePos(timeNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*�傫���ݒ�*/
	//SetFontScale(Font, 0.7f);

	///*�X�R�A�\��*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawDamage(int x, int y, int num, int Font, int type)
{
	/*�傫���ݒ�*/
	float sizeX = EaseAppearInOutPop(damage[type].timer, 0, 15.f, 1.f);
	float sizeY = EaseAppearInOutPop(damage[type].timer, 0, 17.f, 1.f);
	int clear = 0;
	if(damage[type].timer >= 0.15f)
		clear = EaseAppearInOutPop(damage[type].timer, 0, 255, 0.85f);


	for (int i = 0; i < 10; i++) {
		SetSpriteSize(damageNumber[i], sizeX, sizeY);
		SetSpriteColor(damageNumber[i], D3DCOLOR_RGBA(255, 255, 255, clear));
	}

	/*�X�R�A�\��*/
	if (num == 0) RenderSpritePos(damageNumber[0], x, y);
	while (num)		// �J��菈��
	{

		RenderSpritePos(damageNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*�傫���ݒ�*/
	//SetFontScale(Font, 0.4f);

	///*�X�R�A�\��*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(252, 175, 23, 255), "%d", num)
}

void AddDamageUI(float x, float y, int dmg)
{
	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		if (damage[i].isActive) continue;
		damage[i].x = x;
		damage[i].y = y;
		damage[i].count = dmg;
		damage[i].isActive = true;
		damage[i].timer = 0;
		break;
	}
}

void UpdateDamage()
{
	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		if (damage[i].isActive == false) continue;
		damage[i].timer += DELTA_TIME;
		if (damage[i].timer > 1.f)
			damage[i].isActive = false;
	}
}

void EasingUpdate()
{
	if (beforeEnergy != UIData[Energy].count) {
		beforeEnergy = UIData[Energy].count;
		easing = true;
		levelUp = true;
	}
	else
		levelUp = false;

	if (easing == true) {
		easingTimer += DELTA_TIME;
		if(easingTimer >= 0.25f)
			changeTimer = true;

		if (levelUp == true) {
			if (changeTimer == true) {
				easingTimer = 0.5f - easingTimer;
				changeTimer = false;
			}
			levelUp = false;
		}

		if (easingTimer >= 0.5f) {
			easingTimer = 0;
			easing = false;
			changeTimer = false;
			levelUp = false;
		}
	}
}