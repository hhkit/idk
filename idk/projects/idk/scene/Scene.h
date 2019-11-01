#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <ds/span.h>
#include <res/Resource.h>
#include <res/SaveableResource.h>

namespace idk
{
	class GameObject;
	class Scene;

	RESOURCE_EXTENSION(Scene, ".ids")

	enum class SceneLoadResult
	{
		Ok = 0,
		Err_SceneAlreadyActive,
		Err_ScenePathNotFound,
	};
	
	enum class SceneUnloadResult
	{
		Ok = 0,
		Err_SceneAlreadyInactive,
	};

	class Scene 
		: public Resource<Scene>
		, public Saveable<Scene, false_type>
	{
    public:
        static constexpr uint8_t editor = 0x80;
        static constexpr uint8_t prefab = 0x81;

	public:
		class iterator;
		explicit Scene(unsigned char index = 0);

		Handle<GameObject> CreateGameObject(const Handle<GameObject>&);
		Handle<GameObject> CreateGameObject();
		void               DestroyGameObject(const Handle<GameObject>&);

		bool              Loaded();
		SceneLoadResult   LoadFromResourcePath();
		SceneLoadResult   Activate();
		SceneUnloadResult Deactivate();

		iterator begin() const;
		iterator end() const;
	private:
		friend class ProjectManager;
		friend class GameState;

		uint8_t scene_id;
		bool      _loaded = 0;
	};

	class Scene::iterator
	{
	public:
		iterator() = default;

		GameObject& operator*();
		GameObject* operator->();
		iterator&   operator++();
		iterator    operator++(int);
		bool        operator==(const iterator&) const;
		bool        operator!=(const iterator&) const;
	private:
		GameObject* curr_ = nullptr;
		GameObject* end_  = nullptr;
		uint8_t scene_id_ = 0;
		iterator(uint8_t scene_id, GameObject* curr, GameObject* end);

		friend Scene;
	};
}