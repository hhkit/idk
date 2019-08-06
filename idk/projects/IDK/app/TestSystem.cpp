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

	void TestSystem::TestSpan(span<const Transform> transforms)
	{
		auto index = 0;
		std::cout << "tfm update: ";
		for (auto& tfm : transforms)
			std::cout << (tfm,index++) << ',';
		std::cout << '\n';
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
