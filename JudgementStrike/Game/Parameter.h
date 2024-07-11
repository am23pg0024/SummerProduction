#pragma once

// 敵関連のパラメータを管理する構造体
struct ENEMY_PARAM {
	int hp;
	float speed;
	int power;
	int hitPower;
};

// 電力システム関連のパラメータを管理する構造体
struct ENERGY_PARAM {
	int maxGauge;	// 電力レベルアップに必要なゲージ量

	int swordPower;		// 次の電力レベルの剣の攻撃力
	float swordLength;	// 次の電力レベルの剣の長さ
	float swordAngle;	// 次の電力レベルの剣の範囲
	int gunPower;		// 次の電力レベルの銃の攻撃力
	float gunScale;		// 次の電力レベルのチャージ弾のスケール
	int cGunPower;		// 次の電力レベルのチャージ弾の攻撃力
};

// ウェーブ関連のパラメータを管理する構造体
struct WAVE_PARAM {
	int spawnNum;		// 一度にスポーンする敵の数
	int border;			// 次のウェーブまでに必要なキル数
	float highRate;		// Explosion Enemyがスポーンする確率
	float explRate;		// Explosion Enemyがスポーンする確率
};

struct PARAM {
	float timeLimit;	// 制限時間

	// プレイヤー
	struct {
		float speed;
		float speed2;
		float speed3;
		float CHARGE_CHECK_TIME;
		float CHARGING_TIME;
		float MAX_INVINCIBLE_TIME;
		float MAX_COOL_TIME;
		float AVOID_DIST;
	} player;

	// 弾
	struct {
		float speed;
	} bullet;

	// シールド回復
	struct {
		float startTime;	// 回復が始まるまでの時間
		float interval;		// 回復するインターバル
		int num;			// 一度に回復するシールド量
	} shieldRecovery;

	// 電力システム
	ENERGY_PARAM energyParams[5];

	// 敵
	struct {
		ENEMY_PARAM params[3];
		int maxSpawn;
		float maxBlinkTime;
	} enemy;

	// 爆発
	struct {
		int power;
		float scale;
	} explosion;

	// ウェーブ
	struct {
		WAVE_PARAM params[4];
		float spawnTime;
	} wave;
	
};

const PARAM* GetParameter();

void SaveParam();
void LoadParam();
