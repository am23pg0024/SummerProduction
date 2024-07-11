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

UI UIData[UIMAX];			// 画面内のUI
UI damage[DRAWDAMAGEMAX];	// 画面にダメージを映す
int damageNumber[10];		// ダメージの数字画像
int comboNumber[10];		// コンボの数字画像
int timeNumber[10];			// タイムリミットの数字画像
int timeNumberDot;			// コロン
int comboSprite;			// COMBO!の文字

int MaruMonicaFont;
int DroidFont;

float beforeEnergy;
 float easingTimer;	// イージング用の変数
 bool easing;		// イージングするかどうか
 bool changeTimer;	// イージングタイマーの時間を変更するか
 bool levelUp;		// 電力ゲージが増えたかどうか

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

	// イージング
	beforeEnergy = 0;
	easingTimer = 0;
	easing = false;
	changeTimer = false;
	levelUp = false;
}

void UI_Initialize()
{
	 /*画像読み込み*/
	UIData[Hp].sprite = CreateSprite("Assets/Sprites/UI/HP_memory.png");						// HP
	UIData[Shield].sprite = CreateSprite("Assets/Sprites/UI/shield_memory.png");				// シールド
	UIData[StatusBarBlack].sprite = CreateSprite("Assets/Sprites/UI/HP_bar_black.png");			// HPバーの黒背景
	UIData[StatusBarFrame].sprite = CreateSprite("Assets/Sprites/UI/HP_bar_frame.png");			// HPバーのフレーム
	UIData[Energy].sprite = CreateSprite("Assets/Sprites/UI/energy_gauge.png");					// 電力ゲージ
	UIData[EnergyFrame].sprite = CreateSprite("Assets/Sprites/UI/energy_frame.png");			// エネルギーゲージのフレーム
	UIData[TimelimitBack].sprite = CreateSprite("Assets/Sprites/UI/Time_background.png");		// タイムリミットの背景
	UIData[KillBack].sprite = CreateSprite("Assets/Sprites/UI/Kill_background.png");			// キル数の背景
	CreateSpriteMatrix("Assets/Sprites/UI/Damage_number.png", 22, 37, 10, 1, 10, damageNumber);	// ダメージ数字
	CreateSpriteMatrix("Assets/Sprites/UI/Combo_numbers.png", 26, 40, 10, 1, 10, comboNumber);	// コンボ数字
	CreateSpriteMatrix("Assets/Sprites/UI/Time_number.png", 22, 38, 10, 1, 10, timeNumber);		// タイムリミット数字
	timeNumberDot = CreateSprite("Assets/Sprites/UI/Time_number_dot.png");
	comboSprite = CreateSprite("Assets/Sprites/UI/Combo_letter.png");

	MaruMonicaFont  = CreateFont("Assets/Font/x12y16pxMaruMonica/x12y16pxMaruMonica.tfd");		// MaruMonicaFont
	DroidFont = CreateFont("Assets/Font/DroidSans/DroidSans.tfd");								// DroidFont

	/*HPデータ*/
	UIData[Hp].x = 44;
	UIData[Hp].y = 50;
	UIData[Hp].ox = 11;
	UIData[Hp].oy = 11;

	/*シールドデータ*/
	UIData[Shield].x = 44;
	UIData[Shield].y = 92;
	UIData[Shield].ox = 11;
	UIData[Shield].oy = 11;

	/*ステータス枠黒背景*/
	UIData[StatusBarBlack].x = 190;
	UIData[StatusBarBlack].y = 75;
	UIData[StatusBarBlack].ox = 307;
	UIData[StatusBarBlack].oy = 92;

	/*ステータス枠*/
	UIData[StatusBarFrame].x = 190;
	UIData[StatusBarFrame].y = 75;
	UIData[StatusBarFrame].ox = 311;
	UIData[StatusBarFrame].oy = 88;

	/*電力ゲージ枠*/
	UIData[EnergyFrame].x = SCREEN_WIDTH - 200;
	UIData[EnergyFrame].y = SCREEN_HEIGHT / 8 + 20;
	UIData[EnergyFrame].ox = 263;
	UIData[EnergyFrame].oy = 55;

	/*電力ゲージデータ*/
	UIData[Energy].x = SCREEN_WIDTH - 169;
	UIData[Energy].y = SCREEN_HEIGHT / 8 + 31;
	UIData[Energy].ox = 263;
	UIData[Energy].oy = 55;
	UIData[Energy].count = 0;

	/*タイムリミットデータ*/
	UIData[Timelimit].x = SCREEN_WIDTH / 2 + 40;
	UIData[Timelimit].y = 32;

	/*タイムリミット背景*/
	UIData[TimelimitBack].x = SCREEN_WIDTH / 2 - 96;
	UIData[TimelimitBack].y = -13;

	/*キル数データ*/
	UIData[Kill].x = 150;
	UIData[Kill].y = SCREEN_HEIGHT - 42;

	/*キル数背景*/
	UIData[KillBack].x = 0;
	UIData[KillBack].y = SCREEN_HEIGHT - 48;

	/*コンボ数データ*/
	UIData[Combo].x = SCREEN_WIDTH - 100;
	UIData[Combo].y = SCREEN_HEIGHT / 5 + 10;

	/*ハイスコアデータ*/
	UIData[HighScore].x = SCREEN_WIDTH - 30;
	UIData[HighScore].y = SCREEN_HEIGHT / 15 - 31;

	/*現在のスコアデータ*/
	UIData[Score].x = SCREEN_WIDTH - 30;
	UIData[Score].y = SCREEN_HEIGHT / 15;

	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		damage[i].x = 0;
		damage[i].y = 0;
		damage[i].count = 0;
		damage[i].isActive = false;
	}

	// イージング
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
	/*情報取得*/
	const PLAYER* player = GetPlayer();
	const GAME_MANAGER* gm = GetGameManager();
	const CAMERA* cam = GetCamera();

	/*各UI表示*/
	for (int i = 0; i < DRAWDAMAGEMAX; i++) {
		if (damage[i].isActive == false) continue;
		if (damage[i].x < cam->x || cam->x + SCREEN_WIDTH < damage[i].x ||
			damage[i].y < cam->y || cam->y + SCREEN_HEIGHT < damage[i].y)
			continue;
		DrawDamage(damage[i].x - cam->x, damage[i].y - cam->y, damage[i].count, DroidFont, i);	// ダメージ表記
	}
	DrawStatus(player->obj.hp, player->shield);														// ステータス
	DrawEnergyGauge(gm->energyLevel, (float)gm->energyGauge, (float)gm->maxGauge);					// 電力ゲージ
	DrawTimeLimit(UIData[Timelimit].x, UIData[Timelimit].y, gm->timeLimit * 100, MaruMonicaFont);	// タイムリミット
	DrawEnergyNum(SCREEN_WIDTH - 404, SCREEN_HEIGHT / 7 + 5, gm->energyLevel, MaruMonicaFont);		// 電力レベル
	RenderSpritePos(UIData[KillBack].sprite, UIData[KillBack].x, UIData[KillBack].y);				// キル数背景
	DrawCount(UIData[Kill].x, UIData[Kill].y, gm->killCnt, MaruMonicaFont);							// キル数
	if (gm->comboCnt > 0)
		DrawCombo(UIData[Combo].x, UIData[Combo].y, gm->comboCnt, MaruMonicaFont);					// コンボ数
	DrawHighScore(UIData[HighScore].x, UIData[HighScore].y, gm->highScore, MaruMonicaFont);			// ハイスコア
	DrawScore(UIData[Score].x, UIData[Score].y, gm->score, MaruMonicaFont);							// スコア
}

