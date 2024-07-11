/**
 *	@file
 *	@brief	Windows関連
 */
#include "Window.h"
#if USE_IMGUI
#include "../imgui/imgui_impl_win32.h"
#endif


// ウィンドウハンドル
static HWND g_hWnd = NULL;
// 終了通知が来ているか？
static bool g_isQuitMessage = false;
// ウィンドウサイズ
static int g_width = 0;
static int g_height = 0;

// マウスホイール情報取得
static int g_mouseWheelDeltaMod = 0;
static int g_mouseWheelNotch = 0;


LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// ウインドウプロシージャー
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch(msg)
	{
    case WM_MOUSEWHEEL: // マウスホイールが回転したときにフォーカスウィンドウに送信されます
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            delta += g_mouseWheelDeltaMod;
            int notch = delta / WHEEL_DELTA;
            g_mouseWheelDeltaMod = delta % WHEEL_DELTA;
            g_mouseWheelNotch += notch;
        }
        return 0;
	case WM_CLOSE:		// 閉じる際にWindowを破棄する
#if USE_IMGUI
		ImGui_ImplWin32_Shutdown();
#endif
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:	// プログラムの終了を通知する
		PostQuitMessage(0);
		return 0L;
	}
	// 既定のウィンドウプロシージャを呼び出す
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// シンプルなウィンドウの作成
bool CreateGameWindow(const char* pName, int x, int y, int width, int height, bool isFullscreen)
{
	WNDCLASSEX wcex;
	// ウインドウの設定
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= pName;
	wcex.hIcon			= LoadIcon(NULL, IDC_ARROW);
	wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassExA(&wcex);

	DWORD dwStyle = isFullscreen ? WS_VISIBLE | WS_POPUP : WS_OVERLAPPEDWINDOW;

	RECT Rect;
	Rect.left	= 0;
	Rect.top	= 0; 
	Rect.right	= width;
	Rect.bottom	= height;
	// ウィンドウのスタイルに合わせた適切なサイズを取得する
	AdjustWindowRect(&Rect, dwStyle, false);

	int w = Rect.right - Rect.left;
	int h = Rect.bottom - Rect.top;

	// ウインドウの生成
	g_hWnd = CreateWindowA(wcex.lpszClassName,
		pName,
		dwStyle,
		x,
		y,
		w,
		h,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	if( g_hWnd == NULL )
	{
		return false;
	}

	g_width = width;
	g_height = height;
	// ウインドウの表示
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

#if USE_IMGUI
	ImGui_ImplWin32_Init(g_hWnd);
#endif

	return true;
}

// メッセージの更新
bool UpdateWindowMessage(void)
{
	MSG msg;
	// メッセージが存在するか確認
	if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		// 終了通知が来ている場合は抜ける
		if( msg.message == WM_QUIT )
		{
			g_isQuitMessage = true;
		}
		else
		{
			// メッセージをウインドウプロシージャに転送
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	return false;
}

void QuitWindow(void) { SendMessage(g_hWnd, WM_CLOSE, 0, 0); }

// 終了通知が来ているか？
bool IsQuitMessage(void) { return g_isQuitMessage; }

// ウィンドウハンドルの取得
HWND GetWindowHandle(void) { return g_hWnd; }


// ウィンドウの横幅取得
int GetWindowWidth(void) { return g_width; }
// ウィンドウの縦幅取得
int GetWindowHeight(void) { return g_height; }

// マウスホイール情報取得
int GetMouseWheelNotchBefore(void) {
    return g_mouseWheelNotch;
}
// マウスホイール情報リセット
void UpdateMouseWheelNotch(void) {
    g_mouseWheelNotch = 0;
}

