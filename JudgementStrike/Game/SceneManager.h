#pragma once

// �������p�֐�
using initFunc = bool(*)();
// ����p�֐�
using finalFunc = void(*)();
// �X�V�p�֐�
using updateFunc = void(*)();
// �`��p�֐�
using renderFunc = void(*)();

// �V�[�������ݒ�p�\����
struct SceneProc {
	const char* name;	// �V�[����
	initFunc init;
	finalFunc final;
	updateFunc update;
	renderFunc render;
};

// �V�[���J�ڗp�̊֐�
using setupFunc = SceneProc(*)();


void InitSceneManager();
void FinalSceneManager();
void UpdateSceneManager();
void RenderSceneManager();

// �V�[���̑J��
bool JumpScene(setupFunc setup);
// �V�[�����̎擾
const char* GetSceneName();
