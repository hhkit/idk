#pragma once
#include <idk.h>
#include <gfx/vertex_descriptor.h>
#include <res/ResourceExtension.h>

namespace idk
{
	struct CompiledBuffer
	{
		vector<vtx::Descriptor> attribs;
		vector<unsigned char>   data;
	};

	struct CompiledMesh
	{
		vector<CompiledBuffer> buffers;
		vector<unsigned>       element_buffer;
		sphere                 bounding_volume;
		EXTENSION(".idmesh");
	};
}