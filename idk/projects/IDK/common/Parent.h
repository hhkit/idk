#pragma once
#include <core/Component.h>

namespace idk
{
	class Parent
		: public Component<Parent>
	{
	public:
		ObjectHandle<GameObject> parent;
		Parent() = default;
	};
}