#pragma once
#include "stdafx.h"
#include "MeshFactory.h"
#include <iostream>

namespace idk
{
	void IMeshFactory::Init()
	{
		GenerateDefaultMeshes();
		auto i = 0;
		for (auto& elem : Mesh::defaults)
		{
			if (!elem)
				std::cout << "Default mesh " << MeshType{ i }.to_string() << " not created\n";
            elem->Name(string{ "Default" } + string{ MeshType{ i }.to_string() });
			++i;
		}
	}
}
