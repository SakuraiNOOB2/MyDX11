#pragma once

#include "graphics.h"
#include <DirectXMath.h>


class Bindable;

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
	virtual void Update(float dt) noexcept = 0;

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
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG);

private:

	//Bridge for Drawable class to access the staticBinds going to be declare by its children
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:

	//special pointer to the transformation constant buffer
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
	
};