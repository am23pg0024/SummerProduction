#pragma once
/**
 *	@file
 *	@brief	入力関連
 */

#include "UnlimitedLib.h"
#include <xinput.h>

#define PAD_MAX	4


// 入力システムの初期化
void InitializeInput(void);
// 入力システムの解放
void FinalizeInput(void);

// 入力状況の更新
void UpdateInput(void);

//=======================================================
// キーが押されているか？
bool IsKeyPress(int key);
// キーが押されたか？
bool IsKeyPush(int key);
// キーが離されたか？
bool IsKeyRelease(int key);

//=======================================================
// (DEBUG用)パッドの userIndex 取得
int DebugGetPadUserIndex(int index);

// パッドがつながっているか
bool IsPadConnect(int index);
// パッドが押されているか？
bool IsPadPress(int index, int pad);
// パッドが押されたか？
bool IsPadPush(int index, int pad);
// パッドが離されたか？
bool IsPadRelease(int index, int pad);

// 左スティックのXを取得
float GetPadLeftStickX(int index);
// 左スティックのYを取得
float GetPadLeftStickY(int index);
// 右スティックのXを取得
float GetPadRightStickX(int index);
// 右スティックのYを取得
float GetPadRightStickY(int index);
// 左トリガー値を取得
float GetPadLeftTrigger(int index);
// 右トリガー値を取得
float GetPadRightTrigger(int index);

// パッドがつながっているか
bool IsPadConnectDirect(int userIndex);
// パッドが押されているか？
bool IsPadPressDirect(int userIndex, int pad);
// パッドが押されたか？
bool IsPadPushDirect(int userIndex, int pad);
// パッドが離されたか？
bool IsPadReleaseDirect(int userIndex, int pad);

// 左スティックのXを取得
float GetPadLeftStickXDirect(int userIndex);
// 左スティックのYを取得
float GetPadLeftStickYDirect(int userIndex);
// 右スティックのXを取得
float GetPadRightStickXDirect(int userIndex);
// 右スティックのYを取得
float GetPadRightStickYDirect(int userIndex);
// 左トリガーを取得
float GetPadLeftTriggerDirect(int userIndex);
// 右トリガーを取得
float GetPadRightTriggerDirect(int userIndex);

//=======================================================
// マウスカーソルの位置取得
bool GetMousePosition(int* px, int* py);

// マウスホイール取得
int GetMouseWheelNotch(void);

// マウスボタン取得
short GetMouseButton(int nVirtKey);
