#include "stdafx.h"
#include "Component.h"

namespace idk
{
	Handle<GameObject> GenericComponent::GetGameObject()
	{
		return _gameObject;
	}
}