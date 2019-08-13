#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"

namespace idk
{
	Scene::Scene(uint8_t scene_id)
		: scene_id{ scene_id }
	{
	}

	Handle<GameObject> Scene::CreateGameObject()
	{
		return GameState::GetGameState().CreateObject<GameObject>(scene_id);
	}

	void Scene::DestroyGameObject(const Handle<GameObject>& go)
	{
		GameState::GetGameState().DestroyObject(go);
	}

	Scene::iterator Scene::begin()
	{
		auto span = GameState::GetGameState().GetObjectsOfType<GameObject>();
		auto beg = span.begin();
		auto etr = span.end();

		while (beg != etr && beg->GetHandle().scene != scene_id)
			++beg; 

		return iterator{scene_id, beg, etr};
	}

	Scene::iterator Scene::end()
	{
		auto span = GameState::GetGameState().GetObjectsOfType<GameObject>();
		return iterator{scene_id, span.end(), span.end() };
	}

	Scene GetScene(const GenericHandle& handle)
	{
		return Scene{ handle.scene };
	}

	GameObject& Scene::iterator::operator*()
	{
		return *curr_;
	}

	GameObject* Scene::iterator::operator->()
	{
		return curr_;
	}

	Scene::iterator& Scene::iterator::operator++()
	{
		do
		{
			++curr_;
		} while (curr_ != end_ && curr_->GetHandle().scene != scene_id_);

		return *this;
	}

	Scene::iterator  Scene::iterator::operator++(int)
	{
		auto copy = *this;
		operator++();
		return copy;
	}

	bool Scene::iterator::operator==(const iterator& rhs) const
	{
		return curr_ == rhs.curr_;
	}

	bool Scene::iterator::operator!=(const iterator& rhs) const
	{
		return curr_ != rhs.curr_;
	}

	Scene::iterator::iterator(uint8_t scene_id, GameObject* curr, GameObject* end)
		: curr_{curr}, end_{end}, scene_id_{scene_id}
	{
	}
}