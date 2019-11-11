#pragma once
#include <res/ResourceFactory.h>
#include <gfx/Mesh.h>
#include <util/enum.h>

namespace idk
{

	class IMeshFactory
		: public ResourceFactory<Mesh>
	{
	public:
		void Init() override final; // force inheritors to create default meshes
		virtual void GenerateDefaultMeshes() = 0;
	};
}