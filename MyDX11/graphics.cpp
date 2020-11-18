#include "graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>


namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

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
#define GFX_THROW_INFO_ONLY(call) (call)
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
	sd.OutputWindow = (HWND)hWnd;
	
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

void Graphics::DrawTestTriangle(float angle) {

	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex {
		
		//position
		struct {
			float x;
			float y;
		}pos;

		//color
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		}color;

	};

	//Create vertex buffer (1 2d triangle at center of screen)
	Vertex vertices[]{
		

		//Hexagon
		{ 0.0f,0.5f,255,0,0,0},
		{ 0.5f,-0.5f,0,255,0,0 },
		{ -0.5f,-0.5f,0,0,255,0 },
		{ -0.3f,0.3f,0,0,255,0 },
		{ 0.3f,0.3f,0,0,255,0 },
		{ 0.0f,-1.0f,255,0,0,0},

	};
	vertices[0].color.g = 255;
	 

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};

	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	//Create Vertex Buffer
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	//Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset);

	//create index buffer
	const unsigned short indices[] = {
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;

	//Create index buffer
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	//bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);


	//create constant buffer
	struct ConstantBuffer {
		
		struct {

			float element[4][4];

		}transformation;

	};

	const ConstantBuffer cb = {

		//rotation matrix around Z
		{
		(3.0f/4.0f)*std::cos(angle),std::sin(angle),0.0f,0.0f,
		(3.0f / 4.0f) * -std::sin(angle),std::cos(angle),0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
		}
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;

	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	//bind constant buffer
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	wrl::ComPtr<ID3DBlob> pBlob;
	//Create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	//Bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	//Create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	//Bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	//Input (vertex) layout (2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;

	//Describtor for layout format
	const D3D11_INPUT_ELEMENT_DESC ied[] = {

		//Sematic name has to match up the input of the vertex shader
		{"Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, 
		(UINT)std::size(ied), 
		pBlob->GetBufferPointer(), 
		pBlob->GetBufferSize(), 
		&pInputLayout));


	//Bind input layout
	pContext->IASetInputLayout(pInputLayout.Get());

	//Bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	//Set primitive topology to triangle list (groups of 3 vertices)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 1024;
	vp.Height = 768;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	//Draw Triangle
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u,0u));

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

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs)
	:
	Exception(line,file)
{
	//Join all info messages with newlines into single string

	for (const auto& m : infoMsgs) {

		info += m;
		info.push_back('\n');
	}

	//Remove final newline if exists
	if (!info.empty()) {

		info.pop_back();
	}


}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n@[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "SupaHotFire Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}


