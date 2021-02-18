#include "camera.h"
#include "imgui/imgui.h"
#include "myMath.h"

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{

	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3(&position);
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera")) {

		//Showing position details
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Y", &position.y, -80.0f, 80.0f);
		ImGui::SliderAngle("Z", &position.z, -80.0f, 80.0f);
		
		//Showing orientation details
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch",&pitch, 0.995f*-90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset")) {

			Reset();
		}

	}


	ImGui::End();
}

void Camera::Reset() noexcept
{
	
	position = { 0.0f,7.5f,-18.0f };

	float pitch = 0.0f;
	float yaw = 0.0f;

}

void Camera::Rotate(float dx, float dy) noexcept
{

	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{

	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		DirectX::XMMatrixScaling(velocity, velocity, velocity)
	));

	position = {
		position.x + translation.x,
		position.y + translation.y,
		position.z + translation.z
	};

}
