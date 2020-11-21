#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng, 
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
{
	//Create Vertex
	struct Vertex {

		struct {

			float x;
			float y;
			float z;
		}pos;

	};

	const std::vector<Vertex> vertices = {

		{-1.0f,-1.0f,-1.0f},
		{1.0f,-1.0f,-1.0f},
		{-1.0f,1.0f,-1.0f},
		{1.0f,1.0f,-1.0f},
		{-1.0f,-1.0f,1.0f},
		{1.0f,-1.0f,1.0f},
		{-1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f}

	};

	//Bind Vertex Buffer
	AddBind(std::make_unique<VertexBuffer>(gfx, vertices));

	//Bind Vertex Shader
	auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	//Bind Pixel Shader
	AddBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

	//Create indices
	const std::vector<unsigned short> indices = {

		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	//Bind Index Buffer
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

	//Create Constant Buffer for face colors
	struct ConstantBuffer2 {

		struct {

			float r;
			float g;
			float b;
			float a;
		}face_colors[6];

	};

	const ConstantBuffer2 cb2 = {

		{
			{1.0f,0.0f,1.0f},
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
			{1.0f,1.0f,0.0f},
			{0.0f,1.0f,1.0f}
		}
	};

	//Bind Constant Buffer
	AddBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

	//Create Input Layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {

		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	//Bind Input Layout to the pipeline
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	//Bind Topology
	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	//Bind Transform Constant Buffer pipeline
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

	void Box::Update(float dt) noexcept
	{
		//get delta time(dt) to update the angles
		roll += droll * dt;
		pitch += dpitch * dt;
		yaw += dyaw * dt;

		theta += dtheta * dt;
		phi += dphi * dt;
		chi += dchi * dt;

	}

	DirectX::XMMATRIX Box::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
	}