void DrawTimeLimit(int x, int y, float num, int Font)
{
	if (num < 0) num = 0;
	int minutes = (num / 100) / 60;
	int seconds = ((int)num / 100) % 60;
	int mseconds = num - (((minutes * 60) + seconds) * 100);

	RenderSpritePos(UIData[TimelimitBack].sprite, UIData[TimelimitBack].x, UIData[TimelimitBack].y);	

	/*画像を使っての時間表示*/
	/*大きさ設定*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 22, 38);
	}
	// コンマ以下表示
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
	// 秒表示
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

	// 分表示
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
	///*分表示*/
	//if (minutes < 10) RenderFormatText(Font, x - 50, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", minutes);
	//else if (minutes == 0) RenderFormatText(Font, x - (60 * 0.7), y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", minutes);
	//else RenderFormatText(Font, x - (60 * 0.7), y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", minutes);
	//
	///*:表示*/
	//RenderFormatText(Font, x, y - 3, D3DCOLOR_RGBA(171, 225, 250, 255), ":");

	///*秒表示*/
	//if (seconds < 10) RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", seconds);
	//else if (seconds == 0) RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", seconds);
	//else RenderFormatText(Font, x + 20, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", seconds);

	///*コンマ以下の表示*/
	//if (mseconds < 10) RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "0%d", mseconds);
	//else if (mseconds == 0) RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "00", mseconds);
	//else RenderFormatText(Font, x + 90, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", mseconds);
}

