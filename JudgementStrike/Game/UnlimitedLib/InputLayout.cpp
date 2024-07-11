#include "InputLayout.h"

// 初期化
bool InputLayout::Initialize(VertexShader& shader, D3D11_INPUT_ELEMENT_DESC* pElements, UINT num)
{
	auto pDevice = GetDevice();
	auto pCode = shader.GetShaderCode();
	// 入力レイアウトの生成
	if (FAILED(pDevice->CreateInputLayout(
		pElements,
		num,
		pCode->GetBufferPointer(),
		pCode->GetBufferSize(),
		&m_pInputLayout)))
	{
		return false;
	}
	return true;
}
// 終了処理
void InputLayout::Finalize(void)
{
	SAFE_RELEASE(m_pInputLayout);
}