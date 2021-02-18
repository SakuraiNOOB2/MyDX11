#include "Model.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>

/// <summary>
/// Model Error Handeling
/// </summary>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	myException(line,file),
	m_note(std::move(note))
{
}


const char* ModelException::what() const noexcept
{
	std::ostringstream oss;

	oss << myException::what() << std::endl
		<< "[Note] " << GetNote();

	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "SupaHotFire Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	// TODO: insert return statement here
	return m_note;
}


/// <summary>
/// Mesh Handeling
/// </summary>

//constructor
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs) {

	if (!IsStaticInitialized()) {

		//assume all mesh are in trianglelist
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	//checking indexbuffer around bindables 
	for (auto& pb : bindPtrs) {

		//updating indexbuffer in the memory
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get())) {

			//constructing new index buffer pointer
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });

			//release memory from origin indexbuffer pointer
			pb.release();
		}
		else {

			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {

	//storing transform into m_transform
	DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);

	Drawable::Draw(gfx);
}

//getting mesh's transform

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {

	return DirectX::XMLoadFloat4x4(&m_transform);
}

Node::Node(int id,const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept(!IS_DEBUG)
	:
	m_nodeID(id),
	meshPtrs(std::move(meshPtrs)),
	m_name(name)
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform_in);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {

	//attaching parent's transform to own node's transform
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&baseTransform) *
		accumulatedTransform;

	//applying transform to all mesh connected to this node 
	for (const auto pm : meshPtrs) {

		pm->Draw(gfx, built);
	}

	//drawing children
	for (const auto& pc : childPtrs) {

		pc->Draw(gfx, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{

	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetNodeID() const noexcept
{
	return m_nodeID;
}

void Node::ShowNodeTree(Node*& pSelectedNode) const noexcept
{

	//if there is no selected node, set selectedID to an impossible value
	const int selectedID = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetNodeID();


	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetNodeID() == selectedID) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

	//render this node

	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetNodeID(),
		node_flags, m_name.c_str());
	
	// processing for selecting node
	if (ImGui::IsItemClicked()){

		pSelectedNode = const_cast<Node*>(this);
	}

	// recursive rendering of open node's children
	if (expanded){

		for (const auto& pChild : childPtrs){

			pChild->ShowNodeTree(pSelectedNode);
		}

		ImGui::TreePop();
	}

}

//add children to the node
void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG) {

	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

Model::Model(Graphics& gfx, const std::string fileName)
	:
	m_pWindow(std::make_unique<ModelWindow>())
{

	//creating importer
	Assimp::Importer imp;

	//reading model file into pScene
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr) {

		throw ModelException(__LINE__,
			__FILE__, 
			imp.GetErrorString());
	}
	

	//load all meshes from pScene
	for (size_t i = 0; i < pScene->mNumMeshes; i++) {

		//adding binded mesh into mesh pointer
		m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i],pScene->mMaterials));
	}

	//updating root node
	int nextId = 0;
	m_pRoot = ParseNode(nextId,*pScene->mRootNode);
}

void Model::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	if (auto node = m_pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(m_pWindow->GetTransform());
	}
	m_pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{

	m_pWindow->Show(windowName, *m_pRoot);
}

Model::~Model() noexcept
{
}

//binding every mesh
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials) {


	using MyDynamicVertex::VertexLayout;

	//creating vertex buffer
	MyDynamicVertex::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));


	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
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

	bool hasSpecularMap = false;
	float shininess = 35.0f;

	//binding texture
	if (mesh.mMaterialIndex >= 0) {

		
		auto& material = *pMaterials[mesh.mMaterialIndex];

		using namespace std::string_literals;

		const auto base = "asset\\model\\nano_textured\\"s;

		aiString texFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str())));
		
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS) {
			
			bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1));
			
			hasSpecularMap = true;
		}
		else {

			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));
	}


	//binding vertex buffer
	bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

	//binding index buffer
	bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	//create and bind vertex shader
	auto pvs = std::make_unique<VertexShader>(gfx, L"ModelPhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	bindablePtrs.push_back(std::move(pvs));

	//binding input layout
	bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	//binding pixel shader
	if (hasSpecularMap) {

		bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"ModelPhongPSSpecMap.cso"));

	}
	else {

		bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"ModelPhongPS.cso"));

		//creating material constant for pixel shader
		struct PSMaterialConstant
		{

			float specularIntensity = 0.8f;
			float specularPower;
			float padding[2];
		} pmc;

		pmc.specularPower = shininess;

		//binding material constant
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}

	
	//return a unique_ptr to mesh
	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}




std::unique_ptr<Node> Model::ParseNode(int& nextID,const aiNode& node) noexcept {

	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);

	for (size_t i = 0; i < node.mNumMeshes; i++) {

		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextID++,node.mName.C_Str(),std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {

		pNode->AddChild(ParseNode(nextID,*node.mChildren[i]));
	}

	return pNode;
}

void ModelWindow::Show(const char* windowName, const Node& root) noexcept
{

	//window name defaults to Model
	windowName = windowName ? windowName : "Model";

	//ints for tracking node indices and selected node
	int nodeIndexTracker = 0;

	if (ImGui::Begin(windowName))
	{
		ImGui::Columns(2, nullptr, true);
		root.ShowNodeTree(m_pSelectedNode);

		ImGui::NextColumn();
		if (m_pSelectedNode != nullptr) {

			auto& transform = transforms[m_pSelectedNode->GetNodeID()];

			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

		}

		
	}

	ImGui::End();


}

DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept
{

	assert(m_pSelectedNode != nullptr);

	const auto& transform = transforms.at(m_pSelectedNode->GetNodeID());

	return DirectX::XMMatrixRotationRollPitchYaw(
		transform.roll, 
		transform.pitch, 
		transform.yaw) *
		DirectX::XMMatrixTranslation(
			transform.x, 
			transform.y, 
			transform.z);
}

Node* ModelWindow::GetSelectedNode() const noexcept
{
	return m_pSelectedNode;
}

