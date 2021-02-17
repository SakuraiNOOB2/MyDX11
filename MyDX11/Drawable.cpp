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

	//Bind all the static binds
	for (auto& b : GetStaticBinds()) {

		b->Bind(gfx);
	}

	//Draw command
	gfx.DrawIndexed(pIndexBuffer->GetCount());

}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{

	//Check binds' typeid is IndexBuffer or not
	assert("**Must** use ONLY AddIndexBuffer to bind index buffer!" && typeid(*bind) != typeid(IndexBuffer));

	binds.push_back(std::move(bind));

}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{

	//
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);

	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));

}
