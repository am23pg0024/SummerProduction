#pragma once

#include "DirectX.h"


// ResourceBaseクラス
class ResourceBase
{
public:
	ResourceBase(void) : m_pBuffer(nullptr) {}
	virtual ~ResourceBase(void) = default;

	// バッファの生成
	virtual bool Create(UINT size, const void* pInit = nullptr) = 0;
	// バッファの解放
	virtual void Finalize(void) = 0;

	// バッファの取得
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

	void* Map(D3D11_MAP map);
	void Unmap();

protected:
	ID3D11Buffer* m_pBuffer;
};

