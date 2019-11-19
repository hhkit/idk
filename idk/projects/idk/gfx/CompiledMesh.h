#pragma once
#include <idk.h>
#include <gfx/vertex_descriptor.h>
#include <res/ResourceExt.h>

namespace idk
{
	struct CompiledBuffer
	{
		vector<vtx::Attrib>   attribs;
		vector<unsigned char> data_buffer;
	};

	struct CompiledMesh
	{
		vector<CompiledBuffer> buffers;
		vector<unsigned> element_buffer;
		EXTENSION(".idmsh");
	};

}