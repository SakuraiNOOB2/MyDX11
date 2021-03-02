#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	//Bind all the instance binds
	for (auto& b : binds) {

		b->Bind(gfx);
	}


	//Draw command
	gfx.DrawIndexed(pIndexBuffer->GetCount());

}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{

	//Check binds' typeid is IndexBuffer or not
	if (typeid(*bind) == typeid(IndexBuffer)){

		assert("Binding multiple index buffers not allowed" && pIndexBuffer == nullptr);
		
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}

	binds.push_back(std::move(bind));

}

