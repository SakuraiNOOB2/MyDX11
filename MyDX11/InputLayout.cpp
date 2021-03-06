#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

namespace Bind {

	InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
	{

		INFOMAN(gfx);

		//create InputLayout
		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout));


	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		//Bind input layout
		GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	}

}