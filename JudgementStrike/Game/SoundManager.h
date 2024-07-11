#pragma once

#define SOUNDMAX 13

enum SOUNDSCENE {
	/*BGM*/
	Title,				// タイトル
	Game,				// インゲーム
	Result,				// リザルト
	/*SE*/
	GameOver,			// ゲームオーバー
	SwordAttack,		// 剣攻撃
	GunAttack,			// 銃(通常)攻撃
	GunChargeAttack,	// 銃(チャージ)攻撃
	PlayerDamage,		// シールドなしダメージ
	ShieldDamage,		// シールドありダメージ
	ShieldBreak,		// シールド破壊時
	MoveCursor,			// カーソル移動
	ButtonDecision,		// ボタン決定
	ButtonCancel,		// ボタンキャンセル
};

enum SOUNDTYPE {
	BGM,
	SE,
};

struct VOLUME {
	int BGM;		// BGMの音量
	int SE;		// SEの音量
	int nowBGM;		// 表示用のBGM音量
	int nowSE;		// 表示用のSE音量
};

struct SOUNDMANAGER {
	int handle;		// サウンド保存
	bool loop;		// ループ再生するか
	SOUNDTYPE type;	// BGMかSEか
};

const VOLUME* GetVolume();

void Sounds_Initialize();
void Sounds_Finalize();
void Sounds_Update();
void PlaySounds(SOUNDSCENE scene);
void StopSounds(SOUNDSCENE scene);
void SetSoundVolume(SOUNDTYPE type, int vol);