#include "stdafx.h"
#include "Component.h"

namespace idk
{
	Handle<GameObject> GenericComponent::GetGameObject() const
	{
		return _gameObject;
	}
}