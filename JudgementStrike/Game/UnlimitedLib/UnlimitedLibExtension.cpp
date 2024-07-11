#include "UnlimitedLib.h"


static bool enableScissor = false;


void SetScissorState(bool enable)
{
	if (enableScissor == enable)
	{
		return;
	}
	enableScissor = enable;

	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetDeviceContext();

	D3D11_RASTERIZER_DESC rasterDesc{};
	//　ラスタライズステートの設定
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = enable;

	ID3D11RasterizerState* pRasterState;
	pDevice->CreateRasterizerState(&rasterDesc, &pRasterState);
	pContext->RSSetState(pRasterState);
	SAFE_RELEASE(pRasterState);
}

void BeginScissor(int x, int y, int width, int height)
{
	SetScissorState(true);

	ID3D11DeviceContext* pContext = GetDeviceContext();

	D3D11_RECT rect;
	rect.left = x;
	rect.right = x + width;
	rect.top = y;
	rect.bottom = y + height;

	pContext->RSSetScissorRects(1, &rect);
}

void EndScissor()
{
	SetScissorState(false);
}


void SetStencilTest()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetDeviceContext();

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
}



#define	SHADER_MAX	128

struct LoadedShaderInfo
{
	char		filename[MAX_PATH];
	int			count;
	PixelShader shader;
};
static LoadedShaderInfo g_loadedShaders[SHADER_MAX] = {};


int LoadCustomShader(const char* pFileName, const char* pEntryPoint)
{
	for (int i = 0; i < SHADER_MAX; i++)
	{
		LoadedShaderInfo* pInfo = &g_loadedShaders[i];
		if (pInfo->count == 0) continue;
	}

	for (int i = 0; i < SHADER_MAX; i++)
	{
		LoadedShaderInfo* pInfo = &g_loadedShaders[i];
		if (pInfo->count != 0) continue;

		pInfo->shader.Initialize(pFileName, pEntryPoint);

		strcpy(pInfo->filename, pFileName);
		pInfo->count++;
		return i;
	}
	return -1;
}

void ReleaseCustomShader(int index)
{
	if (index < 0 || index >= SHADER_MAX) return;

	LoadedShaderInfo* pInfo = &g_loadedShaders[index];
	if (-- pInfo->count == 0) return;

	pInfo->shader.Finalize();
}

void BeginCustomShader(int index)
{
	LoadedShaderInfo* pInfo = &g_loadedShaders[index];

	BeginShader(&pInfo->shader);
}

void EndCustomShader()
{
	EndShader();
}

// シェーダパラメータの設定
void SetCustomShaderParam(void* pParam, int size)
{
	SetShaderParam(pParam, size);
}