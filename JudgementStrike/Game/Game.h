#pragma once

#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

#define DELTA_TIME (1.f / 60)

// �Q�[���̏��
enum GAME_STATE {
	GPlay,		// �Q�[���v���C
	GClear,		// �Q�[���N���A
	GGameover,	// �Q�[���I�[�o�[
	GPause,		// �|�[�Y
	GOption,	// �I�v�V�������
};

// �I�u�W�F�N�g�������
enum STATE {
	SIdle,			// �ҋ@
	SMove,			// �ړ�
	SAttack,		// �U��
	SCharging,		// �`���[�W��
	SChargeAttack,	// �`���[�W�U��
	SAvoid,			// ���
	SDamaged,		// ��_���[�W
	SDead,			// ���S
};

struct VECTOR {
	float x, y;
};

struct FRECT {
    float left, top, right, bottom;
};

struct CIRCLE {
	float x, y;		// ���S���W
	float radius;	// ���a
};

struct FAN {
	float x, y; // ��̒��S
	float radius;    // ��̔��a
	float angle;     // ��͈̔�(�p�x)
};

struct OBJECT {
	int sprite;			// �X�v���C�g
	int width, height;	// �X�v���C�g�T�C�Y

	float x, y;		// ���W
	float vx, vy;	// ���x
	float speed;	// ����
	float angle;	// �p�x
	bool isActive;	// �����t���O
	bool isRight;	// �E�������Ă��邩�ǂ���
	STATE state;	// ���
	FRECT hitRect;	// �����蔻���`

	int hp;		// �̗�
};

// HSV�p�����[�^
struct HSV_PARAM {
	float hue;			// �F��
	float saturation;	// �ʓx
	float value;		// �P�x
	float dummy3;
};

void Initialize();
void Finalize();
void GameMain();

void RenderBG();

const GAME_STATE GetGameState();
void SetGameState(GAME_STATE state);
OBJECT* GetSelectCursor();
void SetSelectCursorSprite(int sprite);
VECTOR* GetOffset();
int GetChangeHSVShader();
HSV_PARAM* GetHSVParam();
void SetHSVParam(float hue, float saturation, float value);

float GetDistance(const OBJECT* a, const OBJECT* b);
void Swap(float& a, float& b);

// �ΏۃI�u�W�F�N�g�Ƀ_���[�W��^����
void Damage(OBJECT* obj, int damage);
