#pragma once
#include <core/Component.h>
#include <core/Scene.h>
#include "GameState_detail.h"

namespace idk
{
	class Scene;
	class GameObject;

	class GameState
	{
	public:
		GameState();
		~GameState();

		opt<Scene> ActivateScene(uint8_t scene);
		bool       DectivateScene(uint8_t scene);
		bool       DectivateScene(Scene scene);

		bool ValidateHandle(const GenericHandle& handle);
		void DestroyObject(const GenericHandle&);
		void DestroyObject(const Handle<GameObject>&);
		void DestroyQueue();

		template<typename T>  span<T>   GetObjectsOfType();

		template<typename T>  T*        GetObject(const Handle<T>& handle);
		template<typename T>  Handle<T> CreateObject(uint8_t scene);
		template<typename T>  Handle<T> CreateObject(const Handle<T>& handle);
		template<typename T>  bool      ValidateHandle(const Handle<T>& handle);
		template<typename T>  bool      DestroyObjectNow(const Handle<T>& handle);

		static GameState& GetGameState();
	private:
		static inline GameState* _instance = nullptr;
	
		detail::ObjectPools_t _objects;
		vector<GenericHandle> _destruction_queue;

		template<typename Fn>
		using JumpTable = array<Fn, detail::ObjectPools::TypeCount>;


		using DestroyJT = JumpTable<void(*)(GameState&, const GenericHandle&)>;
		using ValidateJT = JumpTable<bool(*)(GameState&, const GenericHandle&)>;
		static inline DestroyJT  destroy_handles_jt;
		static inline ValidateJT validate_handles_jt;

		friend detail::ObjectPools;
	};
}

#include "GameState.inl"