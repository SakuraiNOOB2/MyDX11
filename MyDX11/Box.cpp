#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
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

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		//Bind static Vertex Buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		//Bind static Vertex Shader
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		//Bind static Pixel Shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		//Bind static Index Buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		//Create static Input Layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {

			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
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

	struct PSMaterialConstant {

		alignas(16) DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;

		float padding[2];
	}colorConst;

	colorConst.color = material;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));

	//model deformation transform(per instance,not stored as bind)
	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

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
		return	DirectX::XMLoadFloat3x3(&mt)*
				DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
				DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
				DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
