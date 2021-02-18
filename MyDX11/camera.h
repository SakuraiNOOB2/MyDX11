#pragma once

#include "graphics.h"

class Camera {

public:

	//constructor

	Camera() noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

private:


	DirectX::XMFLOAT3 position;
	//orientation of the camera
	float pitch = 0.0f;
	float yaw = 0.0f;

	static constexpr float velocity = 12.0f;
	static constexpr float rotationSpeed = 0.004f;

};