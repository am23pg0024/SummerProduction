#pragma once
/**
 *	@file
 *	@brief	Windows関連
 */

#include "UnlimitedLib.h"

// シンプルなウィンドウの作成
bool CreateGameWindow(const char* pName, int x, int y, int width, int height, bool isFullscreen);
// メッセージの更新
bool UpdateWindowMessage(void);
// ウィンドウに終了通知を送る
void QuitWindow(void);
// 終了通知が来ているか？
bool IsQuitMessage(void);
// ウィンドウハンドルの取得
HWND GetWindowHandle(void);

// ウィンドウの横幅取得
int GetWindowWidth(void);
// ウィンドウの縦幅取得
int GetWindowHeight(void);

// マウスホイール情報取得
int GetMouseWheelNotchBefore(void);
// マウスホイール情報リセット
void UpdateMouseWheelNotch(void);
