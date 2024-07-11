#pragma once

#include "DirectX.h"
#include "ResourceBase.h"

// ConstantBufferクラス
class ConstantBuffer : public ResourceBase
{
public:
	ConstantBuffer(void);
	~ConstantBuffer(void);

	// バッファの生成
	bool Create(UINT size, const void* pBuffer = NULL) override;
	// バッファの解放
	void Finalize(void) override;

	// バッファの更新
	void UpdateBuffer(const void* pParam, int size);

	// バッファの取得
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
	int m_Size;
	void* m_pParam;
};

