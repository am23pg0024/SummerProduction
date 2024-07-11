#include "UnlimitedLib/UnlimitedLib.h"
#include "SoundManager.h"
#include "Game.h"
#include <thread>

SOUNDMANAGER sound[SOUNDMAX];
VOLUME volume;

const VOLUME* GetVolume() { return &volume; }

// サウンドの読み込み用スレッド管理
std::thread soundLoadThread;

void Sounds_Initialize()
{
	// タイトルで流すBGM
	sound[Title].handle = CreateSEOgg("Sounds/BGM/Subject_System.ogg");		
	sound[Title].loop = true;
	sound[Title].type = BGM;
	// ボタン決定時に流すSE
	sound[ButtonDecision].handle = CreateSEOgg("Sounds/SE/ボタン決定時.ogg");
	sound[ButtonDecision].loop = false;
	sound[ButtonDecision].type = SE;
	// ボタンキャンセル時に流すSE
	sound[ButtonCancel].handle = CreateSEOgg("Sounds/SE/ボタンキャンセル.ogg");
	sound[ButtonCancel].loop = false;
	sound[ButtonCancel].type = SE;
	// カーソル移動時に流すSE
	sound[MoveCursor].handle = CreateSEOgg("Sounds/SE/カーソル移動.ogg");
	sound[MoveCursor].loop = false;
	sound[MoveCursor].type = SE;

	std::thread thread([&]()
	{
		// ゲームプレイ時に流すBGM
		sound[Game].handle = CreateSEOgg("Sounds/BGM/Battle_Till_Dawn.ogg");
		sound[Game].loop = true;
		sound[Game].type = BGM;
		// リザルトで流すBGM
		sound[Result].handle = CreateSEOgg("Sounds/BGM/Sunset_Our_Memories.ogg");
		sound[Result].loop = true;
		sound[Result].loop = true;
		// ゲームオーバー時に流すSE
		sound[GameOver].handle = CreateSEOgg("Sounds/SE/dead-sound.ogg");
		sound[GameOver].loop = false;
		sound[GameOver].type = SE;
		// 剣攻撃時に流すSE
		sound[SwordAttack].loop = false;
		sound[SwordAttack].handle = CreateSEOgg("Sounds/SE/剣攻撃時の音声.ogg");	
		sound[SwordAttack].type = SE;
		// 銃の通常攻撃時に流すSE
		sound[GunAttack].handle = CreateSEOgg("Sounds/SE/銃攻撃時音声.ogg");
		sound[GunAttack].loop = false;
		sound[GunAttack].type = SE;
		// 銃のチャージ攻撃時に流すSE
		sound[GunChargeAttack].handle = CreateSEOgg("Sounds/SE/チャージ攻撃時音声_銃_.ogg");
		sound[GunChargeAttack].loop = false;
		sound[GunChargeAttack].type = SE;
		// 被ダメージ(シールドなし)時に流すSE
		sound[PlayerDamage].handle = CreateSEOgg("Sounds/SE/被ダメ音声_シールドなし_.ogg");
		sound[PlayerDamage].loop = false;
		sound[PlayerDamage].type = SE;
		// 被ダメージ(シールドあり)時に流すSE
		sound[ShieldDamage].handle = CreateSEOgg("Sounds/SE/被ダメ音声_シールドあり_.ogg");
		sound[ShieldDamage].loop = false;
		sound[ShieldDamage].type = SE;
		// シールド破壊時に流すSE
		sound[ShieldBreak].handle = CreateSEOgg("Sounds/SE/被ダメ音声_シールド消滅時音声_.ogg");
		sound[ShieldBreak].loop = false;
		sound[ShieldBreak].type = SE;
		});
	thread.swap(soundLoadThread);

	// サウンドの読み込みが完了していなければ読み込みを待つ
	// soundLoadThread.join();
	
	// 音量設定
	volume.BGM = 20;
	volume.SE = 40;
	volume.nowBGM = volume.BGM * 100;
	volume.nowSE = volume.SE * 100;
}

void Sounds_Finalize()
{
	if (soundLoadThread.joinable())
		soundLoadThread.join();
	for (int i = 0; i < SOUNDMAX; i++)
	{
		ReleaseSE(sound[i].handle);
	}
}

void Sounds_Update()
{	
	for (int i = 0; i < SOUNDMAX; i++)
	{
		if (sound[i].type == BGM)
			SetVolumeSE(sound[i].handle, volume.BGM * 0.01);
		if (sound[i].type == SE)
			SetVolumeSE(sound[i].handle, volume.SE * 0.01);
	}
}

void PlaySounds(SOUNDSCENE scene)
{
	if(sound[scene].type == BGM)PlaySE(sound[scene].handle, sound[scene].loop, volume.BGM * 0.01);
	if(sound[scene].type == SE)	PlaySE(sound[scene].handle, sound[scene].loop, volume.SE * 0.01);
}

void StopSounds(SOUNDSCENE scene)
{
	StopSE(sound[scene].handle);
}

void SetSoundVolume(SOUNDTYPE type, int vol)	/// (BGMかSEか、音量10 or -10)
{	
	switch (type)
	{
	case BGM:
		volume.BGM += vol;
		if (volume.BGM > 100)
			volume.BGM = 100;
		if (volume.BGM < 0)
			volume.BGM = 0;
		break;
	case SE:
		volume.SE += vol;
		if (volume.SE > 100)
			volume.SE = 100;
		if (volume.SE < 0)
			volume.SE = 0;
		break;
	}
}