#include "SolidSphere.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"


SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;

	struct Vertex {

		DirectX::XMFLOAT3 pos;

	};

	auto model = Sphere::Make<Vertex>();
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	//Bind vertex buffer
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
	
	//Bind index buffer
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	//Bind static vertex shader
	auto pvs = std::make_shared<VertexShader>(gfx, L"SolidVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	//Bind static pixel shader
	AddBind(std::make_shared<PixelShader>(gfx, L"SolidPS.cso"));

	//Creatre constant Buffer
	struct PSColorConstant {
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	}colorConst;

	//Bind static constant buffer
	AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

	//Create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},

	};

	//Bind static input layout
	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	//Bind static topology
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));


	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

}

void SolidSphere::Update(float dt) noexcept
{

}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
	
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
