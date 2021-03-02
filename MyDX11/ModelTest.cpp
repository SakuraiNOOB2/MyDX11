#include "ModelTest.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Vertex.h"

using namespace Bind;

ModelTest::ModelTest(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	DirectX::XMFLOAT3 material, float scale)
	:
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{

	
	using MyDynamicVertex::VertexLayout;
	MyDynamicVertex::VertexBuffer vbuf(
		std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		)
	);


	Assimp::Importer imp;
	const auto pModel = imp.ReadFile("asset\\model\\suzanne.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);
	const auto pMesh = pModel->mMeshes[0];

	

	for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			DirectX::XMFLOAT3{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
			*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
			);
	}

	std::vector<unsigned short> indices;
	indices.reserve(pMesh->mNumFaces * 3);
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		const auto& face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	AddBind(std::make_shared<VertexBuffer>(gfx, vbuf));

	AddBind(std::make_shared<IndexBuffer>(gfx, indices));

	auto pvs = std::make_shared<VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"PhongPS.cso"));

	AddBind(std::make_shared<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	pmc.color = material;
	AddBind(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}
