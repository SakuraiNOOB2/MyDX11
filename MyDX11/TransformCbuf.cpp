#include "TransformCbuf.h"



//Transformation Constant Buffer
TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	parent(parent)
{

	//Check the constant buffer has been allocated yet or not
	if (!pVcbuf) {

		//Allocating the constant buffer
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx);
	}

}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{

	const auto model = parent.GetTransformXM();
	const Transforms transform = {
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};


	//Update the constant buffer every frame
	pVcbuf->Update(gfx, transform);

	//bind the constant buffer to the graphic objects every frame
	pVcbuf->Bind(gfx);
}

//Declaration for static variable
std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;

