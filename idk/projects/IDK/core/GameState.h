#pragma once
#include <core/Component.h>
#include "GameState_detail.h"

namespace idk
{
	class GameState
	{
	public:
		GameState();
		~GameState();

		template<typename T>  Handle<T> CreateObject(uint8_t scene);
		template<typename T>  Handle<T> CreateObject(const Handle<T>& handle);
		template<typename T>  bool      ValidateHandle(const Handle<T>& handle);
		template<typename T>  bool      DestroyObject(const Handle<T>& handle);
	private:
		detail::ObjectPools_t _objects;
	};
}