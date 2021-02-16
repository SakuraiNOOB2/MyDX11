#pragma once

#include <vector>
#include <type_traits>
#include "graphics.h"

namespace MyDynamicVertex {

	struct RGBAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	class VertexLayout
	{

	public:

		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			RGBAColor,
			Count,
		};

		template<ElementType> struct Map;
		template<> struct Map<Position2D>{

			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
		};

		template<> struct Map<Position3D>{

			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};

		template<> struct Map<Texture2D>{

			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
		};

		template<> struct Map<Normal>{

			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
		};

		template<> struct Map<Float3Color>{

			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
		};

		template<> struct Map<Float4Color>{

			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
		};

		template<> struct Map<RGBAColor>{

			using SysType = MyDynamicVertex::RGBAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
		};

		class Element{

		public:
			Element(ElementType type, size_t offset)
				:
				m_type(type),
				m_offset(offset)
			{}

			size_t GetOffsetAfter() const noexcept(!IS_DEBUG){
				
				return m_offset + Size();
			}

			size_t GetOffset() const{

				return m_offset;
			}

			size_t Size() const noexcept(!IS_DEBUG){

				return SizeOf(m_type);
			}

			static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
			{

				switch (type)
				{
				case Position2D:

					return sizeof(Map<Position2D>::SysType);

				case Position3D:

					return sizeof(Map<Position3D>::SysType);

				case Texture2D:

					return sizeof(Map<Texture2D>::SysType);

				case Normal:

					return sizeof(Map<Normal>::SysType);

				case Float3Color:

					return sizeof(Map<Float3Color>::SysType);

				case Float4Color:

					return sizeof(Map<Float4Color>::SysType);

				case RGBAColor:

					return sizeof(Map<RGBAColor>::SysType);
				}

				assert("Invalid element type" && false);
				return 0u;
			}

			ElementType GetType() const noexcept{
				return m_type;
			}

			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
			{

				switch (m_type)
				{

				case Position2D:

					return GenerateDesc<Position2D>(GetOffset());
				
				case Position3D:
					
					return GenerateDesc<Position3D>(GetOffset());
				
				case Texture2D:
					
					return GenerateDesc<Texture2D>(GetOffset());
				case Normal:
					
					return GenerateDesc<Normal>(GetOffset());
				
				case Float3Color:
					
					return GenerateDesc<Float3Color>(GetOffset());
				
				case Float4Color:
					
					return GenerateDesc<Float4Color>(GetOffset());
				
				case RGBAColor:
					
					return GenerateDesc<RGBAColor>(GetOffset());
				}

				assert("Invalid element type" && false);

				return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept(!IS_DEBUG)
			{
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}


		private:

			ElementType m_type;
			size_t m_offset;
		};

	public:

		template<ElementType Type>
		const Element& Resolve() const noexcept(!IS_DEBUG){

			for (auto& e : m_elements){

				if (e.GetType() == Type){

					return e;
				}
			}

			assert("Could not resolve element type" && false);

			return m_elements.front();
		}
		
		const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG){
			return m_elements[i];
		}

		VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG){
			m_elements.emplace_back(type, Size());
			return *this;
		}

		size_t Size() const noexcept(!IS_DEBUG){
			return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter();
		}

		size_t GetElementCount() const noexcept{
			return m_elements.size();
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(GetElementCount());
			for (const auto& e : m_elements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}

	private:


		std::vector<Element> m_elements;
	};

	class Vertex
	{

		friend class VertexBuffer;

	public:

		template<VertexLayout::ElementType Type>
		auto& Attr() noexcept(!IS_DEBUG)
		{
			auto pAttribute = m_pData + m_layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}

		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val) noexcept(!IS_DEBUG)
		{

			const auto& element = m_layout.ResolveByIndex(i);
			auto pAttribute = m_pData + element.GetOffset();

			switch (element.GetType())
			{
			case VertexLayout::Position2D:

				SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Position3D:

				SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Texture2D:

				SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Normal:

				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Float3Color:

				SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Float4Color:

				SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::RGBAColor:
				
				SetAttribute<VertexLayout::RGBAColor>(pAttribute, std::forward<T>(val));
				break;

			default:
				assert("Bad element type" && false);
			}
		}
	protected:
		Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
			:
			m_pData(pData),
			m_layout(layout){

			assert(pData != nullptr);
		}

	private:

		template<typename First, typename ...Rest>

		// enables parameter pack setting of multiple parameters by element index
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG){

			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}

		// helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val) noexcept(!IS_DEBUG){

			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;

			if constexpr (std::is_assignable<Dest, SrcType>::value){

				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else{

				assert("Parameter attribute type mismatch" && false);
			}
		}

	private:

		char* m_pData = nullptr;
		const VertexLayout& m_layout;

	};

	class ConstVertex
	{
	public:


		ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
			:
			m_vertex(v)
		{}


		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept(!IS_DEBUG)
		{
			return const_cast<Vertex&>(m_vertex).Attr<Type>();
		}

	private:
		Vertex m_vertex;
	};

	class VertexBuffer
	{

	public:

		VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
			:
			m_layout(std::move(layout))
		{}

		const char* GetData() const noexcept(!IS_DEBUG){

			return m_buffer.data();
		}

		const VertexLayout& GetLayout() const noexcept{

			return m_layout;
		}

		size_t Size() const noexcept(!IS_DEBUG)
		{
			return m_buffer.size() / m_layout.Size();
		}
		size_t SizeBytes() const noexcept(!IS_DEBUG)
		{
			return m_buffer.size();
		}
		template<typename ...Params>

		void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG){

			assert(sizeof...(params) == m_layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			m_buffer.resize(m_buffer.size() + m_layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		Vertex Back() noexcept(!IS_DEBUG){

			assert(m_buffer.size() != 0u);
			return Vertex{ m_buffer.data() + m_buffer.size() - m_layout.Size(),m_layout };
		}

		Vertex Front() noexcept(!IS_DEBUG){

			assert(m_buffer.size() != 0u);
			return Vertex{ m_buffer.data(),m_layout };
		}

		Vertex operator[](size_t i) noexcept(!IS_DEBUG){

			assert(i < Size());
			return Vertex{ m_buffer.data() + m_layout.Size() * i,m_layout };
		}

		ConstVertex Back() const noexcept(!IS_DEBUG){

			return const_cast<VertexBuffer*>(this)->Back();
		}

		ConstVertex Front() const noexcept(!IS_DEBUG){

			return const_cast<VertexBuffer*>(this)->Front();
		}

		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG){

			return const_cast<VertexBuffer&>(*this)[i];
		}

	private:

		std::vector<char> m_buffer;
		VertexLayout m_layout;

	};

}