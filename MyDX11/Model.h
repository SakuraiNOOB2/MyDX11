#pragma once

#include "Drawable.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <optional>

//assimp loading stuffs
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace Bind;

class ModelException :public myException {

public:

	ModelException(int line, const char* file, std::string note) noexcept;

	const char* what() const noexcept override;
	const char* GetType() const noexcept override;

	const std::string& GetNote() const noexcept;

private:

	std::string m_note;


};


/// <summary>
/// Mesh class
/// </summary>
class Mesh : public Drawable
{

public:

	//constructor
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs);

	//draw function
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

	//getting mesh's transform
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

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

	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& m_transform) noexcept(!IS_DEBUG);

	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	
	int GetNodeID() const noexcept;
	void ShowNodeTree(Node*& pSelectedNode) const noexcept;

private:

	//add children to the node
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);


private:

	std::string m_name;
	int m_nodeID;


	std::vector<std::unique_ptr<Node>> childPtrs;	//children pointers
	std::vector<Mesh*> meshPtrs;		//mesh pointer
	DirectX::XMFLOAT4X4 baseTransform;	
	DirectX::XMFLOAT4X4 appliedTransform;
};

/// <summary>
/// Model window class
/// </summary>
class ModelWindow {

public:

	void Show(const char* windowName, const Node& root) noexcept;

	DirectX::XMMATRIX GetTransform() const noexcept;

	Node* GetSelectedNode() const noexcept;

private:

	Node* m_pSelectedNode;

	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	std::unordered_map<int, TransformParameters> transforms;

};


/// <summary>
/// Model class
/// </summary>
class Model
{

public:

	//constructor
	Model(Graphics& gfx, const std::string fileName);

	//draw
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);

	//showing imgui window
	void ShowWindow(const char* windowName = nullptr) noexcept;

	//destructor
	~Model() noexcept;

private:

	//binding every mesh
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);


	std::unique_ptr<Node> ParseNode(int& nextID,const aiNode& node) noexcept;
	
private:

	std::unique_ptr<Node> m_pRoot;
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs;

	//model window
	std::unique_ptr<class ModelWindow> m_pWindow;

};