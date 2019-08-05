#include "stdafx.h"
#include "TestSystem.h"
#include <iostream>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
