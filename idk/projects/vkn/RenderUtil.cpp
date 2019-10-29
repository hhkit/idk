#include "pch.h"
#include "RenderUtil.h"
namespace idk::vkn
{
	VulkanPipeline& CubemapConvoluter::Pipeline()
	{
		return *pipeline;
	}
	string CubemapConvoluter::EpName() const { return "environment_probe"; }
	string CubemapConvoluter::M4Name() const { return "view_projection"; }
	void CubemapConvoluter::Init()
{
	pipeline_config& config = *(config_ = std::make_shared<pipeline_config>());
	config.depth_test = false;
	config.depth_write = false;
	config.render_pass_type = BasicRenderPasses::eRgbaColorOnly;
	auto hvert= Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VPBRConvolute],
		 hgeom= Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GSinglePassCube],
		 hfrag= Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPBRConvolute];


	vert = RscHandle<ShaderModule>{ hvert };
	geom = RscHandle<ShaderModule>{ hgeom };
	frag = RscHandle<ShaderModule>{ hfrag };

	pipeline_manager.View(View());

	pipeline = &pipeline_manager.GetPipeline(config, { hvert,hgeom,hfrag }, 0);
	

	//RenderObject baka{};//dummy
	//thingy.FinalizeDrawCall(baka);
	//thingy.GenerateDS(ds_manager);

	//auto& p_ro =thingy.DrawCalls().front();
	//auto m4_layout = thingy.GetUniform(m4_name);
	//auto ep_layout = thingy.GetUniform(ep_name);
	//if(m4_layout)
	//	mat4blk = *p_ro.descriptor_sets.find(m4_layout->set);
	//
	//if (ep_layout)
	//	environment_probe = *p_ro.descriptor_sets.find(ep_layout->set);


	//Create environment probe
	environment_probe;

	req.requirements = {
		std::make_pair(vtx::Attrib::Position, 0),
		std::make_pair(vtx::Attrib::Normal, 1),
		std::make_pair(vtx::Attrib::UV, 2) };
	h_mesh = Mesh::defaults[MeshType::Box];
	ro.mesh = h_mesh;
	ro.config = config_;
}

RscHandle<VknFrameBuffer> CubemapConvoluter::NewFrameBuffer(RscHandle<CubeMap> dst)
{
	FrameBufferBuilder builder;
	auto& cube_map = *dst;
	builder.Begin(cube_map.Size());
	idk::AttachmentInfo info{};
	info.load_op = LoadOp::eClear;
	info.store_op = StoreOp::eStore;
	info.buffer = dst.as<VknCubemap>().Tex();
	builder.AddAttachment(info);
	return RscHandle<VknFrameBuffer>{ Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End())};
}

void CubemapConvoluter::BeginQueue(UboManager& ubo_manager, std::optional<vk::Fence >ofence)
{
	if (ofence)
	{
		auto& fence = *ofence;
	auto fence_status = View().Device()->getFenceStatus(fence);
	while (fence_status == vk::Result::eNotReady)
	{
		fence_status = View().Device()->getFenceStatus(fence);
	};
	//Clear unused FB
	for (auto itr = unused.begin(); itr != unused.end(); ++itr)
	{
		cached.erase(itr->first);
		Core::GetResourceManager().Release(itr->second);
	}

	}
	unused.clear();
	//Mark everything as unused.
	for (auto& [cube_map, frame_buffer] : cached)
	{
		unused.emplace(cube_map, frame_buffer);
	}
	//Create mat4blk
	static const auto perspective_matrix = perspective(deg{ 90 }, 1.f, 0.1f, 100.f);

	ResetRsc();
	thingy.SetRef(ubo_manager);

	static const mat4 view_matrices[] =
	{
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f))) },
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f))) },
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f))) },
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f))) },
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f))) },
		mat4{ invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f))) }
	};
	thingy.BindShader(ShaderStage::Vertex, RscHandle<ShaderProgram>{vert});
	thingy.BindShader(ShaderStage::Geometry, RscHandle<ShaderProgram>{geom});
	thingy.BindShader(ShaderStage::Fragment, RscHandle<ShaderProgram>{frag});
	{
		int i = 0;
		mat4 mat4block[] =
		{
			perspective_matrix,
			//View
			view_matrices[i++],
			view_matrices[i++],
			view_matrices[i++],
			view_matrices[i++],
			view_matrices[i++],
			view_matrices[i++],
		};
		thingy.BindUniformBuffer(M4Name(), 0, mat4block);
	}
}

