#pragma once

#include "DirectX.h"

// �V�F�[�_�Ǘ��p�N���X�e���v���[�g
template<class T>
class Shader
{
public:
	Shader(void) : m_pShader(NULL) {}
	virtual ~Shader(void) { Finalize(); }

	// ������
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint) = 0;
	// �I������
	virtual void Finalize(void) { SAFE_RELEASE(m_pShader); }

	// �V�F�[�_�̎擾
	T* GetShader(void) { return m_pShader; }

protected:
	// �R���p�C��
	bool Compile(const char* pFilename, const char* pEntryPoint, const char* pShaderModel, ID3DBlob** ppBlob);

protected:
	T* m_pShader;	// �V�F�[�_
};

// VertexShader�N���X
class VertexShader : public Shader<ID3D11VertexShader>
{
public:
	VertexShader(void) : m_pShaderCode(NULL) {}
	// ������
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint);
	// �I������
	virtual void Finalize(void);
	// �V�F�[�_�R�[�h�̎擾
	ID3DBlob* GetShaderCode(void) { return m_pShaderCode; }

private:
	ID3DBlob* m_pShaderCode;
};

// PixelShader�N���X
class PixelShader : public Shader<ID3D11PixelShader>
{
public:
	// ������
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint);
};
