#pragma once

#include "DirectX.h"
#include "ResourceBase.h"


// IndexBufferクラス
class IndexBuffer : public ResourceBase
{
public:
	IndexBuffer(void);
	~IndexBuffer(void);

	// バッファの生成
	bool Create(UINT size, const void* pIndeces = nullptr) override;
	// バッファの解放
	void Finalize(void) override;

	// バッファの取得
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
};


class DynamicIndexBuffer : public IndexBuffer
{
public:
	bool Create(UINT size, const void* pIndeces = NULL) override;
};