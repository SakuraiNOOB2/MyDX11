#include "Pyramid.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cone.h"
#include <array>

Pyramid::Pyramid(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_int_distribution<int>& tdist)
	:TestObject(gfx,rng,adist,ddist,odist,rdist)
{

	using namespace Bind;


	auto pvs = std::make_unique<VertexShader>(gfx, L"BlendedPhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));
	
	AddBind(std::make_shared<PixelShader>(gfx, L"BlendedPhongPS.cso"));

	
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Normal",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},

	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSMaterialConstant {

		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	}colorConst;

	AddBind(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));



	struct Vertex {

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		std::array<char, 4> color;
		char padding;
	};

	const auto tesselation = tdist(rng);
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);

	//set vertex colors for mesh
	for (auto& v : model.vertices) {

		v.color = { (char)10,(char)10,(char)255 };
	}

	for (int i = 0; i < tesselation; i++) {

		model.vertices[i * 3].color = { (char)255,(char)10,(char)10 };//very first vertex is the cone tip
	}

	//squash mesh a bit in the z direction
	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	//add normals
	model.SetNormalsIndependentFlat();

	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));

	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));


	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}
