#include "pch.h"
#include "CubemapRenderer.h"
#include <vkn/UboManager.inl>
#include <math/matrix_transforms.inl>
#include "RenderUtil.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

namespace idk::vkn
{
	VulkanPipeline& CubemapRenderer::Pipeline()
	{
		return *pipeline;
	}
	PipelineManager& CubemapRenderer::pipeline_manager()const
	{
		return *_pipeline_manager;
	}
	PipelineManager& CubemapRenderer::pipeline_manager(PipelineManager& mgr)
	{
		return *(_pipeline_manager = &mgr);
	}
	string CubemapRenderer::EpName() const { return "environment_probe"; }
	string CubemapRenderer::M4Name() const { return "view_projection"; }
	void CubemapRenderer::Init(RscHandle<ShaderProgram> v, RscHandle<ShaderProgram> f, RscHandle<ShaderProgram> g, pipeline_config* pipline_conf, RscHandle<Mesh> mesh)
	{
		pipeline_config& config = *(config_ = std::make_shared<pipeline_config>());
		if (pipline_conf)
			config = { *pipline_conf }; //Attempt using default copy ctor to copy the external configs
		else
		{
			config.cull_face = static_cast<uint32_t>(CullFace::eNone);
			config.depth_test = false;
			config.depth_write = false;
			config.render_pass_type = BasicRenderPasses::eRgbaColorOnly;
		}
		auto hvert = v, hgeom = g, hfrag = f;
		//vector<RscHandle<ShaderProgram>> prog;
		prog.clear();
		prog.reserve(3);
		if (hvert)
		{
			vert = RscHandle<ShaderModule>{ hvert };
			prog.emplace_back(hvert);
		}
		if (hgeom)
		{
			geom = RscHandle<ShaderModule>{ hgeom };
			prog.emplace_back(hgeom);
		}
		if (hfrag)
		{
			frag = RscHandle<ShaderModule>{ hfrag };
			prog.emplace_back(hfrag);
		}


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
		//environment_probe;

		req.mesh_requirements = {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2) };

		h_mesh = (mesh) ? mesh : Mesh::defaults[MeshType::Box];

		ro.mesh = h_mesh;
		ro.config = config_;
		static renderer_attributes single_pass_cube = { {
			{vtx::Attrib::Position,0}
		}
		};
		ro.renderer_req = &single_pass_cube;
		;
	}

	RscHandle<VknFrameBuffer> CubemapRenderer::NewFrameBuffer(RscHandle<CubeMap> dst)
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

	void CubemapRenderer::BeginQueue(UboManager& ubo_manager, std::optional<vk::Fence >ofence)
	{
		pipeline_manager().View(View());
		//pipeline = &pipeline_manager().GetPipeline(*config_, prog, 0);
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

		if (vert)
			thingy.BindShader(ShaderStage::Vertex, RscHandle<ShaderProgram>{vert});
		if (geom)
			thingy.BindShader(ShaderStage::Geometry, RscHandle<ShaderProgram>{geom});
		if (frag)
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

	//For skybox
	void CubemapRenderer::QueueSkyBox(UboManager& ubo_manager, std::optional<vk::Fence >ofence, RscHandle<CubeMap> src, const mat4& vp_trnsform)
	{
		pipeline = &pipeline_manager().GetPipeline(*config_, prog, 0);
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
		//static const auto perspective_matrix = perspective(deg{ 90 }, 1.f, 0.1f, 100.f);

		ResetRsc();
		thingy.SetRef(ubo_manager);

		if (vert)
			thingy.BindShader(ShaderStage::Vertex, RscHandle<ShaderProgram>{vert});
		if (geom)
			thingy.BindShader(ShaderStage::Geometry, RscHandle<ShaderProgram>{geom});
		if (frag)
			thingy.BindShader(ShaderStage::Fragment, RscHandle<ShaderProgram>{frag});
		{
			mat4 mat4block[] =
			{
				vp_trnsform
			};
			thingy.BindUniformBuffer("CameraBlock", 0, mat4block);

			thingy.BindSampler("sb", 0, src.as<VknCubemap>());
			thingy.BindMeshBuffers(ro.mesh, req);
			thingy.FinalizeDrawCall(ro);
		}
	}

	//Returns false when it failes to queue the new instruction because the queue is full.
	//End this queue with ProcessQueue before beginning again.

	void CubemapRenderer::QueueConvoluteCubeMap(RscHandle<CubeMap> src, RscHandle<CubeMap> dst)
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

		//auto temp = citr->second->GetRenderPass();
		//temp;
		
		thingy.BindSampler(EpName(), 0, src.as<VknCubemap>());
		thingy.BindMeshBuffers(ro.mesh, req);
		thingy.FinalizeDrawCall(ro);
	}

	void CubemapRenderer::QueueRenderToCubeMap(RscHandle<CubeMap> dst)//WIP
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
		RenderImpl();
		//thingy.FinalizeDrawCall(ro);
	}
