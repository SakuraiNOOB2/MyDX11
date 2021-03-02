#include "SkinnedBox.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"

SkinnedBox::SkinnedBox(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist)
	:
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{

	using namespace Bind;


	struct Vertex {

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		DirectX::XMFLOAT2 tc;
	};

	auto model = Cube::MakeIndependentTextured<Vertex>();
	model.SetNormalsIndependentFlat();

	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));

	AddBind(std::make_shared<Texture>(gfx, Surface::FromFile("asset\\texture\\stonk.jpg")));

	AddBind(std::make_shared<Sampler>(gfx));

	auto pvs = std::make_shared<VertexShader>(gfx, L"TexturedPhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"TexturedPhongPS.cso"));

	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
	
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSMaterialConstant {

		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	}colorConst;

	AddBind(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));


	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}
