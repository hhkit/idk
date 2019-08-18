#include "pch.h"
#include "buffer_desc.h"
namespace idk
{
size_t buffer_desc::AddAttribute(AttribFormat format, uint32_t location, uint32_t offset)
{
	size_t index = attributes.size();
	attribute_info attrib{ format,location,offset };
	if (attrib.format == eMat3 || attrib.format == eMat4)
	{
		int count = ((attrib.format == eMat3) ? 3 : 4) - 1;
		attrib.format = (attrib.format == eMat3) ? eSVec3 : eSVec4;
		while (count-- > 0)
		{
			attributes.emplace_back(attrib);
			attrib.location++;
			attrib.offset += (attrib.format == eSVec3) ? sizeof(vec3) : sizeof(vec4);
		}
	}
	attributes.emplace_back(attrib);
	return index;
}

buffer_desc buffer_desc::Process(uint32_t location_offset) const
{
	buffer_desc transformed;
	transformed.binding = binding;
	for (auto attrib : attributes)
	{
		attrib.location += location_offset;
		if (attrib.format == eMat3 || attrib.format == eMat4)
		{
			int count = ((attrib.format == eMat3) ? 3 : 4) - 1;
			attrib.format = (attrib.format == eMat3) ? eSVec3 : eSVec4;
			while (count-- > 0)
			{
				transformed.attributes.emplace_back(attrib);
				attrib.location++;
				attrib.offset += (attrib.format == eSVec3) ? sizeof(vec3) : sizeof(vec4);
			}
		}
		transformed.attributes.emplace_back(attrib);
		return transformed;
	}
}
}