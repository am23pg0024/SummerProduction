#include "DirectX.h"
#include "Shader.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

// コンパイル
template<class T>
bool Shader<T>::Compile(const char* pFilename, const char* pEntryPoint, const char* pShaderModel, ID3DBlob** ppBlob)
{
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, pFilename, _TRUNCATE);

	ID3DBlob* pErrorMsg;
	HRESULT hr = D3DCompileFromFile(
		path,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntryPoint,
		pShaderModel,
		0,
		0,
		ppBlob,
		&pErrorMsg
	);
	if (FAILED(hr))
	{
		// シェーダのエラー内容を表示
		MessageBox(NULL, (char*)pErrorMsg->GetBufferPointer(), "Compile Error", MB_OK);

		SAFE_RELEASE(pErrorMsg);
		return false;
	}
	return true;
}

// 初期化
bool VertexShader::Initialize(const char* pFilename, const char* pEntryPoint)
{
	ID3DBlob* pBlob;
	if (!Compile(pFilename, pEntryPoint, "vs_5_0", &pBlob))
	{
		return false;
	}
	auto pDevice = GetDevice();
	HRESULT hr;
	// 頂点シェーダの生成
	hr = pDevice->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		NULL,
		&m_pShader
	);
	if (FAILED(hr)) {
		SAFE_RELEASE(pBlob);
		return false;
	}
	m_pShaderCode = pBlob;

	return true;
}
// 終了処理
void VertexShader::Finalize(void)
{
	SAFE_RELEASE(m_pShader);
	Shader::Finalize();
}

// 初期化
bool PixelShader::Initialize(const char* pFilename, const char* pEntryPoint)
{
	ID3DBlob* pBlob;
	if (!Compile(pFilename, pEntryPoint, "ps_5_0", &pBlob))
	{
		return false;
	}
	auto pDevice = GetDevice();
	HRESULT hr;
	// ピクセルシェーダの生成
	hr = pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		NULL,
		&m_pShader
	);
	SAFE_RELEASE(pBlob);

	return SUCCEEDED(hr);
}