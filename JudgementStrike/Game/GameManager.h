#pragma once

struct ENEMY;

// ゲームマネージャー
struct GAME_MANAGER {
	int wave;			// 現在のウェーブ
	int spawnNum;		// 一度にスポーンする敵の数
	int waveBorder;		// 次のウェーブまでに必要なキル数
	float highRate;		// High Enemyがスポーンする確率
	float explRate;		// Explosion Enemyがスポーンする確率

	int energyLevel;	// 電力レベル
	int energyGauge;	// 電力ゲージ
	int maxGauge;		// 電力レベルアップに必要なゲージ量

	int killCnt;		// 敵の討伐数
	float timeLimit;	// 制限時間(秒)

	int comboCnt;		// コンボ数
	int maxCombo;		// 最大コンボ数
	float comboTimer;	// コンボを増やすか戻すかを決める時間

	unsigned int score;		// スコア(最小値 0)
	unsigned int highScore;	// ハイスコア(最小値 0)
	double scoreRate;		// コンボ数によるスコア倍率
};

void GameManager_Initialize();
void GameManager_Finalize();
void GameManager_Update();

const GAME_MANAGER* GetGameManager();

void AddEnergy();
void AddEnergy(int num);
void AddKillCount();
void AddKillCount(int cnt);
void SubTimeLimit(float num);
void AddComboCount();
void AddComboCount(int cnt);
void AddScore(ENEMY* enemy);
