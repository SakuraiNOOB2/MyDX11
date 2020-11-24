#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>


class TransformCbuf :public Bindable {

public:

	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;

private:

	//dynamic allocated static VertexConstantBuffer
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
	
	//Grab the matrix from it's parent and update to vcbuf
	const Drawable& parent;

};