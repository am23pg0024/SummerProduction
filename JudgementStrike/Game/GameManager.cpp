#include "UnlimitedLib/UnlimitedLib.h"
#include "GameManager.h"
#include "Game.h"
#include "Player.h"
#include "Parameter.h"
#include"EnemyManager.h"

static GAME_MANAGER gm;

const GAME_MANAGER* GetGameManager() { return &gm; }

void CountComboTimer();
int LoadHighScore(const char* filename, unsigned int* score);
int SaveHighScore(const char* filename, unsigned int* score);

void GameManager_Initialize()
{
	const PARAM* param = GetParameter();
	gm.wave = 1;
	gm.spawnNum = param->wave.params[0].spawnNum;
	gm.waveBorder = param->wave.params[0].border;
	gm.highRate = param->wave.params[0].highRate;
	gm.explRate = param->wave.params[0].explRate;

	gm.energyLevel = 1;
	gm.energyGauge = 0;
	gm.maxGauge = param->energyParams[0].maxGauge;

	gm.killCnt = 0;
	gm.timeLimit = param->timeLimit;

	gm.comboCnt = 0;
	gm.maxCombo = 0;
	gm.comboTimer = 0.f;

	gm.score = 0;
	gm.highScore = 0;
	gm.scoreRate = 1.0;

	LoadHighScore("Data/HighScore.dat", &gm.highScore);
}

void GameManager_Finalize()
{
	if (gm.score > gm.highScore) {
		SaveHighScore("Data/HighScore.dat", &gm.score);
		gm.highScore = gm.score;
	}
}

void GameManager_Update()
{
	gm.timeLimit -= DELTA_TIME;

	CountComboTimer();		// 一定時間敵を倒さなければコンボをリセットする

	// 電力ゲージが一定値以上になるとレベルアップ
	if (gm.energyLevel < 4) {
		if (gm.energyGauge >= gm.maxGauge) {
			gm.energyGauge -= gm.maxGauge;
			gm.energyLevel++;

			ENERGY_PARAM* param = (ENERGY_PARAM*)GetParameter()->energyParams;
			gm.maxGauge = param[gm.energyLevel - 1].maxGauge;

			Enhance(param);
		}
	}
	if (gm.energyLevel >= 4)
		gm.energyGauge = 1;

	// ウェーブ処理
	if (gm.wave < 4) {
		if (gm.killCnt >= gm.waveBorder) {
			gm.wave++;
			gm.spawnNum = GetParameter()->wave.params[gm.wave - 1].spawnNum;
			gm.waveBorder += GetParameter()->wave.params[gm.wave - 1].border;
			gm.highRate = GetParameter()->wave.params[gm.wave - 1].highRate;
			gm.explRate = GetParameter()->wave.params[gm.wave - 1].explRate;
		}
	}
}

void AddEnergy()
{
	if (gm.energyLevel < 4)
		gm.energyGauge++;
}

void AddEnergy(int num)
{
	if (gm.energyLevel < 4)
		gm.energyGauge += num;
}

void AddKillCount()
{
	gm.killCnt++;
	AddEnergy();
}

void AddKillCount(int cnt)
{
	gm.killCnt += cnt;
	AddEnergy();
}

void SubTimeLimit(float num)
{
	gm.timeLimit -= num;
}

void AddComboCount()
{
	gm.comboCnt++;
	gm.comboTimer = 0.f;

	if (gm.maxCombo < gm.comboCnt) 
		gm.maxCombo = gm.comboCnt;	// 最大コンボ数更新
}

void AddComboCount(int cnt)
{
	gm.comboCnt += cnt;
	gm.comboTimer = 0.f;

	if (gm.maxCombo < gm.comboCnt)
		gm.maxCombo = gm.comboCnt;	// 最大コンボ数更新
}

void CountComboTimer()
{
	gm.comboTimer += DELTA_TIME;

	if (gm.comboTimer >= 5.0f) {
		gm.comboCnt = 0;
		gm.comboTimer = 0.f;
	}
}

void AddScore(ENEMY* enemy)
{
	// コンボ数によるスコア倍率変更
	if (gm.comboCnt < 50) gm.scoreRate = 1.0;
	else if (gm.comboCnt < 100) gm.scoreRate = 1.5;
	else if (gm.comboCnt < 200) gm.scoreRate = 2.0;
	else gm.scoreRate = 3.0;

	gm.score += enemy->score * gm.scoreRate;
}

// ハイスコアロード
int LoadHighScore(const char* filename, unsigned int* score)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == nullptr) return 1;

	fread(score, sizeof(int), 1, fp);

	fclose(fp);
	return 0;
}

// ハイスコアセーブ
int SaveHighScore(const char* filename, unsigned int* score)
{
	FILE* fp = fopen(filename, "wb");
	if (fp == nullptr) return 1;

	fwrite(score, sizeof(int), 1, fp);

	fclose(fp);
	return 0;
}

