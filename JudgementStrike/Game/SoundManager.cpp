#include "UnlimitedLib/UnlimitedLib.h"
#include "SoundManager.h"
#include "Game.h"
#include <thread>

SOUNDMANAGER sound[SOUNDMAX];
VOLUME volume;

const VOLUME* GetVolume() { return &volume; }

// �T�E���h�̓ǂݍ��ݗp�X���b�h�Ǘ�
std::thread soundLoadThread;

void Sounds_Initialize()
{
	// �^�C�g���ŗ���BGM
	sound[Title].handle = CreateSEOgg("Sounds/BGM/Subject_System.ogg");		
	sound[Title].loop = true;
	sound[Title].type = BGM;
	// �{�^�����莞�ɗ���SE
	sound[ButtonDecision].handle = CreateSEOgg("Sounds/SE/�{�^�����莞.ogg");
	sound[ButtonDecision].loop = false;
	sound[ButtonDecision].type = SE;
	// �{�^���L�����Z�����ɗ���SE
	sound[ButtonCancel].handle = CreateSEOgg("Sounds/SE/�{�^���L�����Z��.ogg");
	sound[ButtonCancel].loop = false;
	sound[ButtonCancel].type = SE;
	// �J�[�\���ړ����ɗ���SE
	sound[MoveCursor].handle = CreateSEOgg("Sounds/SE/�J�[�\���ړ�.ogg");
	sound[MoveCursor].loop = false;
	sound[MoveCursor].type = SE;

	std::thread thread([&]()
	{
		// �Q�[���v���C���ɗ���BGM
		sound[Game].handle = CreateSEOgg("Sounds/BGM/Battle_Till_Dawn.ogg");
		sound[Game].loop = true;
		sound[Game].type = BGM;
		// ���U���g�ŗ���BGM
		sound[Result].handle = CreateSEOgg("Sounds/BGM/Sunset_Our_Memories.ogg");
		sound[Result].loop = true;
		sound[Result].loop = true;
		// �Q�[���I�[�o�[���ɗ���SE
		sound[GameOver].handle = CreateSEOgg("Sounds/SE/dead-sound.ogg");
		sound[GameOver].loop = false;
		sound[GameOver].type = SE;
		// ���U�����ɗ���SE
		sound[SwordAttack].loop = false;
		sound[SwordAttack].handle = CreateSEOgg("Sounds/SE/���U�����̉���.ogg");	
		sound[SwordAttack].type = SE;
		// �e�̒ʏ�U�����ɗ���SE
		sound[GunAttack].handle = CreateSEOgg("Sounds/SE/�e�U��������.ogg");
		sound[GunAttack].loop = false;
		sound[GunAttack].type = SE;
		// �e�̃`���[�W�U�����ɗ���SE
		sound[GunChargeAttack].handle = CreateSEOgg("Sounds/SE/�`���[�W�U��������_�e_.ogg");
		sound[GunChargeAttack].loop = false;
		sound[GunChargeAttack].type = SE;
		// ��_���[�W(�V�[���h�Ȃ�)���ɗ���SE
		sound[PlayerDamage].handle = CreateSEOgg("Sounds/SE/��_������_�V�[���h�Ȃ�_.ogg");
		sound[PlayerDamage].loop = false;
		sound[PlayerDamage].type = SE;
		// ��_���[�W(�V�[���h����)���ɗ���SE
		sound[ShieldDamage].handle = CreateSEOgg("Sounds/SE/��_������_�V�[���h����_.ogg");
		sound[ShieldDamage].loop = false;
		sound[ShieldDamage].type = SE;
		// �V�[���h�j�󎞂ɗ���SE
		sound[ShieldBreak].handle = CreateSEOgg("Sounds/SE/��_������_�V�[���h���Ŏ�����_.ogg");
		sound[ShieldBreak].loop = false;
		sound[ShieldBreak].type = SE;
		});
	thread.swap(soundLoadThread);

	// �T�E���h�̓ǂݍ��݂��������Ă��Ȃ���Γǂݍ��݂�҂�
	// soundLoadThread.join();
	
	// ���ʐݒ�
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

void SetSoundVolume(SOUNDTYPE type, int vol)	/// (BGM��SE���A����10 or -10)
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