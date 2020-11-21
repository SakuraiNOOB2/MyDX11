#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class InputLayout :public Bindable {

public:

	//Constructor
	InputLayout(Graphics& gfx,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* pVertexShaderByteCode);

	void Bind(Graphics& gfx) noexcept override;

protected:

	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

};