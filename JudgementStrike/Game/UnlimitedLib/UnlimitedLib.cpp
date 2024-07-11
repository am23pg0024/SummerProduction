/**
 *	@file
 *	@brief	アンリミテッドライブラリ関連
 */
#include "UnlimitedLib.h"

#pragma comment(lib, "libs/FreeImage/x32/FreeImage.lib")

// UnlimitedLibの初期化
bool InitializeUnlimitedLib(const char* pName, int width, int height, bool isFullscreen)
{
#ifndef _DEBUG
	// ウィンドウの生成
	if (!CreateGameWindow(pName, 0, 0, 1920, 1080, isFullscreen))
	{
		return false;
	}
#else 
	// ウィンドウの生成
	if (!CreateGameWindow(pName, 0, 0, width, height, isFullscreen))
	{
		return false;
	}
#endif
	// ロガー系初期化
	if(!InitializeLogger()) return false;
	// DirectXの初期化
	if (!InitializeDirectX(GetWindowHandle(), width, height, isFullscreen))
	{
		return false;
	}
	// サウンド初期化
	if (!InitializeSound())
	{
		return false;
	}

	// 入力システムの初期化
	InitializeInput();

	return true;
}


// UnlimitedLibの解放
void FinalizeUnlimitedLib(void)
{
	// 入力システムの解放
	FinalizeInput();

	// サウンドの終了処理
	FinalizeSound();

	// DirectXの終了処理
	FinalizeDirectX();
}

