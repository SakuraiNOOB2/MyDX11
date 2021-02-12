#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "myMath.h"


class Prism {


public:


	template<class V>

	static IndexedTriangleList<V> MakeTesselated(int longDiv) {

		assert(longDiv >= 3);

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		//near center
		std::vector<V> vertices;
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iCenterNear = (unsigned short)(vertices.size() - 1);

		//fat center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iCenterFar = (unsigned short)(vertices.size() - 1);

		//base vertices
		for (int iLong = 0; iLong < longDiv; iLong++) {

			//near base
			{
				//construct new element for vertices
				vertices.emplace_back();

				//calculating the near base element
				auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);

				//add the near base element to the constrcued element in vertices
				DirectX::XMStoreFloat3(&vertices.back().pos, v);
			}

			//far base
			{
				
				vertices.emplace_back();
				
				auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);

				DirectX::XMStoreFloat3(&vertices.back().pos * v);

			}

		}

		//side indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {

			const auto i = iLong * 2;
			const auto mod = longDiv * 2;

			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 1 + 2);
		}

		//base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {

			const auto i = iLong * 2;
			const auto mod = longDiv * 2;

			indices.push_back(i + 2);
			indices.push_back(iCenterNear);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(iCenterFar);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 3) % mod + 2);
		}

		return { std::move(vertices),std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> MakeTesselatedIndependentCapNormals(int longDiv) {

		assert(longDiv >= 3);

		/// <summary>
		/// setting base,offset and longitude angle
		/// </summary>
		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		/// <summary>
		/// vertices part
		/// </summary>

		std::vector<V> vertices;

		//near center
		const auto iCenterNear = (unsigned short)vertices.size();

		vertices.emplace_back();

		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		vertices.back().n = { 0.0f,0.0f,-1.0f };

		//near base vertices
		const auto iBaseNear = (unsigned short)vertices.size();

		for (int iLong = 0; iLong < longDiv; iLong++) {
			
			vertices.emplace_back();

			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(
					longitudeAngle * 
					iLong)
			);

			DirectX::XMStoreFloat3(&vertices.back().pos, v);

			vertices.back().n = { 0.0f,0.0f,-1.0f };
		}

		//far center
		const auto iCenterFar = (unsigned short)vertices.size();

		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		vertices.back().n = { 0.0f,0.0f,1.0f };

		//far base vertices
		const auto iBaseFar = (unsigned short)vertices.size();

		for (int iLong = 0; iLong < longDiv; iLong++) {

			vertices.emplace_back();

			auto v = DirectX::XMVector3Transform(base,
				DirectX::XMMatrixRotationZ(
					longitudeAngle *
					iLong
				)
			);

			v = DirectX::XMVectorAdd(v, offset);

			DirectX::XMStoreFloat3(&vertices.back().pos, v);

			vertices.back().n = { 0.0f,0.0f,1.0f };

		}

		//fusilage vertices
		const auto iFusilage = (unsigned short)vertices.size();

		for (int iLong = 0; iLong < longDiv; iLong++) {

			//near base
			{
				vertices.emplace_back();

				auto v = DirectX::XMVector3Transform(base,
					DirectX::XMMatrixRotationZ(
						longitudeAngle *
						iLong
					)
				);

				DirectX::XMStoreFloat3(&vertices.back().pos, v);

				vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,0.0f };

			}

			//far base
			{
				vertices.emplace_back();

				auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ(
						longitudeAngle *
						iLong
					)
				);
				v = DirectX::XMVectorAdd(v, offset);
				DirectX::XMStoreFloat3(&vertices.back().pos, v);


				vertices.back().n = { vertices.back().pos.x,vertices.back().pos.y,0.0f };
			}

		}

		/// <summary>
		/// indices part
		/// </summary>

		std::vector<unsigned short> indices;
		//near base
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {

			const auto i = iLong;
			const auto mod = longDiv;

			//near
			indices.push_back(i + iBaseNear);
			indices.push_back(iCenterNear);
			indices.push_back((i + 1) % mod + iBaseNear);
		}

		//far base
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
			
			const auto i = iLong;
			const auto mod = longDiv;

			//far
			indices.push_back(iCenterFar);
			indices.push_back(i + iBaseFar);
			indices.push_back((i + 1) % mod + iBaseFar);
		}

		//fusilage
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
			
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;

			indices.push_back(i + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back((i + 3) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);

		}

		return { std::move(vertices),std::move(indices) };

	}

	template<class V>
	static IndexedTriangleList<V> Make() {
		
		return MakeTesselated<V>(24);
	}

};