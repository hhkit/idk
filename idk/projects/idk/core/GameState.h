#pragma once
#include <core/Component.h>
#include <core/Scene.h>
#include "GameState_detail.h"
#undef GetObject

namespace idk::reflect
{
	class type;
}

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

		bool CreateObject(const GenericHandle& handle);
		GenericHandle CreateComponent(const Handle<GameObject>&, reflect::type);
		bool ValidateHandle(const GenericHandle& handle);
		void DestroyObject(const GenericHandle&);
		void DestroyObject(const Handle<GameObject>&);
		void DestroyQueue();

		uint8_t GetTypeID(const reflect::type&);

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

		using CreateTypeJT = JumpTable<GenericHandle(*)(GameState&, const Handle<GameObject>&)>;
		using CreateJT = JumpTable<GenericHandle(*)(GameState&, const GenericHandle&)>;
		using DestroyJT = JumpTable<void(*)(GameState&, const GenericHandle&)>;
		using ValidateJT = JumpTable<bool(*)(GameState&, const GenericHandle&)>;
		static inline CreateTypeJT create_type_jt;
		static inline CreateJT     create_handles_jt;
		static inline DestroyJT    destroy_handles_jt;
		static inline ValidateJT   validate_handles_jt;

		using TypeIDLUT = hash_table<string_view, uint8_t>;
		static inline TypeIDLUT name_to_type_id;

		friend detail::ObjectPools;
		template<typename T>
		friend struct detail::TableGenerator;
	};

	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(uint8_t);
	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(const Handle<class GameObject>&);
}

#include "GameState.inl"