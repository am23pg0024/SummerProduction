#include "IndexBuffer.h"


IndexBuffer::IndexBuffer(void)
	: m_pBuffer(NULL)
{}
IndexBuffer::~IndexBuffer(void)
{
	Finalize();
}
// インデックスバッファの生成
bool IndexBuffer::Create(UINT size, const void* pIndeces)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pIndeces;
	data.SysMemPitch = size;
	data.SysMemSlicePitch = size;
	if (FAILED(GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
	{
		return false;
	}

	return true;
}


void IndexBuffer::Finalize(void)
{
	SAFE_RELEASE(m_pBuffer);
}

bool DynamicIndexBuffer::Create(UINT size, const void* pIndeces)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA* pInit = nullptr;
	D3D11_SUBRESOURCE_DATA data;
	if (pIndeces) {
		data.pSysMem = pIndeces;
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

