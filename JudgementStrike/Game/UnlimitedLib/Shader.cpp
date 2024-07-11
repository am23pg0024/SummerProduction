#include "DirectX.h"
#include "Shader.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

// �R���p�C��
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
		// �V�F�[�_�̃G���[���e��\��
		MessageBox(NULL, (char*)pErrorMsg->GetBufferPointer(), "Compile Error", MB_OK);

		SAFE_RELEASE(pErrorMsg);
		return false;
	}
	return true;
}

// ������
bool VertexShader::Initialize(const char* pFilename, const char* pEntryPoint)
{
	ID3DBlob* pBlob;
	if (!Compile(pFilename, pEntryPoint, "vs_5_0", &pBlob))
	{
		return false;
	}
	auto pDevice = GetDevice();
	HRESULT hr;
	// ���_�V�F�[�_�̐���
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
// �I������
void VertexShader::Finalize(void)
{
	SAFE_RELEASE(m_pShader);
	Shader::Finalize();
}

// ������
bool PixelShader::Initialize(const char* pFilename, const char* pEntryPoint)
{
	ID3DBlob* pBlob;
	if (!Compile(pFilename, pEntryPoint, "ps_5_0", &pBlob))
	{
		return false;
	}
	auto pDevice = GetDevice();
	HRESULT hr;
	// �s�N�Z���V�F�[�_�̐���
	hr = pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		NULL,
		&m_pShader
	);
	SAFE_RELEASE(pBlob);

	return SUCCEEDED(hr);
}