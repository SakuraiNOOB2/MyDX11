#pragma once

#include "TestObjects.h"
#include "ConstantBuffers.h"

using namespace Bind;

//Drawable Base templated on Box
class Box :public TestObject {

public:

	//contructor
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material);

	//getter for getting the box's transform
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	//for imgui control windows
	bool SpawnControlWindow(int id, Graphics& gfx) noexcept;

private:

	//for syncing materials data
	void SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG);

private:

	//PS material constant stuffs

	struct PSMaterialConstant {

		DirectX::XMFLOAT3 color;

		//Lighting stuffs
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	}materialConstants;

	using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;

private:

	//model transform
	DirectX::XMFLOAT3X3 mt;

};
