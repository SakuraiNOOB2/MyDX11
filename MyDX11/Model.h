#pragma once

#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"

//assimp loading stuffs
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#pragma comment (lib,"assimp.lib")


using namespace Bind;

/// <summary>
/// Mesh class
/// </summary>
class Mesh : public DrawableBase<Mesh>
{

public:

	//constructor
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs){

		if (!IsStaticInitialized()){

			//assume all mesh are in trianglelist
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}

		//checking indexbuffer around bindables 
		for (auto& pb : bindPtrs){

			//updating indexbuffer in the memory
			if (auto pi = dynamic_cast<IndexBuffer*>(pb.get())){
				
				//constructing new index buffer pointer
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
				
				//release memory from origin indexbuffer pointer
				pb.release();
			}
			else{

				AddBind(std::move(pb));
			}
		}

		AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	}

	
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG){
		
		//storing transform into m_transform
		DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
		
		Drawable::Draw(gfx);
	}

	//getting mesh's transform
	DirectX::XMMATRIX GetTransformXM() const noexcept override{
		
		return DirectX::XMLoadFloat4x4(&m_transform);
	}

private:

	mutable DirectX::XMFLOAT4X4 m_transform;	//mesh's transform
};


/// <summary>
/// Node class
/// </summary>
class Node{

	//used to allowing only model to add nodes
	friend class Model;

public:

	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& m_transform) noexcept(!IS_DEBUG)
		:
		meshPtrs(std::move(meshPtrs)){

		DirectX::XMStoreFloat4x4(&this->m_transform, m_transform);
	}

	
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG){

		//attaching parent's transform to own node's transform
		const auto built = DirectX::XMLoadFloat4x4(&m_transform) * accumulatedTransform;
		
		//applying transform to all mesh connected to this node 
		for (const auto pm : meshPtrs){

			pm->Draw(gfx, built);
		}

		//drawing children
		for (const auto& pc : childPtrs){

			pc->Draw(gfx, built);
		}
	}

private:

	//add children to the node
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG){
		
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}

private:

	std::vector<std::unique_ptr<Node>> childPtrs;	//children pointers
	std::vector<Mesh*> meshPtrs;		//mesh pointer
	DirectX::XMFLOAT4X4 m_transform;	//node's transform
};


/// <summary>
/// Model class
/// </summary>
class Model
{

public:

	Model(Graphics& gfx, const std::string fileName){

		//creating importer
		Assimp::Importer imp;

		//reading model file into pScene
		const auto pScene = imp.ReadFile(fileName.c_str(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);

		//load all meshes from pScene
		for (size_t i = 0; i < pScene->mNumMeshes; i++){

			//adding binded mesh into mesh pointer
			m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
		}

		//updating root node
		m_pRoot = ParseNode(*pScene->mRootNode);
	}

	void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const
	{
		m_pRoot->Draw(gfx, transform);
	}

private:

	//binding every mesh
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh){


		using MyDynamicVertex::VertexLayout;

		//creating vertex buffer
		MyDynamicVertex::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));


		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}


		//creating index buffer
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		std::vector<std::unique_ptr<Bindable>> bindablePtrs;

		//binding vertex buffer
		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

		//binding index buffer
		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		//create and bind vertex shader
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		//binding pixel shader
		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		//binding input layout
		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

		//creating material constant for pixel shader
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;

		//binding material constant
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

		//return a unique_ptr to mesh
		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}


	std::unique_ptr<Node> ParseNode(const aiNode& node){
		
		const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
		));

		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);

		for (size_t i = 0; i < node.mNumMeshes; i++){

			const auto meshIdx = node.mMeshes[i];
			curMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
		}

		auto pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
		for (size_t i = 0; i < node.mNumChildren; i++){

			pNode->AddChild(ParseNode(*node.mChildren[i]));
		}

		return pNode;
	}
	
private:

	std::unique_ptr<Node> m_pRoot;
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs;

};