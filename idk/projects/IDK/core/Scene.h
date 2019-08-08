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
		explicit Scene(uint8_t scene_id = 0);
		Handle<GameObject> CreateGameObject();
		void               DestroyGameObject(const Handle<GameObject>&);
		span<GameObject>   GetAllGameObjects();
	private:
		uint8_t scene_id;
		friend class GameState;
	};

	Scene GetScene(const GenericHandle&);
}