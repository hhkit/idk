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
		virtual int GetAttribs() const = 0;
		virtual ~Mesh() = default;
	};
}