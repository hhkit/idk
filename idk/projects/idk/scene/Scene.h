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

	class Scene 
		: public Resource<Scene>
		, public Saveable<Scene, false_type>
	{
	public:
		class iterator;
		explicit Scene(uint8_t scene_id);
		virtual ~Scene();
		Handle<GameObject> CreateGameObject(const Handle<GameObject>&);
		Handle<GameObject> CreateGameObject();
		void               DestroyGameObject(const Handle<GameObject>&);

		iterator begin() const;
		iterator end() const;
	private:
		friend class ProjectManager;
		friend class GameState;

		uint8_t scene_id;
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

	Scene GetScene(const GenericHandle&);
}