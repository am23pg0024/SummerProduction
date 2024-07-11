#pragma once

#include "DirectX.h"

// シェーダ管理用クラステンプレート
template<class T>
class Shader
{
public:
	Shader(void) : m_pShader(NULL) {}
	virtual ~Shader(void) { Finalize(); }

	// 初期化
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint) = 0;
	// 終了処理
	virtual void Finalize(void) { SAFE_RELEASE(m_pShader); }

	// シェーダの取得
	T* GetShader(void) { return m_pShader; }

protected:
	// コンパイル
	bool Compile(const char* pFilename, const char* pEntryPoint, const char* pShaderModel, ID3DBlob** ppBlob);

protected:
	T* m_pShader;	// シェーダ
};

// VertexShaderクラス
class VertexShader : public Shader<ID3D11VertexShader>
{
public:
	VertexShader(void) : m_pShaderCode(NULL) {}
	// 初期化
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint);
	// 終了処理
	virtual void Finalize(void);
	// シェーダコードの取得
	ID3DBlob* GetShaderCode(void) { return m_pShaderCode; }

private:
	ID3DBlob* m_pShaderCode;
};

// PixelShaderクラス
class PixelShader : public Shader<ID3D11PixelShader>
{
public:
	// 初期化
	virtual bool Initialize(const char* pFilename, const char* pEntryPoint);
};
