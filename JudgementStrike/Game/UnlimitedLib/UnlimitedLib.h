#pragma once
/**
 *	@file
 *	@brief	アンリミテッドライブラリ関連
 */

// Win32API Header
#include <Windows.h>
#include <assert.h>
#include <stdio.h>

// UnlimitedLib Header
#include "Debug.h"
#include "Window.h"
#include "DirectX.h"
#include "Texture.h"
#include "2DHelper.h"
#include "Input.h"
#include "Helper/Font.h"
#include "Sound.h"
#include "Logger.h"

// OtherLib Header
#include "../libs/FreeImage/x32/FreeImage.h"

// UnlimitedLibの初期化
bool InitializeUnlimitedLib(const char* pName, int width, int height, bool isFullscreen);
// UnlimitedLibの解放
void FinalizeUnlimitedLib(void);
