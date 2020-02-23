#include "pch.h"
#include "VertexBindingTracker.h"
#include <vkn/GfxConverters.h>
namespace idk::vkn
{
#pragma optimize("",off)
void VertexBindingTracker::Update(RscHandle<ShaderProgram> vtx_shader)
{
	_config.buffer_descriptions.clear();
	pipeline_helper.UseShaderAttribs({ vtx_shader }, _config);
	UpdateLoc();
}

void VertexBindingTracker::Update(const pipeline_config& config, RscHandle<ShaderProgram> vtx_shader)
{
	pipeline_helper.StoreBufferDescOverrides(config);
	Update(vtx_shader);
}

std::optional<uint32_t> VertexBindingTracker::GetBinding(uint32_t location) const
{
	auto itr = loc2bind.find(location);
	if (itr != loc2bind.end())
		return itr->second;
	return {};
}

const vector<buffer_desc>& VertexBindingTracker::GetDescriptors() const
{
	return _config.buffer_descriptions;
}

void VertexBindingTracker::UpdateLoc()
{
	auto [vtx_info, attr_desc] = hlp::ConvertVtxDesc(_config.buffer_descriptions);
	loc2bind.clear();
	for (auto& attr : attr_desc)
	{
		loc2bind.emplace(attr.location, attr.binding);
	}
}

}