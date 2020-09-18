#include "graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib")

//check the return value of the hrcall is a failure code
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr=(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__,__FILE__,(hr))

Graphics::Graphics(HWND hWnd) {

	//required structure configuration information
	
	DXGI_SWAP_CHAIN_DESC sd = {};

	//
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;

	//layout of the pixels the channels in 
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	
	//
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;

	//
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//sampling mode (anti-aliasing)
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	//buffer output window and window mode
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = (HWND)696969/*hWnd*/;
	
	//window mode 
	sd.Windowed = TRUE;

	//the effect used for flipping and presentation
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	//additional flags
	sd.Flags = 0;

	//for checking results of d3d functions
	HRESULT hr;

	//create device and front/back buffers, and swap chain and rendering context

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,		//pointer to a descriptor structure
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	//gain access to teture subresource in swap chain (back buffer)
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer)));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	));

	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget != nullptr) {

		pTarget->Release();
	}

	if (pContext != nullptr) {

		pContext->Release();
	}

	if (pSwap != nullptr) {

		pSwap->Release();
	}

	if (pDevice != nullptr) {

		pDevice->Release();
	}


}

void Graphics::EndFrame()
{

	HRESULT hr;

	//Present function can give you an error code that is DEVICE_REMOVED
	if (FAILED(hr = pSwap->Present(1u, 0u))) {

		//DEVICE_REMOVED is a special error code that contains additional information of the error
		
		//DEVICE_REMOVED error code handling
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {

			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else {

			GFX_THROW_FAILED(hr);
		}
	
	}

}

//Graphics exception stuff

Graphics::HrException::HrException(int line,const char* file,HRESULT hr) noexcept
	:
	Exception(line,file),
	hr(hr)
{}

const char* Graphics::HrException::what() const noexcept {

	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept {

	return "SupaHotFire Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept {

	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept {

	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept {

	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;

}

const char* Graphics::DeviceRemovedException::GetType() const noexcept {

	return "SupaHotFire Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}