//Returns false when it failes to queue the new instruction because the queue is full.
//End this queue with ProcessQueue before beginning again.

void CubemapConvoluter::QueueConvoluteCubeMap(RscHandle<CubeMap> src, RscHandle<CubeMap> dst)
{
	auto uitr = unused.find(dst);
	if (uitr != unused.end())
		unused.erase(uitr);

	auto citr = cached.find(dst);
	if (citr == cached.end())
	{
		citr = cached.emplace(dst, NewFrameBuffer(dst)).first;
	}

	frame_buffers.emplace_back(citr->second);

	thingy.BindSampler(EpName(), 0, src.as<VknCubemap>());
	thingy.FinalizeDrawCall(ro);
}

void CubemapConvoluter::ProcessQueue(vk::CommandBuffer cmd_buffer)
{
	//if (pool.size() == pool.capacity())
	//{
	//	auto& expired = pool.front();
	//	auto itr = cached.find(expired.cube_map);
	//	cached.erase(itr);
	//	pool.pop_front();
	//	pool.emplace_back(dst, NewFrameBuffer(dst));
	//}
	//auto frame_buffer = cached.find(dst)->second;
	cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline().Pipeline());
	thingy.GenerateDS(ds_manager);
	auto& p_ros = thingy.DrawCalls();

	IDK_ASSERT(p_ros.size() == frame_buffers.size());

	for (size_t i = 0; i < p_ros.size(); ++i)
	{
		auto& p_ro = p_ros[i];
		auto& dst = frame_buffers[i];
		//TODO bind mesh
		auto size = dst->Size();
		vk::ClearValue clear{ vk::ClearColorValue{} };
		vk::RenderPassBeginInfo info
		{
			dst->GetRenderPass(),dst->GetFramebuffer(),
			vk::Rect2D
		{
			vk::Offset2D{ 0,0 },vk::Extent2D{ s_cast<uint32_t>(size.x),s_cast<uint32_t>(size.y) }
		},1,&clear
		};
		cmd_buffer.beginRenderPass(info, vk::SubpassContents::eInline);

		//TODO set Viewport
		SetViewport(cmd_buffer, ivec2{ 0,0 }, { size.x, size.y });

		//glDisable(GL_CULL_FACE); // vk::PipelineRasterizationStateCreateInfo
		//glDisable(GL_DEPTH_TEST);// vk::PipelineDepthStencilStateCreateInfo{};

		for (auto& [set, ds] : p_ro.descriptor_sets)
		{
			cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Pipeline().PipelineLayout(), set, ds, {});
		}

		//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), mat4blk.first, mat4blk.second, {});
		//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), environment_probe.first, environment_probe.second, {});

		auto& mesh = p_ro.Object().mesh.as<VulkanMesh>();

		for (auto&& [attrib, location] : req.requirements)
		{
			auto& attrib_buffer = mesh.Get(attrib);
			cmd_buffer.bindVertexBuffers(*Pipeline().GetBinding(location), *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
		}

		auto& oidx = mesh.GetIndexBuffer();
		if (oidx)
		{
			cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
			cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
		}


		cmd_buffer.endRenderPass();
	}
}

void CubemapConvoluter::ResetRsc()
{
	thingy.~PipelineThingy();
	new (&thingy) PipelineThingy{};
	ds_manager.Reset();
	frame_buffers.resize(0);
}

}