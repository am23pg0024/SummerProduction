#pragma once
/**
 *	@file
 *	@brief	DirectX関連
 */

// DirectX11のヘッダーを含める
#include <d3d11.h>
// DirectX9のヘッダーを含める
#include <d3d9.h>



// 解放処理用のマクロを定義
#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; } }
#define SAFE_DELETE(x) { if(x) { delete (x); (x) = NULL; } }


#define	CONSTANT_BUFFER_SIZE	1024

enum VERTEX_FORMAT
{
	VERTEX_FORMAT_SIMPLE_2D,
	VERTEX_FORMAT_MAX,
};

enum SHADER_TYPE
{
	SHADER_TYPE_SIMPLE_2D,
	SHADER_TYPE_SIMPLE_2D_TEX,
	SHADER_TYPE_SIMPLE_FONT,
	SHADER_TYPE_MAX,
};

// DirectXの初期化
bool InitializeDirectX(HWND hWnd, int width, int height, bool isFullscreen);
// DirectXの終了処理
void FinalizeDirectX(void);

bool IsFullScreen(void);

// Deviceの取得
ID3D11Device* GetDevice(void);
// DeviceContextオブジェクトの取得
ID3D11DeviceContext* GetDeviceContext(void);

UINT GetTargetWidth();
UINT GetTargetHeight();

// 描画シーンの開始
void BeginRenderScene(void);
// 描画シーンの終了
void EndRenderScene(void);
// バックバッファのクリア
void ClearBackBuffer(DWORD target, D3DCOLOR color, float depth = 1.0f, DWORD stencil = 0);
// 画面のクリア
void ClearDisp(D3DCOLOR color = 0x00000000);
// 画面のフリップ
void FlipDisp(void);


// 初期の描画ステートを設定
void SetDefaultRenderState(void);

// 描画モード
enum ERenderMode
{
	Normal,		// 書き込み
	Add,		// 加算
	Subtract,	// 減算
	Multiple,	// 乗算
};

// 描画ステートの設定
void SetRenderMode(ERenderMode mode, bool enableAlpha);
// 頂点フォーマットの設定
void SetInputLayout(VERTEX_FORMAT fmt);

// シェーダーの設定
void SetShader(SHADER_TYPE type);
// シェーダパラメータの設定
void SetShaderParam(void* pParam, int size);

// 描画処理
void DrawPrimitiveUP(const void* pVertex, UINT stride, UINT count);
void DrawIndexedPrimitiveUP(const UINT16* pIndex, UINT indexCount, const void* pVertex, UINT stride, UINT vertexCount);

