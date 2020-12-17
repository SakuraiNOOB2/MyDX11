#pragma once

#include "graphics.h"

class Camera {

public:

	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

private:

	//distance of the camera from origin
	float r = 20.0f;

	//rotation around the equater
	float theta = 0.0f;
	
	//rotation around the towards the north pole
	float phi = 0.0f;

	//orientation of the camera
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

};