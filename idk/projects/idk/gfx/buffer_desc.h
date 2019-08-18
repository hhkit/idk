#pragma once
#include <idk.h>
#include <optional>
namespace idk
{
	enum VertexRate
	{
		eVertex
		, eInstance
	};
	enum AttribFormat
	{
		eSVec2,//Single Precision Floats
		eSVec3,//Single Precision Floats
		eSVec4,//Single Precision Floats
		eMat3,
		eMat4
	};
	struct buffer_desc
	{
		struct binding_info
		{
			std::optional<uint32_t> binding_index{};
			uint32_t stride{};
			VertexRate vertex_rate{};
		};
		struct attribute_info
		{
			AttribFormat format{};
			//Must offset the next location by 3/4 when AttribFormat is Mat3/4
			uint32_t     location{};
			uint32_t     offset{};
		};
		binding_info           binding = {};
		vector<attribute_info> attributes = {};

		size_t AddAttribute(AttribFormat format
			//Must offset the next location by 3/4 when AttribFormat is Mat3/4
			, uint32_t     location
			, uint32_t     offset
		);

		buffer_desc Process(uint32_t location_offset)const;
	};
}
