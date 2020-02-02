#include "pch.h"
#include "RenderUtil.h"

#include <vkn/BufferHelpers.inl>

namespace idk::vkn
{
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size)
	{
		vk::Viewport vp{ s_cast<float>(vp_pos.x),s_cast<float>(vp_pos.y),s_cast<float>(vp_size.x),s_cast<float>(vp_size.y),0,1 };
		cmd_buffer.setViewport(0, vp);
	}

	void SetScissor(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size)
	{
		vk::Rect2D vp{ vk::Offset2D{vp_pos.x,vp_pos.y},vk::Extent2D{vp_size.x,vp_size.y} };
		cmd_buffer.setScissor(0, vp);
	}
	struct ShaderLightData : BaseLightData
	{
		FakeMat4<float> vp;
		ShaderLightData() = default;
		ShaderLightData(const LightData& data) : BaseLightData{ data }, vp{ data.vp }{}
	};
	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights)
	{
		vector<ShaderLightData> tmp_light(lights.size());
		for (size_t i = 0; i < tmp_light.size(); ++i)
		{
			auto& light = tmp_light[i] = (lights)[i];
			light.v_pos = cam.view_matrix * vec4{ light.v_pos,1 };
			light.v_dir = (cam.view_matrix * vec4{ light.v_dir,0 }).get_normalized();
		}

		string light_block;
		uint32_t len = s_cast<uint32_t>(tmp_light.size());
		light_block += string{ reinterpret_cast<const char*>(&len),sizeof(len) };
		light_block += string(16 - sizeof(len), '\0');
		light_block += string{ reinterpret_cast<const char*>(tmp_light.data()), hlp::buffer_size(tmp_light) };
		return light_block;
	}
}