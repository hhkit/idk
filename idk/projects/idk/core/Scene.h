#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <ds/span.h>

namespace idk
{
	class GameObject;

	class Scene
	{
	public:
		class iterator;
		explicit Scene(uint8_t scene_id = 0);
		Handle<GameObject> CreateGameObject(const Handle<GameObject>&);
		Handle<GameObject> CreateGameObject();
		void               DestroyGameObject(const Handle<GameObject>&);

		iterator begin();
		iterator end();
	private:
		uint8_t scene_id;
		friend class GameState;
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