#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "imgui/imgui.h"



Box::Box(Graphics& gfx,
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	TestObject(
		gfx,
		rng,
		adist,
		ddist,
		odist,
		rdist)
{
	using namespace Bind;

	//Initialization for the buffers

	//Create Vertex
	struct Vertex {

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
	};

	auto model = Cube::MakeIndependent<Vertex>();
	model.SetNormalsIndependentFlat();

	//Bind Vertex Buffer
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));

	//Bind Vertex Shader
	auto pvs = std::make_shared<VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	//Bind Pixel Shader
	AddBind(std::make_shared<PixelShader>(gfx, L"PhongPS.cso"));

	//Bind Index Buffer
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	//Create Input Layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {

		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//Bind Input Layout to the pipeline
	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	//Bind Topology
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));




	//Bind Transform Constant Buffer pipeline
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	//PS material constants
	materialConstants.color = material;
	AddBind(std::make_shared<MaterialCbuf>(gfx, materialConstants, 1u));

	//model deformation transform(per instance,not stored as bind)
	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

}



DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return	DirectX::XMLoadFloat3x3(&mt) *
		TestObject::GetTransformXM();
}

bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
{

	bool isDirty = false;
	bool isOpen = true;

	//control window for ps material constants
	if (ImGui::Begin(("Box" + std::to_string(id)).c_str(),&isOpen)) {

		ImGui::Text("Material Properties");
		//color
		const auto cd = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);

		//lighting stuffs
		const auto sid= ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity,0.05f,4.0f,"%.2f",2);
		const auto spd=ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", 2);

		isDirty = cd || sid || spd;

		//Transform stuffs
		ImGui::Text("Position");

		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

	}

	ImGui::End();

	if (isDirty) {

		SyncMaterial(gfx);

	}

	return isOpen;
}

void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
{

	//finding materialcbuff
	auto pConstantPS = QueryBindable<MaterialCbuf>();

	//checking pConstantPS is null pointer or not
	assert(pConstantPS != nullptr);

	pConstantPS->Update(gfx, materialConstants);
}
