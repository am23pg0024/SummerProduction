#include "ResourceBase.h"


void* ResourceBase::Map(D3D11_MAP map)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(GetDeviceContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedResource)))
	{
		return nullptr;
	}
	return MappedResource.pData;
}

void ResourceBase::Unmap()
{
	GetDeviceContext()->Unmap(m_pBuffer, 0);
}