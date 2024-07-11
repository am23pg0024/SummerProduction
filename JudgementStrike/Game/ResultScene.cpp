#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Game.h"
#include "SoundManager.h"
#include "GameManager.h"

// ���U���g�\���Ǘ��p�\����
struct RESULT_ANIM {
	int order;			// �\�����鏇��
	float x, y;			// �\�����W
	bool isComplete;	// �\�������t���O
};

// �X�v���C�g
int resultBG, resultLogo;
int killedText, maxComboText, scoreText, highScoreText, rankText, rankFrame, backText;
int rank;
int resultNum[10];

RESULT_ANIM resultAnims[6];
int currentOrder;	// ���U���g�\���Ǘ��p�̕ϐ�
static int alpha;
static float fadeRate;
static float fadeSpeed;


void DrawNumber(int num, float x, float y, float interval, bool isAnimComplete);

static bool SceneInit()
{
	// �X�v���C�g�ǂݍ���
	resultBG = CreateSprite("Assets/Sprites/UI/Result/Result_background.png");
	resultLogo = CreateSprite("Assets/Sprites/UI/Result/Result_logo.png");
	killedText = CreateSprite("Assets/Sprites/UI/Result/Result_killed.png");
	maxComboText = CreateSprite("Assets/Sprites/UI/Result/Result_maxcombo.png");
	scoreText = CreateSprite("Assets/Sprites/UI/Result/Result_score.png");
	highScoreText = CreateSprite("Assets/Sprites/UI/Result/Result_highscore.png");
	rankText = CreateSprite("Assets/Sprites/UI/Result/Result_rank.png");
	rankFrame = CreateSprite("Assets/Sprites/UI/Result/Result_frame.png");
	backText = CreateSprite("Assets/Sprites/UI/Result/Result_back.png");
	CreateSpriteMatrix("Assets/Sprites/UI/Result/Result_number.png", 24, 43, 10, 1, 10, resultNum);

	// �X�R�A�ɉ����������N�X�v���C�g�ǂݍ���
	int score = GetGameManager()->score;
	if (score >= 1200000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_SS.png");
		SetSpriteOrigin(rank, 152, 96);
	}
	else if (score >= 800000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_S.png");
		SetSpriteOrigin(rank, 74, 94);
	}
	else if (score >= 650000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_A.png");
		SetSpriteOrigin(rank, 74, 96);
	}
	else if (score >= 400000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_B.png");
		SetSpriteOrigin(rank, 74, 94);
	}
	else if (score >= 200000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_C.png");
		SetSpriteOrigin(rank, 74, 94);
	}
	else if (score >= 100000) {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_D.png");
		SetSpriteOrigin(rank, 74, 94);
	}
	else {
		rank = CreateSprite("Assets/Sprites/UI/Result/Result_rank_E.png");
		SetSpriteOrigin(rank, 69, 94);
	}
	SetSpritePos(rank, 900, 382);

	// ��_�ݒ�
	SetSpriteOrigin(resultBG, 584, 246);
	SetSpriteOrigin(resultLogo, 474, 46);
	SetSpriteOrigin(killedText, 120, 21);		// �E�[
	SetSpriteOrigin(maxComboText, 236, 21);		// �E�[
	SetSpriteOrigin(scoreText, 128, 21);		// �E�[
	SetSpriteOrigin(highScoreText, 228, 24);	// �E�[
	SetSpriteOrigin(rankText, 49, 22);
	SetSpriteOrigin(rankFrame, 185, 148);
	SetSpriteOrigin(backText, 164, 24);
	for (int i = 0; i < 10; i++)
		SetSpriteOrigin(resultNum[i], 12, 22);

	// �ʒu�ݒ�
	float y1 = 40.f;
	float y2 = (318 - 2 * y1) / 3.f;
	SetSpritePos(resultBG, SCREEN_WIDTH / 2, 382);
	SetSpritePos(resultLogo, SCREEN_WIDTH / 2, 68);
	SetSpritePos(killedText, 420, 136 + y1 + 21);
	SetSpritePos(maxComboText, 420, 136 + y1 + 63 + y2);
	SetSpritePos(scoreText, 420, 136 + y1 + 105 + y2 * 2);
	SetSpritePos(highScoreText, 420, 136 + y1 + 150 + y2 * 3);
	SetSpritePos(rankText, 700, 136 + y1 + 21);
	SetSpritePos(rankFrame, 900, 382);
	SetSpritePos(backText, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);

	// ���U���g���ڏ���
	float x = 165.f;
	// Killed
	const Sprite* sp = GetSprite(killedText);
	resultAnims[0] = { 0, sp->x + x, sp->y, false };
	// MaxCombo
	sp = GetSprite(maxComboText);
	resultAnims[1] = { 1, sp->x + x, sp->y, false };
	// Score
	sp = GetSprite(scoreText);
	resultAnims[2] = { 2, sp->x + x, sp->y, false };
	// HighScore
	sp = GetSprite(highScoreText);
	resultAnims[3] = { 3, sp->x + x, sp->y, false };
	// Rank
	resultAnims[4] = { 4, 0, 0, false };	// order�ȊO�g��Ȃ�
	// BackText
	resultAnims[5] = { 5, 0, 0, false };	// order�ȊO�g��Ȃ�

	// �ϐ�������
	currentOrder = 0;
	alpha = 0;
	fadeRate = 0.f;
	fadeSpeed = 0.5f;

	PlaySounds(Result);

	return true;
}

