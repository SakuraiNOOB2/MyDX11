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


	//check this class is statically initialized or not 
	if (!IsStaticInitialized()) {

		//**Static Initialization for the buffers

		//Create Vertex
		struct Vertex {

			struct {

				float x;
				float y;
				float z;
			}pos;

		};

		const std::vector<Vertex> vertices = {

			{ -1.0f,-1.0f,-1.0f },
			{ 1.0f,-1.0f,-1.0f },
			{ -1.0f,1.0f,-1.0f },
			{ 1.0f,1.0f,-1.0f },
			{ -1.0f,-1.0f,1.0f },
			{ 1.0f,-1.0f,1.0f },
			{ -1.0f,1.0f,1.0f },
			{ 1.0f,1.0f,1.0f },

		};

		//Bind static Vertex Buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		//Bind static Vertex Shader
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		//Bind static Pixel Shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		//Create static indices
		const std::vector<unsigned short> indices = {

			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		//Bind static Index Buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

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
				{ 1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,1.0f,1.0f },
			}
		};

		//Bind static Constant Buffer for face color
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		//Create static Input Layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {

			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		//Bind static Input Layout to the pipeline
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		//Bind static Topology
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else {

		//Set/Update indexBuffer pointer/reference to from static
		SetIndexFromStatic();
	}


	//Bind not static bindable Transform Constant Buffer pipeline
	//*** Not static bindables because every boxes has its own transform
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
		return	DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
				DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
				DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
				DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
	}
