#include "Model.h"
#include "imgui/imgui.h"
#include <unordered_map>

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

Node::Node(const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& m_transform) noexcept(!IS_DEBUG)
	:
	meshPtrs(std::move(meshPtrs)),
	m_name(name)
{
	DirectX::XMStoreFloat4x4(&baseTransform, m_transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {

	//attaching parent's transform to own node's transform
	const auto built =
		DirectX::XMLoadFloat4x4(&baseTransform) *
		DirectX::XMLoadFloat4x4(&appliedTransform) *
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

void Node::ShowNodeTree(int& nodeIndexTracked,
	std::optional<int>& selectedIndex, 
	Node*& pSelectedNode) const noexcept
{

	// nodeIndex serves as the uid for gui tree nodes, incremented throughout recursion
	const int currentNodeIndex = nodeIndexTracked;
	nodeIndexTracked++;

	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

	// if tree node expanded, recursively render all children
	if (ImGui::TreeNodeEx(
		(void*)(intptr_t)currentNodeIndex, 
		node_flags, m_name.c_str())
		){

		// detecting / setting selected node
		if (ImGui::IsItemClicked())
		{
			selectedIndex = currentNodeIndex;
			pSelectedNode = const_cast<Node*>(this);
		}

		for (const auto& pChild : childPtrs){

			pChild->ShowNodeTree(nodeIndexTracked, selectedIndex, pSelectedNode);
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
		aiProcess_JoinIdenticalVertices
	);

	//load all meshes from pScene
	for (size_t i = 0; i < pScene->mNumMeshes; i++) {

		//adding binded mesh into mesh pointer
		m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}

	//updating root node
	m_pRoot = ParseNode(*pScene->mRootNode);
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
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh) {


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




std::unique_ptr<Node> Model::ParseNode(const aiNode& node) noexcept {

	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);

	for (size_t i = 0; i < node.mNumMeshes; i++) {

		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(node.mName.C_Str(),std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++) {

		pNode->AddChild(ParseNode(*node.mChildren[i]));
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
		root.ShowNodeTree(nodeIndexTracker, m_selectedIndex,m_pSelectedNode);

		ImGui::NextColumn();
		if (m_pSelectedNode != nullptr) {

			auto& transform = transforms[*m_selectedIndex];

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
	const auto& transform = transforms.at(*m_selectedIndex);

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
