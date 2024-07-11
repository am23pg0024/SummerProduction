//! \file
#include "UnlimitedLib.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include <vector>
#if USE_IMGUI
#include "../imgui/imgui_impl_dx11.h"
#endif

#pragma comment(lib, "d3d11.lib")


const UINT DynamicBufferSize = 1024 * 1024 * 4;
const UINT DynamicIndexBufferSize = 1024 * 1024;

static bool g_isFullScreen;
static UINT g_FrameCount;
static UINT g_DynamicBufferOffset;
static UINT g_DynamicIndexBufferOffset;
static ID3D11Device* g_pDx11Device = NULL;
static ID3D11DeviceContext* g_pDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain;
static ID3D11RenderTargetView* g_pRenderTargetView;
static DynamicVertexBuffer g_DynamicVertexBuffer;
static DynamicIndexBuffer g_DynamicIndexBuffer;
static VertexShader g_VertexShader;
static PixelShader g_PixelShader2D;
static PixelShader g_PixelShader2DTex;
static PixelShader g_PixelShaderFont;
static InputLayout g_InputLayout[VERTEX_FORMAT_MAX];
static ConstantBuffer g_ConstantBuffer;

VertexShader* g_pVertexShader = NULL;
PixelShader* g_pPixelShader = NULL;

static UINT g_RenderTargetWidth;
static UINT g_RenderTargetHeight;

bool InitShader();
void InitInputLayout();

// DirectXの初期化
bool InitializeDirectX(HWND hWnd, int width, int height, bool isFullscreen)
{
	if (g_pDx11Device != NULL || g_pDeviceContext != NULL) return false;

	DXGI_SWAP_CHAIN_DESC dxgi;
	ZeroMemory(&dxgi, sizeof(dxgi));

	dxgi.BufferCount = 1;
	dxgi.BufferDesc.Width = width;
	dxgi.BufferDesc.Height = height;
	dxgi.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgi.BufferDesc.RefreshRate.Numerator = 60;
	dxgi.BufferDesc.RefreshRate.Denominator = 1;
	dxgi.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgi.OutputWindow = hWnd;
	dxgi.SampleDesc.Count = 1;
	dxgi.SampleDesc.Quality = 0;
	dxgi.Windowed = true;

	UINT flag = 0;
#if _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL level;
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flag,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&dxgi,
		&g_pSwapChain,
		&g_pDx11Device,
		&level,
		&g_pDeviceContext
	)))
	{
		return false;
	}

	// レンダーバッファの作成
	ID3D11Texture2D* pBuffer;
	if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer)))
	{
		return false;
	}

	if (FAILED(g_pDx11Device->CreateRenderTargetView(pBuffer, NULL, &g_pRenderTargetView)))
	{
		FinalizeDirectX();
		return false;
	}
	SAFE_RELEASE(pBuffer);

	if (!g_DynamicVertexBuffer.Create(DynamicBufferSize * 2)) {
		FinalizeDirectX();
		return false;
	}
	if (!g_DynamicIndexBuffer.Create(DynamicIndexBufferSize * 2)) {
		FinalizeDirectX();
		return false;
	}
	if (!InitShader()) {
		FinalizeDirectX();
		return false;
	}
	InitInputLayout();

	g_isFullScreen = isFullscreen;
	g_FrameCount = 0;
	g_DynamicBufferOffset = 0;
	g_DynamicIndexBufferOffset = 0;
	g_RenderTargetWidth = width;
	g_RenderTargetHeight = height;

#if USE_IMGUI
	ImGui_ImplDX11_Init(g_pDx11Device, g_pDeviceContext);
#endif

	return true;
}

// DirectXの終了処理
void FinalizeDirectX(void)
{
#if USE_IMGUI
	ImGui_ImplDX11_Shutdown();
#endif

	for (auto& input : g_InputLayout)
	{
		input.Finalize();
	}
	g_ConstantBuffer.Finalize();
	g_VertexShader.Finalize();
	g_PixelShader2D.Finalize();
	g_PixelShader2DTex.Finalize();
	g_PixelShaderFont.Finalize();

	g_DynamicIndexBuffer.Finalize();
	g_DynamicVertexBuffer.Finalize();

	SAFE_RELEASE(g_pDx11Device);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pRenderTargetView);
}

