#pragma once
#include <idk.h>
#include <gfx/vertex_descriptor.h>
#include <res/ResourceExtension.h>
#include <math/shapes/sphere.h>
namespace idk
{
	struct CompiledBuffer
	{
		vector<vtx::Descriptor> attribs;
		string   data;
	};

	struct CompiledMesh
	{
		vector<CompiledBuffer> buffers;
		vector<unsigned>       element_buffer;
		sphere                 bounding_volume;
		EXTENSION(".idmesh");
	};
}