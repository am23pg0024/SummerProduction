#pragma once

// 初期化用関数
using initFunc = bool(*)();
// 解放用関数
using finalFunc = void(*)();
// 更新用関数
using updateFunc = void(*)();
// 描画用関数
using renderFunc = void(*)();

// シーン処理設定用構造体
struct SceneProc {
	const char* name;	// シーン名
	initFunc init;
	finalFunc final;
	updateFunc update;
	renderFunc render;
};

// シーン遷移用の関数
using setupFunc = SceneProc(*)();


void InitSceneManager();
void FinalSceneManager();
void UpdateSceneManager();
void RenderSceneManager();

// シーンの遷移
bool JumpScene(setupFunc setup);
// シーン名の取得
const char* GetSceneName();
