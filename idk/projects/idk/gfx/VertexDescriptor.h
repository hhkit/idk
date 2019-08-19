#pragma once
#include <idk.h>
#include <reflect/reflect.h>

namespace idk
{
	struct VertexAttribDesc
	{
		enum class Type
		{
			Byte,  UByte,
			Short, UShort,
			Int,   UInt,
			Float,
			Double
		};

		unsigned location = 0;
		unsigned offset   = 0;
		unsigned stride   = 0;
		Type     type     = Type::Float;
	};

	struct VertexDescriptor
	{
		using Descriptors = vector<VertexAttribDesc>;
		Descriptors descriptors;

		auto begin() { return descriptors.begin(); }
		auto end()   { return descriptors.end(); }
	};
}