void DrawCount(int x, int y, int num, int Font)
{
	/*大きさ設定*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 22, 37);
	}

	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// 繰上り処理
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

	/*大きさ設定*/
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

	while (num)		// 繰上り処理
	{
		SetSpritePos(timeNumber[num % 10], x, y);
		RenderSprite(timeNumber[num % 10]);
		x -= 22;
		num /= 10;
	};
}

void DrawEnergyGauge(int energyLevel, float energyGauge, float maxGauge)
{
	/*電力ゲージの下地*/
	RenderSpriteRot(UIData[EnergyFrame].sprite, UIData[EnergyFrame].ox, UIData[EnergyFrame].oy, UIData[EnergyFrame].x, UIData[EnergyFrame].y, 0, 0.8f);

	/*電力ゲージの中身の部分*/
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
	RenderSpriteRot(UIData[StatusBarBlack].sprite, UIData[StatusBarBlack].ox, UIData[StatusBarBlack].oy, UIData[StatusBarBlack].x, UIData[StatusBarBlack].y, 0, 1.f);	// 下地の黒背景
	for (int i = 0; i < HP; i++)
	{
		RenderSpriteRot(UIData[Hp].sprite, UIData[Hp].ox , UIData[Hp].oy, UIData[Hp].x + (8.7 * i), UIData[Hp].y, 0, 1.f);	// HPの表示
	}
	for (int j = 0; j < shield; j++)
	{
		RenderSpriteRot(UIData[Shield].sprite, UIData[Shield].ox, UIData[Shield].oy, UIData[Shield].x + (8.7 * j), UIData[Shield].y, 0, 1.f);	// シールドの耐久値の表示
	}
	RenderSpriteRot(UIData[StatusBarFrame].sprite, UIData[StatusBarFrame].ox, UIData[StatusBarFrame].oy, UIData[StatusBarFrame].x, UIData[StatusBarFrame].y, 0, 1.f);	// フレーム部分
}

void DrawCombo(int x, int y, int num, int Font)
{
	/*COMBO!表示*/
	RenderSpritePos(comboSprite, x - 80, y + 45);
	/*スコア表示*/
	if (num == 0) RenderSpritePos(comboNumber[0], x, y);
	while (num)		// 繰上り処理
	{
		RenderSpritePos(comboNumber[num % 10], x, y);
		x -= 20;
		num /= 10;
	};

	///*大きさ設定*/
	//SetFontScale(Font, 1.f);
	///*COMBO!表示*/
	//RenderFormatText(Font, x - 40, y + 40, D3DCOLOR_RGBA(171, 225, 250, 255), "COMBO!");
	///*コンボ数表示*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawHighScore(int x, int y, int num, int Font)
{
	/*大きさ設定*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 18, 20);
	}

	/*スコア表示*/
	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// 繰上り処理
	{
		RenderSpritePos(timeNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*大きさ設定*/
	//SetFontScale(Font, 0.7f);

	///*ハイスコア表示*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawScore(int x, int y, int num, int Font)
{
	/*大きさ設定*/
	for (int i = 0; i < 10; i++) {
		SetSpriteSize(timeNumber[i], 18, 20);
	}

	/*スコア表示*/
	if (num == 0) RenderSpritePos(timeNumber[0], x, y);
	while (num)		// 繰上り処理
	{
		RenderSpritePos(timeNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*大きさ設定*/
	//SetFontScale(Font, 0.7f);

	///*スコア表示*/
	//RenderFormatText(Font, x, y, D3DCOLOR_RGBA(171, 225, 250, 255), "%d", num);
}

void DrawDamage(int x, int y, int num, int Font, int type)
{
	/*大きさ設定*/
	float sizeX = EaseAppearInOutPop(damage[type].timer, 0, 15.f, 1.f);
	float sizeY = EaseAppearInOutPop(damage[type].timer, 0, 17.f, 1.f);
	int clear = 0;
	if(damage[type].timer >= 0.15f)
		clear = EaseAppearInOutPop(damage[type].timer, 0, 255, 0.85f);


	for (int i = 0; i < 10; i++) {
		SetSpriteSize(damageNumber[i], sizeX, sizeY);
		SetSpriteColor(damageNumber[i], D3DCOLOR_RGBA(255, 255, 255, clear));
	}

	/*スコア表示*/
	if (num == 0) RenderSpritePos(damageNumber[0], x, y);
	while (num)		// 繰上り処理
	{

		RenderSpritePos(damageNumber[num % 10], x, y);
		x -= 18;
		num /= 10;
	};

	///*大きさ設定*/
	//SetFontScale(Font, 0.4f);

	///*スコア表示*/
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