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

		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 materialColor;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;

		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};

private:

	//Constant Buffer data for lighting
	PointLightCBuf cbData;
	
	//
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};