#pragma once
#include <core/Component.h>
#include <core/Scene.h>
#include "GameState_detail.h"

namespace idk
{
	class Scene;

	class GameState
	{
	public:
		GameState();
		~GameState();

		opt<Scene> ActivateScene(uint8_t scene);
		bool       DectivateScene(uint8_t scene);
		bool       DectivateScene(Scene scene);

		template<typename T>  span<T>   GetObjectsOfType();

		template<typename T>  T*        GetObject(const Handle<T>& handle);
		template<typename T>  Handle<T> CreateObject(uint8_t scene);
		template<typename T>  Handle<T> CreateObject(const Handle<T>& handle);
		template<typename T>  bool      ValidateHandle(const Handle<T>& handle);
		template<typename T>  bool      DestroyObject(const Handle<T>& handle);

		static GameState& GetGameState();
	private:
		static inline GameState* _instance = nullptr;
		detail::ObjectPools_t _objects;
	};
}

#include "GameState.inl"