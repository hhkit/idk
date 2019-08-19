#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <gfx/vertex_descriptor.h>

namespace idk
{
	class Mesh
		: public Resource<Mesh>
	{
	public:
		virtual vtx::Flags attribsProvided() const = 0;
	};
}