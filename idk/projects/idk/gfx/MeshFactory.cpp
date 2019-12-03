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
				LOG_TO(LogPool::GFX, "Default mesh %s not created\n", MeshType{ i }.to_string().data());
			else
			{
				elem->Name(string{ "Default" } +string{ MeshType{ i }.to_string() });
			}
			++i;
		}
	}
}