static void SceneFinal()
{
	ReleaseSprite(resultBG);
	ReleaseSprite(resultLogo);
	ReleaseSprite(killedText);
	ReleaseSprite(maxComboText);
	ReleaseSprite(scoreText);
	ReleaseSprite(highScoreText);
	ReleaseSprite(rankText);
	ReleaseSprite(rankFrame);
	ReleaseSprite(backText);
	ReleaseSprite(rank);
	ReleaseSpriteMatrix(resultNum, 10);
}

static void SceneUpdate()
{
	// Enter�ŃQ�[���J�n�O��ʂ�
	if (resultAnims[5].order <= currentOrder) {
		if (IsKeyRelease(VK_RETURN) || IsPadReleaseDirect(0, XINPUT_GAMEPAD_B)) {
			StopSounds(Result);
			JumpScene(SetupTitleScene);
		}

		// BackText����
		fadeRate += fadeSpeed * DELTA_TIME;
		alpha = fadeRate * 255;
		if (alpha <= 10) {
			alpha = 10;
			fadeSpeed = 0.8f;
		}
		else if (alpha >= 255) {
			alpha = 255;
			fadeSpeed = -0.6f;
		}
	}
	else {
		bool& isComplete = resultAnims[currentOrder].isComplete;
		if (!isComplete) {
			if (IsKeyRelease(VK_RETURN) || IsPadReleaseDirect(0, XINPUT_GAMEPAD_B)) {
				alpha = 255;
				isComplete = true;
			}
			fadeRate += fadeSpeed * DELTA_TIME;
			alpha = fadeRate * 255;
			if (alpha >= 255) {
				alpha = 255;
				isComplete = true;
			}
		}
		else {
			if (currentOrder < 5)
				currentOrder++;
			alpha = 0;
			fadeRate = 0.f;
		}
	}
}

static void SceneRender()
{
	RenderBG();
	RenderSprite(resultBG);
	RenderSprite(resultLogo);
	RenderSprite(killedText);
	RenderSprite(maxComboText);
	RenderSprite(scoreText);
	RenderSprite(highScoreText);
	RenderSprite(rankText);
	RenderSprite(rankFrame);

	const GAME_MANAGER* gm = GetGameManager();
	float interval = 21.f;

	// Killed
	if (resultAnims[0].order <= currentOrder)
		DrawNumber(gm->killCnt, resultAnims[0].x, resultAnims[0].y, interval, resultAnims[0].isComplete);
	// MaxCombo
	if (resultAnims[1].order <= currentOrder)
		DrawNumber(gm->maxCombo, resultAnims[1].x, resultAnims[1].y, interval, resultAnims[1].isComplete);
	// Score
	if (resultAnims[2].order <= currentOrder)
		DrawNumber(gm->score, resultAnims[2].x, resultAnims[2].y, interval, resultAnims[2].isComplete);
	// HighScore
	if (resultAnims[3].order <= currentOrder)
		DrawNumber(gm->highScore, resultAnims[3].x, resultAnims[3].y, interval, resultAnims[3].isComplete);
	// Rank
	if (resultAnims[4].order <= currentOrder) {
		if (!resultAnims[4].isComplete)
			SetSpriteColor(rank, D3DCOLOR_RGBA(255, 255, 255, alpha));
		else
			SetSpriteColor(rank, D3DCOLOR_RGBA(255, 255, 255, 255));
		RenderSprite(rank);
	}
	// BackText
	if (resultAnims[5].order <= currentOrder) {
		SetSpriteColor(backText, D3DCOLOR_RGBA(255, 255, 255, alpha));
		RenderSprite(backText);
	}
}

// �Q�[���V�[���̃Z�b�g�A�b�v
SceneProc SetupResultScene()
{
	SceneProc proc{
		"Result",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}

void DrawNumber(int num, float x, float y, float interval, bool isAnimComplete)
{
	const Sprite* sp = GetSprite(resultNum[0]);
	if (num == 0) {
		if (!isAnimComplete)
			SetSpriteColor(resultNum[0], D3DCOLOR_RGBA(255, 255, 255, alpha));
		else
			SetSpriteColor(resultNum[0], D3DCOLOR_RGBA(255, 255, 255, 255));
		RenderSpriteRot(resultNum[0], sp->cx, sp->cy, x, y, 0.f, 1.f);
	}
	while (num > 0) {
		if (!isAnimComplete)
			SetSpriteColor(resultNum[num % 10], D3DCOLOR_RGBA(255, 255, 255, alpha));
		else
			SetSpriteColor(resultNum[num % 10], D3DCOLOR_RGBA(255, 255, 255, 255));
		RenderSpriteRot(resultNum[num % 10], sp->cx, sp->cy, x, y, 0.f, 1.f);
		x -= interval;
		num /= 10;
	}
}
