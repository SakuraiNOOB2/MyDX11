#include "TransformCbuf.h"



//Transformation Constant Buffer
TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	parent(parent)
{

	//Check the constant buffer has been allocated yet or not
	if (!pVcbuf) {

		//Allocating the constant buffer
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}

}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//Update the constant buffer every frame
	pVcbuf->Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection()));

	//bind the constant buffer to the graphic objects every frame
	pVcbuf->Bind(gfx);
}

//Declaration for static variable
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;

