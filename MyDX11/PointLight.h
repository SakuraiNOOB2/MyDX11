#pragma once

#include "graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

class PointLight {

public:

	//constructor
	PointLight(Graphics& gfx, float radius = 0.5f);

	//member functions
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx) const noexcept;

private:

	//constant buffer for point light
	struct PointLightCBuf {

		DirectX::XMFLOAT3 pos;
		float padding;
	};

private:

	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };

	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};