#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
	:count((UINT)indices.size())
{

	//import infomanager into current scope
	INFOMAN(gfx);
	

	//IndexBuffer construction
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(count * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();

	//Create index buffer
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	//bind the index buffer into pipeline
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

//get the indices number
UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
