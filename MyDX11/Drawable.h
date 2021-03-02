#pragma once

#include "graphics.h"
#include <DirectXMath.h>
#include <memory>

namespace Bind {

	class Bindable;
	class IndexBuffer;
}


class Drawable {


public:

	//constructor
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept {}

	//destructor
	virtual ~Drawable() = default;

protected:

	//template for querying the bindables
	//used for changing particular bindables
	template<class T>
	T* QueryBindable() noexcept {

		for (auto& pb : binds) {

			if (auto pt = dynamic_cast<T*>(pb.get())) {

				return pt;
			}

		}

		return nullptr;
	}

	//Adding Bindables and IndexBuffer
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);

private:

	//special pointer to the transformation constant buffer
	const class Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
	
};