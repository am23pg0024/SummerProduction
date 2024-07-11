#pragma once

#include "DirectX.h"
#include "ResourceBase.h"


// IndexBuffer�N���X
class IndexBuffer : public ResourceBase
{
public:
	IndexBuffer(void);
	~IndexBuffer(void);

	// �o�b�t�@�̐���
	bool Create(UINT size, const void* pIndeces = nullptr) override;
	// �o�b�t�@�̉��
	void Finalize(void) override;

	// �o�b�t�@�̎擾
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
};


class DynamicIndexBuffer : public IndexBuffer
{
public:
	bool Create(UINT size, const void* pIndeces = NULL) override;
};