#pragma once

#include "DirectX.h"
#include "ResourceBase.h"

// VertexBufferクラス
class VertexBuffer : public ResourceBase
{
public:
	VertexBuffer(void);
	~VertexBuffer(void);

	// バッファの生成
	bool Create(UINT size, const void* pVertices = NULL) override;
	// バッファの解放
	void Finalize(void) override;

	// バッファの取得
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
};


class DynamicVertexBuffer : public VertexBuffer
{
public:
	bool Create(UINT size, const void* pVertices = NULL) override;
};