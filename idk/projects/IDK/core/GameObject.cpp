#include "stdafx.h"
#include "GameObject.h"

namespace idk
{
	GameObject::GameObject(Scene* scene)
		: _scene{ scene }
	{
	}
	void GameObject::SetScene(Scene* scene)
	{
		assert(_scene);
		_scene = scene;
	}
}
