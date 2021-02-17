#pragma once

#include "Drawable.h"
#include "IndexBuffer.h"

//template class T for different drawable stuffs
template<class T>
class DrawableBase :public Drawable {


protected:

	//Check the static data has been initialized or not by check the staticBinds
	static bool IsStaticInitialized() noexcept {

		return !staticBinds.empty();
	}

	//Add static bindables
	static void AddStaticBind(std::unique_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG) {

		//Check binds' typeid is IndexBuffer or not
		assert("**Must** use ONLY AddStaticIndexBuffer to bind index buffer!" && typeid(*bind) != typeid(Bind::IndexBuffer));

		staticBinds.push_back(std::move(bind));

	}

	//Add static index buffers
	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noexcept(!IS_DEBUG) {

		//
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);

		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));

	}

	//Setting indexBuffer pointers for multiple drawables
	void SetIndexFromStatic() noexcept(!IS_DEBUG) {

		//Sanity check for checking there's something wrong on pIndexBuffer
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);

		//Looping through all the staticBinds
		for (const auto& b : staticBinds) {

			//dynamic_cast to find the indexBuffer
			if (const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get())) {

				//setting indexBuffer pointer
				pIndexBuffer = p;

				return;
			}

		}

		//Sanity check for checking there's something wrong on pIndexBuffer
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private:

	const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept override {

		return staticBinds;
	}

private:

	static std::vector<std::unique_ptr<Bind::Bindable>> staticBinds;

};

//declare outsite the class for static parameter
template<class T>
std::vector<std::unique_ptr<Bind::Bindable>> DrawableBase<T>::staticBinds;

