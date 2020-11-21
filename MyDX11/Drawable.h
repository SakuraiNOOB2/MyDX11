#pragma once

#include "graphics.h"
#include <DirectXMath.h>


class Bindable;

class Drawable {

public:

	//constructor
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;

	//Add bind
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);

	//Add IndexBuffer
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;


	//destructor
	virtual ~Drawable() = default;

private:

	//special pointer to the transformation constant buffer
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;

};