#pragma once

#include "DrawableBase.h"

class SolidSphere:public DrawableBase<SolidSphere>
{
public:

	//constructor
	SolidSphere(Graphics& gfx, float radius);
	
	//member functions
	void Update(float dt) noexcept override;
	
	void SetPosition(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:

	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };

};

