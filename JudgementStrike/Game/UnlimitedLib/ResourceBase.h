#pragma once

#include "DirectX.h"


// ResourceBase�N���X
class ResourceBase
{
public:
	ResourceBase(void) : m_pBuffer(nullptr) {}
	virtual ~ResourceBase(void) = default;

	// �o�b�t�@�̐���
	virtual bool Create(UINT size, const void* pInit = nullptr) = 0;
	// �o�b�t�@�̉��
	virtual void Finalize(void) = 0;

	// �o�b�t�@�̎擾
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

	void* Map(D3D11_MAP map);
	void Unmap();

protected:
	ID3D11Buffer* m_pBuffer;
};

