#include "TransformCbuf.h"



//Transformation Constant Buffer
TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//Update the constant buffer every frame
	vcbuf.Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection()));

	//bind the constant buffer to the graphic objects every frame
	vcbuf.Bind(gfx);
}

