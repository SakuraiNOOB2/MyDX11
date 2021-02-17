#pragma once

#include "myWin.h"
#include "myException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "dxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

namespace Bind
{
	class Bindable;
}

class Graphics {

	friend class Bind::Bindable;

public:

	//Graphics exception
	//error handling

	//basic exception
	class Exception :public myException {

		using myException::myException;
	};

	//exception class with HRESULT
	class HrException :public Exception {

	public:

		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs={}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException :public Exception {

	public:
		
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs = {});
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;

	private:
		std::string info;
		
	};


	//specialized exception
	class DeviceRemovedException :public HrException {

		using HrException::HrException;

	public:
		const char* GetType() const noexcept override;

	private:
		std::string reason;
	};


public:

	Graphics(HWND hWnd,int width,int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;

	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	//camera stuffs
	void SetCamera(DirectX::FXMMATRIX view) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	//imgui stuffs
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

private:
	bool imguiEnabled = true;

private:

	//camera stuffs
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;

#ifndef  NDEBUG
	DxgiInfoManager infoManager;
#endif // ! NDEBUG


	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};
