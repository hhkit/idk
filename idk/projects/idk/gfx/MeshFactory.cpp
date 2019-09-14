#include "stdafx.h"
#include "MeshFactory.h"
#include <iostream>

namespace idk
{
	void IMeshFactory::Init()
	{
		GenerateDefaultMeshes();
		auto i = 0;
		for (auto& elem : defaults)
		{
			if (!elem)
				std::cout << "Default mesh " << MeshType{ i } << " not created";
			++i;
		}
	}
}
