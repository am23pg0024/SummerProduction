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
	// Enterキーでゲームシーンへ遷移
	if (IsKeyPush(VK_RETURN))
		JumpScene(SetupGameScene);
}

static void SceneRender()
{

}

// ゲームシーンのセットアップ
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
