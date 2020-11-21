#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"


//Base ConstantBuffer
//*template for more flexibility
template<typename C>		
class ConstantBuffer :public Bindable {

public:

	//for every frame update
	void Update(Graphics& gfx, const C& consts) {

		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));

		//copy data from subresources
		memcpy(msr.pData, &consts, sizeof(consts));
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		
	}

	//constructor with initializing constant buffer data
	ConstantBuffer(Graphics& gfx, const C& consts) {

		//throw dxgiinfomanager to the scope
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	}

	//constructor without initializing constant buffer data
	ConstantBuffer(Graphics& gfx) {

		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));

	}

protected:

	//ComPtr for ConstantBuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

//VertexConstantBuffer
//*Inheriting with templates
template<typename C>
class VertexConstantBuffer :public ConstantBuffer<C> {

private:

	//using declaration to import the names of ConstantBuffer and Bindable class
	//For access the protected member of the ConstantBuffer and Bindable class
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;

	//bind the vertex constant buffer to vertex shader
	void Bind(Graphics& gfx) noexcept override {

		//"this" pointer can be used if not using "using" declaration
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}

};

//PixelConstantBuffer
//*Inheriting with templates
template<typename C>
class PixelConstantBuffer :public ConstantBuffer<C> {

private:

	//using declaration to import the names of ConstantBuffer and Bindable class
	//For access the protected member of the ConstantBuffer and Bindable class
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;

	//bind the pixel constant buffer to pixel shader
	void Bind(Graphics& gfx) noexcept override {

		//"this" pointer can be used if not using "using" declaration
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}

};