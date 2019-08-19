#pragma once
#include <idk.h>

namespace idk
{
	struct VertexAttribDesc
	{
		unsigned offset = 0;
		unsigned stride = 0;
		
	};

	struct VertexDescriptor
	{
		using Descriptors = vector<VertexAttribDesc>;
		Descriptors descriptors;

		auto begin() { return descriptors.begin(); }
		auto end()   { return descriptors.end(); }
	};
}