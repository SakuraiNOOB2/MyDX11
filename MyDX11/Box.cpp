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

	//PS material constants
	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u));

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