//#pragma optimize("",off)
	void CubemapRenderer::ProcessQueue(vk::CommandBuffer cmd_buffer)
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
		//cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline().Pipeline());
		thingy.GenerateDS(ds_manager);
		auto& p_ros = thingy.DrawCalls();

		IDK_ASSERT(p_ros.size() == frame_buffers.size());

		for (size_t i = 0; i < p_ros.size(); ++i)
		{
			auto& p_ro = p_ros[i];
			auto& dst = frame_buffers[i];
			//TODO bind mesh

			auto rp_obj = dst->GetRenderPass();
			
			pipeline = &pipeline_manager().GetPipeline(*config_, prog, 0, rp_obj);
			cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->Pipeline());
			
			auto size = dst->Size();
			vk::ClearValue clear{ vk::ClearColorValue{} };

			vk::RenderPassBeginInfo info
			{
				*rp_obj,dst->GetFramebuffer(),
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
			{
				uint32_t set = 0;
				for (auto& ods : p_ro.descriptor_sets)
				{
					if (ods)
					{
						auto& ds = *ods;
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Pipeline().PipelineLayout(), set, ds, {});
					}
					++set;
				}
			}

			//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), mat4blk.first, mat4blk.second, {});
			//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), environment_probe.first, environment_probe.second, {});

			auto& mesh = p_ro.Object().mesh.as<VulkanMesh>();

			for (auto&& [attrib, location] : req.mesh_requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				auto binding = Pipeline().GetBinding(location);
				if (binding)
					cmd_buffer.bindVertexBuffers(*binding, *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
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
//#pragma optimize("",on)
	void CubemapRenderer::ProcessQueueWithoutRP(vk::CommandBuffer cmd_buffer, const ivec2& vp_pos, const ivec2& vp_size)
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
		pipeline = &pipeline_manager().GetPipeline(*config_, prog, 0);
		cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline().Pipeline());
		thingy.GenerateDS(ds_manager);
		auto& p_ros = thingy.DrawCalls();

		//IDK_ASSERT(p_ros.size() == frame_buffers.size());

		for (size_t i = 0; i < p_ros.size(); ++i)
		{
			auto& p_ro = p_ros[i];

			//TODO set Viewport
			SetViewport(cmd_buffer, vp_pos, vp_size);

			//glDisable(GL_CULL_FACE); // vk::PipelineRasterizationStateCreateInfo
			//glDisable(GL_DEPTH_TEST);// vk::PipelineDepthStencilStateCreateInfo{};
			{
				uint32_t set = 0;
				for (auto& ods : p_ro.descriptor_sets)
				{
					if (ods)
					{
						auto& ds = *ods;
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Pipeline().PipelineLayout(), set, ds, {});
					}
					++set;
				}
			}

			//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), mat4blk.first, mat4blk.second, {});
			//cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), environment_probe.first, environment_probe.second, {});

			auto& mesh = p_ro.Object().mesh.as<VulkanMesh>();

			for (auto&& [attrib, location] : req.mesh_requirements)
			{
				auto& attrib_buffer = mesh.Get(attrib);
				auto binding = Pipeline().GetBinding(location);
				if (binding)
					cmd_buffer.bindVertexBuffers(*binding, *attrib_buffer.buffer(), vk::DeviceSize{ attrib_buffer.offset }, vk::DispatchLoaderDefault{});
			}

			auto& oidx = mesh.GetIndexBuffer();
			if (oidx)
			{
				cmd_buffer.bindIndexBuffer(*(*oidx).buffer(), 0, mesh.IndexType(), vk::DispatchLoaderDefault{});
				cmd_buffer.drawIndexed(mesh.IndexCount(), 1, 0, 0, 0, vk::DispatchLoaderDefault{});
			}


			//cmd_buffer.endRenderPass();
		}
	}

	void CubemapRenderer::ResetRsc()
	{
		thingy.~PipelineThingy();
		new (&thingy) PipelineThingy{};
		ds_manager.Reset();
		frame_buffers.resize(0);
	}

	void CubemapRenderer::RenderImpl()
	{
		//WIP
	}

	CubemapRenderer::UniqueVknFrameBuffer::~UniqueVknFrameBuffer()
	{
		if (frame_buffer)
			Core::GetResourceManager().Release(*frame_buffer);
	}

}