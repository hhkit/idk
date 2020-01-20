#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class TestComponent
		: public Component<TestComponent>
	{
	public:
		int a;
		int b;
		int c;
		int d;
		void NetworkUpdate();
	};
}