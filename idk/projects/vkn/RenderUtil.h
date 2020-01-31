#pragma once
#include <vkn/VulkanPipeline.h>
#include <gfx/GraphicsSystem.h>

namespace idk::vkn
{
	VulkanView& View();
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size);
	void SetScissor(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uvec2 vp_size);


	template<typename T>
	struct FakeMat4
	{
		static constexpr unsigned N = 4;
		tvec<T, N> data[N];

		FakeMat4() = default;
		FakeMat4(const tmat<T, N, N>& m)
		{
			for (uint32_t i = 0; i < N; ++i)
			{
				data[i] = m[i];
			}
		}
	};



	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights);
}