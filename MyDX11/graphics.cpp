#include "graphics.h"
#include "dxerr.h"
#include <sstream>


namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")

//grahpics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif

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

	UINT swapCreateFlags = 0u;

#ifndef NDEBUG
	swapCreateFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	//for checking results of d3d functions
	HRESULT hr;

	//create device and front/back buffers, and swap chain and rendering context

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
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
	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));


}

void Graphics::EndFrame()
{

	HRESULT hr;

#ifndef NDEBUG

	infoManager.Set();
#endif // !NDEBUG


	//Present function can give you an error code that is DEVICE_REMOVED
	if (FAILED(hr = pSwap->Present(1u, 0u))) {

		//DEVICE_REMOVED is a special error code that contains additional information of the error
		
		//DEVICE_REMOVED error code handling
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {

			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else {

			throw GFX_EXCEPT(hr);
		}
	
	}

}

//Graphics exception stuff

Graphics::HrException::HrException(int line,const char* file,HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line,file),
	hr(hr)
{
	//join all info messages with newlines into single string
	for (const auto& m: infoMsgs) {

		info += m;
		info.push_back('\n');
	}
	//remove final newline if exists
	if (!info.empty()) {

		info.pop_back();
	}

}

const char* Graphics::HrException::what() const noexcept {

	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
		
	if (!info.empty()) {

		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}

	oss << GetOriginString();
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

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept {

	return "SupaHotFire Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}