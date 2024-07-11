#include "ConstantBuffer.h"


ConstantBuffer::ConstantBuffer(void)
	: m_pBuffer(NULL)
{}
ConstantBuffer::~ConstantBuffer(void)
{
	Finalize();
}

// 頂点バッファの生成
bool ConstantBuffer::Create(UINT size, const void* pBuffer)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	D3D11_SUBRESOURCE_DATA* pInit = nullptr;
	if(pInit)
	{
		data.pSysMem = pBuffer;
		data.SysMemPitch = size;
		data.SysMemSlicePitch = size;
		pInit = &data;
	}

	if (FAILED(GetDevice()->CreateBuffer(&desc, pInit, &m_pBuffer)))
	{
		return false;
	}

	return true;
}

void ConstantBuffer::Finalize(void)
{
	SAFE_RELEASE(m_pBuffer);
}

void ConstantBuffer::UpdateBuffer(const void* pParam, int size)
{
	if(size < m_Size) return;

	auto pContext = GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE resource;
	pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, pParam, size);
	pContext->Unmap(m_pBuffer, 0);
}
