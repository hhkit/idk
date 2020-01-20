#pragma once
#include <idk.h>
#include <core/Component.h>
#include <res/ResourceHandle.h>

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
		RscHandle<Prefab> makeme;
		bool send_pos = false;
		bool send_rot = false;
		void NetworkUpdate();
	};
}