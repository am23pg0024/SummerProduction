#pragma once

#include "DirectX.h"
#include "ResourceBase.h"

// VertexBuffer�N���X
class VertexBuffer : public ResourceBase
{
public:
	VertexBuffer(void);
	~VertexBuffer(void);

	// �o�b�t�@�̐���
	bool Create(UINT size, const void* pVertices = NULL) override;
	// �o�b�t�@�̉��
	void Finalize(void) override;

	// �o�b�t�@�̎擾
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
};


class DynamicVertexBuffer : public VertexBuffer
{
public:
	bool Create(UINT size, const void* pVertices = NULL) override;
};