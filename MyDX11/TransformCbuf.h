#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>


class TransformCbuf :public Bindable {

private:

	struct Transforms{

		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};

public:

	TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;

private:

	//dynamic allocated static VertexConstantBuffer
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	
	//Grab the matrix from it's parent and update to vcbuf
	const Drawable& parent;

};