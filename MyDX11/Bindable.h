#pragma once
#include "graphics.h"

namespace Bind {

	class Bindable {

	public:

		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual ~Bindable() = default;

	protected:

		//functions for crack open specific part
		//only avaliable to the children of Bindable class
		static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(Graphics& gfx);

	};

}