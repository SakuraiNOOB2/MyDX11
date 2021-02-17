#include "TransformCbuf.h"


namespace Bind {

	//Transformation Constant Buffer
	TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		parent(parent)
	{

		//Check the constant buffer has been allocated yet or not
		if (!pVcbuf) {

			//Allocating the constant buffer
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}

	}

	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{

		const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		const Transforms m_transform = {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection()
			)
		};


		//Update the constant buffer every frame
		pVcbuf->Update(gfx, m_transform);

		//bind the constant buffer to the graphic objects every frame
		pVcbuf->Bind(gfx);
	}

	//Declaration for static variable
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;

}