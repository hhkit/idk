#pragma once
#include <core/Component.h>

namespace idk
{
	class Parent
		: public Component<Parent>
	{
	public:
		Handle<GameObject> parent;
		Parent() = default;
	};
}