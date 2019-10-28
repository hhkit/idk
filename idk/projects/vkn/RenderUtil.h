#pragma once
#include <vkn/VknCubemap.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VknFrameBuffer.h>
#include <ds/circular_buffer.h>
#include <vkn/ShaderModule.h>
#include <gfx/vertex_descriptor.h>
#include <vkn/VulkanMesh.h>
#include <gfx/GraphicsSystem.h>

#include <vkn/PipelineThingy.h>
#include <gfx/FramebufferFactory.h>
#include <vkn/PipelineManager.h>
namespace idk::vkn
{
	VulkanView& View();
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, ivec2 vp_size);
	struct CubemapConvoluter
	{
		PipelineManager pipeline_manager;
		VulkanPipeline* pipeline;
		VulkanPipeline& Pipeline();
		std::pair<uint32_t, vk::DescriptorSet> mat4blk;
		std::pair<uint32_t,vk::DescriptorSet>  environment_probe;
		renderer_reqs req;
		RscHandle<Mesh> h_mesh;
		RscHandle<ShaderModule> vert;
		RscHandle<ShaderModule> geom;
		RscHandle<ShaderModule> frag;

		vector<RscHandle<VknFrameBuffer>> frame_buffers; //should match ROs in thingy

		DescriptorsManager ds_manager;

		PipelineThingy thingy{};

		string EpName()const;
		string M4Name()const;

		RenderObject ro;
		shared_ptr< pipeline_config> config_;

		void Init();
		struct UniqueVknFrameBuffer
		{
			std::optional<RscHandle<VknFrameBuffer>> frame_buffer{};
			RscHandle<CubeMap> cube_map;
			UniqueVknFrameBuffer() = default;

			UniqueVknFrameBuffer(const UniqueVknFrameBuffer&) = delete;
			UniqueVknFrameBuffer(UniqueVknFrameBuffer&&) = default;
			UniqueVknFrameBuffer&operator=(const UniqueVknFrameBuffer&) = delete;
			UniqueVknFrameBuffer&operator=(UniqueVknFrameBuffer&&) = default;

			~UniqueVknFrameBuffer()
			{
				if (frame_buffer)
					Core::GetResourceManager().Release(*frame_buffer);
			}
		};

		//circular_buffer<UniqueVknFrameBuffer,32> pool;
		hash_table < RscHandle<CubeMap>, RscHandle<VknFrameBuffer>> cached;
		hash_table   <RscHandle<CubeMap>,RscHandle<VknFrameBuffer>> unused;
		RscHandle<VknFrameBuffer> NewFrameBuffer(RscHandle<CubeMap> dst);
		void BeginQueue(UboManager& ubo_manager,std::optional<vk::Fence> fence);
		//Returns false when it failes to queue the new instruction because the queue is full.
		//End this queue with ProcessQueue before beginning again.
		void QueueConvoluteCubeMap(RscHandle<CubeMap> src, RscHandle<CubeMap> dst);
		void ProcessQueue(vk::CommandBuffer cmd_buffer);

		void ResetRsc();

		CubemapConvoluter():ds_manager{View()}{}
	};
}