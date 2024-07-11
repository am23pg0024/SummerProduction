#include "UnlimitedLib/UnlimitedLib.h"
#include "SceneManager.h"

static SceneProc currentScene;	// ���݂̃V�[�����

void InitSceneManager()
{
	ZeroMemory(&currentScene, sizeof(currentScene));
}

void FinalSceneManager()
{
	if (currentScene.final) {
		currentScene.final();
	}
}

void UpdateSceneManager()
{
	if (currentScene.update) {
		currentScene.update();
	}
}

void RenderSceneManager()
{
	if (currentScene.render) {
		currentScene.render();
	}
}

bool JumpScene(setupFunc setup)
{
	FinalSceneManager();
	InitSceneManager();

	bool ret = true;

	if (setup) {
		// �V�[���ݒ���Z�b�g�A�b�v
		currentScene = setup();
		
		// ������
		if (currentScene.init) {
			ret = currentScene.init();
		}
	}
	return ret;
}

const char* GetSceneName()
{
	return currentScene.name;
}