void InitInputLayout()
{
	for (int i = 0; i < VERTEX_FORMAT_MAX; i++)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
		// 入力レイアウト
		switch (i)
		{
		case VERTEX_FORMAT_SIMPLE_2D:
			layouts.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layouts.push_back({ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 4 * 4, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layouts.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 5, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		}
		g_InputLayout[i].Initialize(g_VertexShader, layouts.data(), layouts.size());
	}
}

bool InitShader()
{
	if (!g_VertexShader.Initialize("assets/shader/2D.hlsl", "VS")) {
		MessageBox(NULL, "VertexShader", "Error", MB_OK);
		return false;
	}

	if (!g_PixelShader2D.Initialize("assets/shader/2D.hlsl", "PS")) {
		MessageBox(NULL, "PixelShader", "Error", MB_OK);
		return false;
	}
	if (!g_PixelShader2DTex.Initialize("assets/shader/2D.hlsl", "PSTex")) {
		MessageBox(NULL, "PixelShader", "Error", MB_OK);
		return false;
	}
	if (!g_PixelShaderFont.Initialize("assets/shader/2D.hlsl", "PSFont")) {
		MessageBox(NULL, "PixelShader", "Error", MB_OK);
		return false;
	}
	// カスタム用のコンスタントバッファを初期化
	if (!g_ConstantBuffer.Create(CONSTANT_BUFFER_SIZE)) {
		return false;
	}
	return true;
}

// フルスクリーンモード状態取得
bool IsFullScreen(void) { return g_isFullScreen; }

// Deviceの取得
ID3D11Device* GetDevice(void) { return g_pDx11Device; }
// DeviceContextオブジェクトの取得
ID3D11DeviceContext* GetDeviceContext(void) { return g_pDeviceContext; }


UINT GetTargetWidth() { return g_RenderTargetWidth; }
UINT GetTargetHeight() { return g_RenderTargetHeight; }

// 描画シーンの開始
void BeginRenderScene(void)
{
	ID3D11DeviceContext* pContext = GetDeviceContext();
	pContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = (float)g_RenderTargetWidth;
	viewport.Height= (float)g_RenderTargetHeight;
	pContext->RSSetViewports(1, &viewport);
}

// 描画シーンの終了
void EndRenderScene(void)
{
}

// バックバッファのクリア
void ClearBackBuffer(DWORD target, D3DCOLOR color, float depth, DWORD stencil)
{
	float clearColor[4]
	{
		(float)((color >>  0) & 0xff) / 255.0f,
		(float)((color >>  8) & 0xff) / 255.0f,
		(float)((color >> 16) & 0xff) / 255.0f,
		(float)((color >> 24) & 0xff) / 255.0f,
	};

	ID3D11DeviceContext* pContext = GetDeviceContext();
	if (pContext) pContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
}

// 画面のクリア
void ClearDisp(D3DCOLOR color)
{
	ClearBackBuffer(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, color, 1.0f, 0);
}

// 画面のフリップ
void FlipDisp(void)
{
	if (g_pSwapChain == NULL) return;

	g_pSwapChain->Present(1, 0);

	g_FrameCount++;
	g_DynamicBufferOffset = (g_FrameCount & 1) * DynamicBufferSize;
	g_DynamicIndexBufferOffset = (g_FrameCount & 1) * DynamicIndexBufferSize;
}

// 初期の描画ステートを設定
void SetDefaultRenderState(void)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetDeviceContext();

	D3D11_RASTERIZER_DESC rasterDesc{};
	//　ラスタライズステートの設定
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.DepthClipEnable = true;

	ID3D11RasterizerState* pRasterState;
	pDevice->CreateRasterizerState(&rasterDesc, &pRasterState);
	pContext->RSSetState(pRasterState);
	SAFE_RELEASE(pRasterState);

	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	//　デプスステンシルテストの設定
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.StencilEnable = false;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.BackFace = depthDesc.FrontFace;

	ID3D11DepthStencilState* pDepthState;
	pDevice->CreateDepthStencilState(&depthDesc, &pDepthState);
	pContext->OMSetDepthStencilState(pDepthState, 0xff);
	SAFE_RELEASE(pDepthState);

	//　ブレンドステートの設定
	SetRenderMode(Normal, true);


	D3D11_SAMPLER_DESC samplerDesc{};
	//　サンプラーステートの設定
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = 0.f;

	ID3D11SamplerState* pSamplerState;
	pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	pContext->PSSetSamplers(0, 1, &pSamplerState);

	SAFE_RELEASE(pSamplerState);
}

