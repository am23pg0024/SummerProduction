#include "InputLayout.h"

// ������
bool InputLayout::Initialize(VertexShader& shader, D3D11_INPUT_ELEMENT_DESC* pElements, UINT num)
{
	auto pDevice = GetDevice();
	auto pCode = shader.GetShaderCode();
	// ���̓��C�A�E�g�̐���
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
// �I������
void InputLayout::Finalize(void)
{
	SAFE_RELEASE(m_pInputLayout);
}