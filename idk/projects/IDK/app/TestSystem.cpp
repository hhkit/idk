#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <common/Transform.h>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
	}

	void TestSystem::TestSpan(span<class Transform> transforms)
	{
		for (auto& elem : transforms)
			elem.GetMatrix();
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
