#pragma once

#include "DrawableBase.h"

//Drawable Base templated on Box
class Box :public DrawableBase<Box> {

public:

	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(float dt) noexcept override;

	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:

	//positional
	float r;

	//angles
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;

	//*position
	float theta;
	float phi;
	float chi;

	//angular speed(delta/s)
	
	//*rotation about the box center
	float droll;
	float dpitch;
	float dyaw;

	//*rotation about the about the center of the world space
	float dtheta;
	float dphi;
	float dchi;


};