// 描画ステートの設定
void SetRenderMode(ERenderMode mode, bool enableAlpha)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetDeviceContext();
	D3D11_BLEND_DESC blendDesc{};


	// αが無効な場合は入力されたカラーをそのまま使う
	D3D11_BLEND srcColorblend = D3D11_BLEND_ONE;
	D3D11_BLEND dstColorblend = D3D11_BLEND_ZERO;
	if (enableAlpha)
	{
		// αが有効な場合はα値をカラーに影響させる
		srcColorblend = D3D11_BLEND_SRC_ALPHA;
		dstColorblend = D3D11_BLEND_INV_SRC_ALPHA;
	}

	switch (mode)
	{
	case Normal:
		// αが有効：(SrcColor * 1) + (DestColor * 0)
		// αが無効：(SrcColor * α) + (DestColor * (1 - α))
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = srcColorblend;
		blendDesc.RenderTarget[0].DestBlend = dstColorblend;
		break;
	case Add:
		// αが有効：(SrcColor * 1) + (DestColor * 1)
		// αが無効：(SrcColor * α) + (DestColor * 1)
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = srcColorblend;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		break;
	case Subtract:
		// αが有効：(DestColor * 1) - (SrcColor * 1)
		// αが無効：(DestColor * 1) - (SrcColor * α)
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlend = srcColorblend;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		break;
	case Multiple:
		// 共通：(SrcColor * 0) + (DestColor * SrcColor)
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		break;
	}
	//　ブレンドステートの設定
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* pBlendState;
	pDevice->CreateBlendState(&blendDesc, &pBlendState);

	float color[4]{ 1, 1, 1, 1 };
	pContext->OMSetBlendState(pBlendState, color, 0xffffffff);

	SAFE_RELEASE(pBlendState);
}

// 頂点フォーマットの設定
void SetInputLayout(VERTEX_FORMAT fmt)
{
	GetDeviceContext()->IASetInputLayout(g_InputLayout[fmt].GetInputLayout());
}


// シェーダーの設定
void SetShader(SHADER_TYPE type)
{
	auto pContext = GetDeviceContext();

	VertexShader* pVertexShader = NULL;
	PixelShader* pPixelShader = NULL;
	switch (type)
	{
	case SHADER_TYPE_SIMPLE_2D:
		pVertexShader = &g_VertexShader;
		pPixelShader = &g_PixelShader2D;
		break;
	case SHADER_TYPE_SIMPLE_2D_TEX:
		pVertexShader = &g_VertexShader;
		pPixelShader = &g_PixelShader2DTex;
		break;
	case SHADER_TYPE_SIMPLE_FONT:
		pVertexShader = &g_VertexShader;
		pPixelShader = &g_PixelShaderFont;
		break;
	}
	// カスタムシェーダの置き換え対応
	if (g_pVertexShader) {
		pVertexShader = g_pVertexShader;
	}
	if (g_pPixelShader) {
		pPixelShader = g_pPixelShader;
	}
	pContext->VSSetShader(pVertexShader->GetShader(), NULL, 0);
	pContext->PSSetShader(pPixelShader->GetShader(), NULL, 0);
}


bool UpdateDynamicBuffer(ID3D11Resource* pResource, const void* pBuffer, UINT size, UINT& offset)
{
	auto pContext = GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(pContext->Map(pResource, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedResource)))
	{
		return false;
	}
	auto pOffset = reinterpret_cast<char*>(MappedResource.pData) + offset;

	memcpy(pOffset, pBuffer, size);
	pContext->Unmap(pResource, 0);

	offset += size;

	return true;
}

void DrawPrimitiveUP(const void* pVertex, UINT stride, UINT count)
{
	auto pVertexBuffer = g_DynamicVertexBuffer.GetBuffer();
	auto pContext = GetDeviceContext();

	UINT vertexOffset = g_DynamicBufferOffset;
	UpdateDynamicBuffer(pVertexBuffer, pVertex, stride * count, g_DynamicBufferOffset);

	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &vertexOffset);
	pContext->Draw(count, 0);
}

void DrawIndexedPrimitiveUP(const UINT16* pIndex, UINT indexCount, const void* pVertex, UINT stride, UINT vertexCount)
{
	auto pVertexBuffer = g_DynamicVertexBuffer.GetBuffer();
	auto pIndexBuffer = g_DynamicIndexBuffer.GetBuffer();
	auto pContext = GetDeviceContext();

	UINT vertexOffset = g_DynamicBufferOffset;
	UINT indexOffset = g_DynamicIndexBufferOffset;
	UpdateDynamicBuffer(pVertexBuffer, pVertex, stride * vertexCount, g_DynamicBufferOffset);
	UpdateDynamicBuffer(pIndexBuffer, pIndex, sizeof(UINT16) * indexCount, g_DynamicIndexBufferOffset);

	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &vertexOffset);
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, indexOffset);
	pContext->DrawIndexed(indexCount, 0, 0);
}

// シェーダパラメータの設定
void SetShaderParam(void* pParam, int size)
{
	auto pContext = GetDeviceContext();

	g_ConstantBuffer.UpdateBuffer(pParam, size);

	ID3D11Buffer* pCB = g_ConstantBuffer.GetBuffer();
	pContext->PSSetConstantBuffers(0, 1, &pCB);
}