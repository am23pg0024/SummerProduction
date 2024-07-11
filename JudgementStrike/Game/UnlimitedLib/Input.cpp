/**
 *	@file
 *	@brief	入力関連
 */
#include "UnlimitedLib.h"

#define KEY_MAX	256

struct KeyInfo
{
	bool keyState[KEY_MAX];
	bool keyStateOld[KEY_MAX];
};
static KeyInfo	s_keyState;

struct MouseInfo
{
    int mouseWheelNotch;
};
static MouseInfo s_mouseState;

struct PadInfo
{
	bool bConnected;
	bool bConnectedOld;
	XINPUT_STATE xinputState;
	XINPUT_STATE xinputStateOld;
};
static PadInfo	s_padState[PAD_MAX];
static const int INVALID_PAD_USER_INDEX = -1;	// userIndex の無効値
static int s_padIndexList[PAD_MAX];	// pad の userIndex リスト

// 入力システムの初期化
void InitializeInput(void)
{
	for (int i = 0; i < KEY_MAX; i++)
	{
		s_keyState.keyState[i] = false;
		s_keyState.keyStateOld[i] = false;
	}
    s_mouseState.mouseWheelNotch = 0;
	for(int i = 0; i < PAD_MAX; i++)
	{
		s_padState[i].bConnected = false;
		s_padState[i].bConnectedOld = false;
		s_padState[i].xinputState = {};
		s_padState[i].xinputStateOld = {};

		s_padIndexList[i] = INVALID_PAD_USER_INDEX;
	}
}

// 入力システムの解放
void FinalizeInput(void)
{
	InitializeInput();
}

// キーボードの更新
void UpdateKeyboard(void)
{
	BYTE keyboardState[256]{};
	// キーボード状態の取得
	if (!GetKeyboardState(keyboardState))
	{
		return;
	}

	for (int i = 0; i < KEY_MAX; i++)
	{
		// 入力されている場合はキーのビットを立てる
		bool isInput = (keyboardState[i] & 0x80) != 0;

		// ステートの更新
		s_keyState.keyStateOld[i] = s_keyState.keyState[i];
		s_keyState.keyState[i] = isInput;
	}
}

#if 0
// test
void TestXInput(void)
{
	int key[PAD_MAX] = { 'A', 'S', 'D', 'F' };
	for(int i = 0; i < PAD_MAX; ++i){
		s_padState[i].bConnected = IsKeyPress(key[i]);
	}
}
#endif

// マウス更新
void UpdateMouse(void)
{
    s_mouseState.mouseWheelNotch = GetMouseWheelNotchBefore();
    UpdateMouseWheelNotch();
}

// userIndex 更新
void UpdatePadUserIndex(void)
{
	// 切断
	for (int userIndex = 0; userIndex < PAD_MAX; userIndex++)
	{
		if (!s_padState[userIndex].bConnected && s_padState[userIndex].bConnectedOld) {
			for (int index = 0; index < PAD_MAX; index++) {
				if (s_padIndexList[index] == userIndex) {
					s_padIndexList[index] = INVALID_PAD_USER_INDEX;
					break;
				}
			}
		}
	}

	// 接続
	for (int userIndex = 0; userIndex < PAD_MAX; userIndex++)
	{
		if (s_padState[userIndex].bConnected && !s_padState[userIndex].bConnectedOld) {
			for (int index = 0; index < PAD_MAX; index++) {
				if (s_padIndexList[index] == INVALID_PAD_USER_INDEX) {
					s_padIndexList[index] = userIndex;
					break;
				}
			}
		}
	}
}

