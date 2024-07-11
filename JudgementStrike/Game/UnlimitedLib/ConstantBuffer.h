#pragma once

#include "DirectX.h"
#include "ResourceBase.h"

// ConstantBuffer�N���X
class ConstantBuffer : public ResourceBase
{
public:
	ConstantBuffer(void);
	~ConstantBuffer(void);

	// �o�b�t�@�̐���
	bool Create(UINT size, const void* pBuffer = NULL) override;
	// �o�b�t�@�̉��
	void Finalize(void) override;

	// �o�b�t�@�̍X�V
	void UpdateBuffer(const void* pParam, int size);

	// �o�b�t�@�̎擾
	ID3D11Buffer* GetBuffer(void) { return m_pBuffer; }

protected:
	ID3D11Buffer* m_pBuffer;
	int m_Size;
	void* m_pParam;
};

