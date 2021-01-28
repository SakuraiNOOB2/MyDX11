#pragma once
#include "DrawableBase.h"

template<class T>
class TestObject :public DrawableBase<T> {

public:

	//contructor
	TestObject(Graphics& gfx,std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist)
		:
		r(rdist(rng)),
		droll(ddist(rng)),
		dpitch(ddist(rng)),
		dyaw(ddist(rng)),
		dphi(odist(rng)),
		dtheta(odist(rng)),
		dchi(odist(rng)),
		chi(adist(rng)),
		theta(adist(rng)),
		phi(adist(rng))
	{}

	void Update(float dt) noexcept {

		//get delta time(dt) to update the angles
		roll += droll * dt;
		pitch += dpitch * dt;
		yaw += dyaw * dt;

		theta += dtheta * dt;
		phi += dphi * dt;
		chi += dchi * dt;

	}

	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			   DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
			   DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}

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