// XInputの更新
void UpdateXInput(void)
{
	for(int userIndex = 0; userIndex < PAD_MAX; userIndex++)
	{
		// ステートの更新
		s_padState[userIndex].xinputStateOld = s_padState[userIndex].xinputState;
		s_padState[userIndex].bConnectedOld = s_padState[userIndex].bConnected;

		XINPUT_STATE state = {0};
		// XInput 状態取得
		DWORD ret = XInputGetState(userIndex, &state);
		if(ret == ERROR_SUCCESS){
			// デッドゾーン設定
			short deadLeft = 12000; //XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			short deadRight = 12000; //XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			if(abs(state.Gamepad.sThumbLX) < deadLeft && abs(state.Gamepad.sThumbLY) < deadLeft){
				state.Gamepad.sThumbLX = 0;
				state.Gamepad.sThumbLY = 0;
			}
			if(abs(state.Gamepad.sThumbRX) < deadRight && abs(state.Gamepad.sThumbRY) < deadRight){
				state.Gamepad.sThumbRX = 0;
				state.Gamepad.sThumbRY = 0;
			}

			s_padState[userIndex].bConnected = true;
			s_padState[userIndex].xinputState = state;
		}
		else{
			s_padState[userIndex].bConnected = false;
			s_padState[userIndex].xinputState = state;
		}
	}

	// test
	//TestXInput();

	// userIndex 更新
	UpdatePadUserIndex();
}

// 入力状況の更新
void UpdateInput(void)
{
	UpdateKeyboard();
    UpdateMouse();
	UpdateXInput();
}


// キーが押されているか？
bool IsKeyPress(int key)
{
	return s_keyState.keyState[key];
}

// キーが押されたか？
bool IsKeyPush(int key)
{
	return s_keyState.keyState[key] & !s_keyState.keyStateOld[key];
}

// キーが離されたか？
bool IsKeyRelease(int key)
{
	return !s_keyState.keyState[key] & s_keyState.keyStateOld[key];
}

// パッドの userIndex 取得
int GetPadUserIndex(int index)
{
	if (index < 0 || index >= PAD_MAX) { return INVALID_PAD_USER_INDEX; }
	return s_padIndexList[index];
}

// (DEBUG用)パッドの userIndex 取得
int DebugGetPadUserIndex(int index)
{
	return GetPadUserIndex(index);
}

// パッドがつながっているか
bool IsPadConnect(int index)
{
	int userIndex = GetPadUserIndex(index);
	return IsPadConnectDirect(userIndex);
}

// パッドが押されているか？
bool IsPadPress(int index, int pad)
{
	int userIndex = GetPadUserIndex(index);
	return IsPadPressDirect(userIndex, pad);
}

// パッドが押されたか？
bool IsPadPush(int index, int pad)
{
	int userIndex = GetPadUserIndex(index);
	return IsPadPushDirect(userIndex, pad);
}

// パッドが離されたか？
bool IsPadRelease(int index, int pad)
{
	int userIndex = GetPadUserIndex(index);
	return IsPadReleaseDirect(userIndex, pad);
}

// 左スティックのXを取得
float GetPadLeftStickX(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadLeftStickXDirect(userIndex);
}

// 左スティックのYを取得
float GetPadLeftStickY(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadLeftStickYDirect(userIndex);
}

// 右スティックのXを取得
float GetPadRightStickX(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadRightStickXDirect(userIndex);
}

// 右スティックのYを取得
float GetPadRightStickY(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadRightStickYDirect(userIndex);
}

// 左トリガー値を取得
float GetPadLeftTrigger(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadLeftTriggerDirect(userIndex);
}

// 右トリガー値を取得
float GetPadRightTrigger(int index)
{
	int userIndex = GetPadUserIndex(index);
	return GetPadRightTriggerDirect(userIndex);
}

// パッドがつながっているか
bool IsPadConnectDirect(int userIndex)
{
	return s_padState[userIndex].bConnected;
}

// パッドが押されているか？
bool IsPadPressDirect(int userIndex, int pad)
{
	// XINPUT_GAMEPAD_A
	if (userIndex == INVALID_PAD_USER_INDEX) { return false; }
	if (!s_padState[userIndex].bConnected) { return false; }
	return (s_padState[userIndex].xinputState.Gamepad.wButtons & pad) != 0;
}

