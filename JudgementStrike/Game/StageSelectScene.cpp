#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "OptionUI.h"
#include "Game.h"

static bool SceneInit()
{

	return true;
}

static void SceneFinal()
{

}

static void SceneUpdate()
{
	// Enter�L�[�ŃQ�[���V�[���֑J��
	if (IsKeyPush(VK_RETURN))
		JumpScene(SetupGameScene);
}

static void SceneRender()
{

}

// �Q�[���V�[���̃Z�b�g�A�b�v
SceneProc SetupStageSelectScene()
{
	SceneProc proc{
		"StageSelect",
		SceneInit,
		SceneFinal,
		SceneUpdate,
		SceneRender
	};
	return proc;
}
