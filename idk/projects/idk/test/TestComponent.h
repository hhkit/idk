#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class TestComponent
		: public Component<TestComponent>
	{
	};
}