// パッドが押されたか？
bool IsPadPushDirect(int userIndex, int pad)
{
	if (userIndex == INVALID_PAD_USER_INDEX) { return false; }
	if (!s_padState[userIndex].bConnected) { return false; }
	return (s_padState[userIndex].xinputState.Gamepad.wButtons & pad) && !(s_padState[userIndex].xinputStateOld.Gamepad.wButtons & pad);
}

// パッドが離されたか？
bool IsPadReleaseDirect(int userIndex, int pad)
{
	bool bOld = false;
	bool bNow = false;

	if (userIndex == INVALID_PAD_USER_INDEX) { return false; }
	bOld = s_padState[userIndex].bConnectedOld && (s_padState[userIndex].xinputStateOld.Gamepad.wButtons & pad);
	bNow = !s_padState[userIndex].bConnected || !(s_padState[userIndex].xinputState.Gamepad.wButtons & pad);
	return bNow & bOld;
}

// 左スティックのXを取得
float GetPadLeftStickXDirect(int userIndex)
{
	short value = 0;
	if (userIndex == INVALID_PAD_USER_INDEX) { return 0.0f; }
	value = s_padState[userIndex].xinputState.Gamepad.sThumbLX;
	if (value > 0) {
		return (float)value / SHRT_MAX;
	}
	else {
		return (float)-value / SHRT_MIN;
	}
}

// 左スティックのYを取得
float GetPadLeftStickYDirect(int userIndex)
{
	short value = 0;
	if (userIndex == INVALID_PAD_USER_INDEX) { return 0.0f; }
	value = s_padState[userIndex].xinputState.Gamepad.sThumbLY;
	if (value > 0) {
		return (float)value / SHRT_MAX;
	}
	else {
		return (float)-value / SHRT_MIN;
	}
}

// 右スティックのXを取得
float GetPadRightStickXDirect(int userIndex)
{
	short value = 0;
	if (userIndex == INVALID_PAD_USER_INDEX) { return 0.0f; }
	value = s_padState[userIndex].xinputState.Gamepad.sThumbRX;
	if (value > 0) {
		return (float)value / SHRT_MAX;
	}
	else {
		return (float)-value / SHRT_MIN;
	}
}

// 右スティックのYを取得
float GetPadRightStickYDirect(int userIndex)
{
	short value = 0;
	if (userIndex == INVALID_PAD_USER_INDEX) { return 0.0f; }
	value = s_padState[userIndex].xinputState.Gamepad.sThumbRY;
	if (value > 0) {
		return (float)value / SHRT_MAX;
	}
	else {
		return (float)-value / SHRT_MIN;
	}
}

// 左トリガーを取得
float GetPadLeftTriggerDirect(int userIndex)
{
	unsigned char value = 0;
	if(userIndex == INVALID_PAD_USER_INDEX) return 0;
	value = s_padState[userIndex].xinputState.Gamepad.bLeftTrigger;
	return (float)value / 255;
}

// 右トリガーを取得
float GetPadRightTriggerDirect(int userIndex)
{
	unsigned char value = 0;
	if (userIndex == INVALID_PAD_USER_INDEX) return 0;
	value = s_padState[userIndex].xinputState.Gamepad.bRightTrigger;
	return (float)value / 255;
}

// マウスカーソルの位置取得
bool GetMousePosition(int* px, int* py)
{
	POINT point;
	if (!GetCursorPos(&point)) return false;
	if (!IsFullScreen()) {
		ScreenToClient(GetWindowHandle(), &point);
	}
	*px = point.x;
	*py = point.y;
	return true;
}

// マウスホイール取得
int GetMouseWheelNotch(void)
{
	return s_mouseState.mouseWheelNotch;
}

/*!
 *	\brief	マウスボタン取得
 *	\param	[in] nVirtKey	VK_LBUTTON or VK_RBUTTON or VK_MBUTTON
 *	\return	最上位ビット:押下 最下位ビット:トグル
 *	\author	Kenji Nishida
 *	\date	2019/12/04
 */
short GetMouseButton(int nVirtKey)
{
	return GetKeyState(nVirtKey);
}