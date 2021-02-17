#pragma once

#include "graphics.h"
#include <DirectXMath.h>

namespace Bind {

	class Bindable;
	class IndexBuffer;
}


class Drawable {

	//Make DrawableBase a friend class of Drawable for private access
	template<class T>
	friend class DrawableBase;

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
	void AddBind(std::unique_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noexcept(!IS_DEBUG);

private:

	//Bridge for Drawable class to access the staticBinds going to be declare by its children
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;

private:

	//special pointer to the transformation constant buffer
	const class Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> binds;
	
};