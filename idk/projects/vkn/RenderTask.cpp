#include "pch.h"
#include "RenderTask.h"

#include <vkn/ShaderModule.h>
#include <vkn/RenderBundle.h>
#include <vkn/utils/VknUtil.h>

#include <vkn/PipelineManager.h>
#include <res/ResourceHandle.inl>

#include <vkn/DebugUtil.h>

#include <vkn/Stopwatch.h>
#include <vkn/time_log.h>

#include <vkn/DescriptorUpdateData.h>

using idk::vkn::dbg::add_rendertask_durations;


static void DoNothing()
{

}

void dbg_chk([[maybe_unused]]vk::Image img)
{
	return;
}

namespace idk::vkn
{

	struct count_duration
	{
		int count{};
		dbg::milliseconds total_duration;
		count_duration& operator+=(count_duration rhs)
		{
			count += rhs.count;
			total_duration += rhs.total_duration;
			return *this;
		}
	};
	dbg::time_log& GetGfxTimeLog();
	hash_table<string, count_duration>& ProcBatchLut();

	
	RenderTask::RenderTask()//UniformManager& um) : _uniform_manager{um}
	{
		//batches.reserve(256);
	}
	idk::vkn::RenderTask::DCMoveWrapper::DCMoveWrapper(DCMoveWrapper&& rhs)
		:_vertex_bindings{std::move(rhs._vertex_bindings)},
		 _dc_builder{std::move(rhs._dc_builder)}
	{
		_dc_builder.set_bindings(_vertex_bindings);
	}
	RenderTask::DCMoveWrapper& RenderTask::DCMoveWrapper::operator=(DCMoveWrapper&& rhs)
	{
		_vertex_bindings.operator=(std::move(rhs._vertex_bindings));
		_dc_builder.operator=(std::move(rhs._dc_builder));
		_dc_builder.set_bindings(_vertex_bindings);
		return *this;
	}
	void RenderTask::DebugLabel(LabelLevel type, string label)
	{
		switch (type)
		{
		case idk::vkn::RenderTask::LabelLevel::eDrawCall:
			this->_dc_bindings._dc_builder.SetLabel(LabelType::eInsert, std::move(label));
			break;
		case idk::vkn::RenderTask::LabelLevel::eBatch:
			this->_current_batch.label = std::move(label);
			break;
		case idk::vkn::RenderTask::LabelLevel::eWhole:
			this->_label = std::move(label);
			break;
		default:
			break;
		}
	}
	void RenderTask::SetUboManager(UboManager& ubo_manager)
	{
		_uniform_manager.SetUboManager(ubo_manager);
	}

	void RenderTask::SetPipelineManager(PipelineManager& pipeline_manager)
	{
		ppm = &pipeline_manager;
	}




	void RenderTask::BindVertexBuffer(uint32_t location, VertexBuffer vertex_buffer, size_t byte_offset)
	{
		auto binding = _vtx_binding_tracker.GetBinding(location);
		if (binding)
		{
			if (!vertex_buffer)
				DoNothing();
			_dc_bindings._dc_builder.AddVertexBuffer(VertexBindingData{ vertex_buffer,*binding,byte_offset });
		}
		else
		{

			DoNothing();
		}
	}
	void RenderTask::BindVertexBufferByBinding(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset)
	{
		_dc_bindings._dc_builder.AddVertexBuffer(VertexBindingData{ vertex_buffer,binding,byte_offset });
	}
	void RenderTask::BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType)
	{
		_dc_bindings._dc_builder.SetIndexBuffer(IndexBindingData{ buffer,offset,indexType});
	}
	void RenderTask::BindDescriptorSet(uint32_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl)
	{
		_uniform_manager.BindDescriptorSet(set,ds,dsl);
	}
	void RenderTask::BindUniform(string_view name, uint32_t index, string_view data,bool skip_if_bound)
	{
		dbg::stopwatch timer;
		timer.start();
		_uniform_manager.BindUniformBuffer(name, index, data,skip_if_bound);
		timer.stop();
		add_rendertask_durations("bind ubo", timer.time().count());
	}
	//void RenderTask::BindUniform(vk::DescriptorSet ds, std::optional<string_view> data)
	//{
	//	_uniform_manager.BindUniformBuffer(ds,data);
	//}

	static void DoNothing() {}
	void RenderTask::BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		dbg::stopwatch timer;
		timer.start();
		_uniform_manager.BindSampler(name, index, texture, skip_if_bound,layout);
		timer.stop();
		add_rendertask_durations("bind tex", timer.time().count());
	}
	void RenderTask::BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader_handle)
	{
		auto& bound_shader = _current_batch.shaders.shaders[static_cast<size_t>(stage)];
		if (bound_shader == shader_handle || shader_handle.guid == Guid{})
			return;
		auto& shader = shader_handle.as<ShaderModule>();
		if (!shader.HasCurrent())
			return;
		UnbindShader(stage);
		//DebugBreak();
		_uniform_manager.AddShader(shader);

		bound_shader = shader_handle;
		if (stage == ShaderStage::Fragment)
			BindInputAttachmentToCurrent();
		else if (stage == ShaderStage::Vertex)
			_vtx_binding_tracker.Update(_current_batch.pipeline,shader_handle);
	}
	void RenderTask::UnbindShader(ShaderStage stage)
	{
		StartNewBatch();
		auto& oshader = _current_batch.shaders.shaders[static_cast<size_t>(stage)];
		if (oshader)
		{
			auto& shader = oshader->as<ShaderModule>();
			_uniform_manager.RemoveShader(shader);
			oshader.reset();
		}
	}
	void RenderTask::SetRenderPass(VknRenderPass render_pass)
	{
		curr_rp = render_pass;
	}
	void RenderTask::SetFrameBuffer(const Framebuffer& fb, uvec2 size)
	{
		curr_frame_buffer = fb;
		fb_size = size;
	}
	void RenderTask::Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance)
	{
		vertex_draw_info di{};
		di.first_instance = first_instance;
		di.num_instances = num_instances;
		di.num_vertices = num_vertices;
		di.first_vertex = first_vertex;
		auto uniforms = _uniform_manager.FinalizeCurrent(_uniform_sets);
		if (uniforms)
			AddToBatch(_dc_bindings._dc_builder.end(di, *uniforms));
	}
	//static bool derpz = false;
	//void TestFunc(RenderTask::RenderBatch& _current_batch,const RenderTask::DrawCall& draw_call)
	//{
	//	_current_batch.draw_calls.emplace_back(draw_call);
	//	//_start_new_batch = false;
	//
	//}
	void RenderTask::DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance)
	{
		indexed_draw_info di{};
		di.first_instance = first_instance;
		di.num_instances = num_instances;
		di.num_indices = num_indices;
		di.first_index = first_index;
		di.first_vertex = first_vertex;
		auto uniforms = _uniform_manager.FinalizeCurrent(_uniform_sets);
		//derpz = uniforms.operator bool();
		if (uniforms)
		//{
		//	TestFunc(_current_batch, _dc_bindings._dc_builder.end(di, *uniforms));
		//	_start_new_batch = true;
		//}

			AddToBatch(_dc_bindings._dc_builder.end(di, *uniforms));
	}
	void RenderTask::Copy(CopyCommand&& copy)
	{
		_copy_commands.emplace_back(std::move(copy));
	}
	bool RenderTask::SetPipeline(const VulkanPipeline& pipeline)
	{
		bool can_set = &pipeline != _current_batch.pipeline_override;
		if (can_set)
		{
			StartNewBatch();
			_current_batch.pipeline_override = &pipeline;
		}
		return can_set;
	}
	void RenderTask::Copy(const CopyCommand& copy)
	{
		_copy_commands.emplace_back(copy);
	}
	void RenderTask::SetBufferDescriptions(span<const buffer_desc> descriptions)
	{
		StartNewBatch();
		_current_batch.pipeline.buffer_descriptions = {descriptions.begin(),descriptions.end()};
		auto& vtx_shader = _current_batch.shaders.shaders[static_cast<int>(ShaderStage::Vertex)];
		if(vtx_shader)
			_vtx_binding_tracker.Update(_current_batch.pipeline,*vtx_shader);
	}
	void RenderTask::SetBlend(uint32_t attachment_index, AttachmentBlendConfig blend_config)
	{
		auto& configs = _current_batch.pipeline.attachment_configs;
		uint32_t size = static_cast<uint32_t>(configs.size());
		configs.resize(std::max(size, attachment_index + 1));
		configs[attachment_index] = blend_config;
	}

	//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
	//If col is nullopt, we clear all the colors from attachment_index onwards.

	void RenderTask::SetClearColor(uint32_t attachment_index, std::optional<color> col)
	{
		if (col)
			clear_colors[attachment_index] = *col;
		else
			clear_colors.resize(attachment_index);
	}
	void RenderTask::SetClearDepthStencil(std::optional<float> depth, std::optional<uint8_t> stencil)
	{
		clear_depths = depth;
		clear_stencil = stencil;
		auto depth_value = (depth) ? *depth : 1.0f;
		auto stencil_value =(stencil) ? *stencil : 0ui8;
		if (clear_depths || clear_stencil)
		{
			_clear_depth_stencil = vk::ClearDepthStencilValue{ depth_value,stencil_value};
		}
		else
		{
			_clear_depth_stencil = {};
		}
	}
	rect ViewportScissors(rect r, uvec2 fb_size)
	{
		r.Scale(fb_size);
		r.position = max(r.position, vec2{ 0.0f,0.0f });
		r.size = min(r.size, vec2{ fb_size } -r.position);

		if (r.position.x + r.size.x > fb_size.x
			||
			r.position.y + r.size.y > fb_size.y)
		{
			DoNothing();
		}
		return r;
	}
	void RenderTask::SetScissors(rect r)
	{
		StartNewBatch();
		r = ViewportScissors(r, fb_size);
		_rect_builder->start();
		_rect_builder->emplace_back(r);
		_current_batch.scissor = _rect_builder->end();
	}
	void RenderTask::SetViewport(rect r)
	{
		StartNewBatch();
		r = ViewportScissors(r, fb_size);
		_rect_builder->start();
		_rect_builder->emplace_back(r);
		_current_batch.viewport = _rect_builder->end();
	}
	void RenderTask::SetScissorsViewport(rect r)
	{
		SetScissors(r);
		SetViewport(r);
	}
	void RenderTask::SetFillType(FillType type)
	{
		StartNewBatch();
		_current_batch.pipeline.fill_type = type;
	}
	void RenderTask::SetCullFace(CullFaceFlags cf)
	{
		StartNewBatch();
		_current_batch.pipeline.cull_face = cf;
	}
	void RenderTask::SetPrimitiveTopology(PrimitiveTopology pt)
	{
		StartNewBatch();
		_current_batch.pipeline.prim_top = pt;
	}
	void RenderTask::SetDepthTest(bool enabled)
	{
		StartNewBatch();
		_current_batch.pipeline.depth_test = enabled;
	}
	void RenderTask::SetDepthWrite(bool enabled) {
		StartNewBatch();
		_current_batch.pipeline.depth_write = enabled;
	}
	void RenderTask::SetStencilTest(bool enabled)
	{
		StartNewBatch();
		_current_batch.pipeline.stencil_test = enabled;
	}
	void RenderTask::SetStencilWrite(bool enabled)
	{
		StartNewBatch();
		_current_batch.pipeline.stencil_write = enabled;
	}
	const pipeline_config& RenderTask::GetCurrentConfig() const noexcept
	{
		return _current_batch.pipeline;
	}
	void RenderTask::SkipRenderPass(bool skip)
	{
		_skip_render_pass = skip;
	}
	void RenderTask::SetInputAttachments(span<VknTextureView> input_attachments) noexcept
	{
		_input_attachments = input_attachments;
	}

	void RenderTask::SetOutputAttachmentSize(size_t size)
	{
		_num_output_attachments = size;
	}

	void RenderTask::SetClearDepthStencil(std::optional<vk::ClearValue> clear_value)
	{
		_clear_depth_stencil = clear_value;
	}

	void RenderTask::PreprocessDescriptors(DescriptorUpdateData& dud, DescriptorsManager& dm)
	{
		vector<vk::DescriptorSet>& uniform_sets = _descriptor_sets;
		uniform_sets.resize(_uniform_sets.size());

		_uniform_manager.GenerateDescriptorSets(span{ _uniform_sets },dud, dm, uniform_sets);
	}

	uint32_t compute_clear_info(size_t num_output_attachments,span<const color> clear_colors, std::optional<vk::ClearValue> clear_depth_stencil,
		vector<vk::ClearValue>& clear
		)
	{
		clear.clear();
		clear.reserve(num_output_attachments + ((clear_depth_stencil) ? 1 : 0));
		for (auto color : clear_colors)
		{
			clear.emplace_back(std::array<float, 4> { color.r,color.g,color.b,color.a });
		}
		while (clear.size() < num_output_attachments)
		{
			clear.emplace_back(std::array<float, 4> { 0, 0, 0, 0 });
		}
		if(clear_depth_stencil)
			clear.emplace_back(*clear_depth_stencil);
		return static_cast<uint32_t>(clear.size());
	}

	float test{};
	void RenderTask::ProcessBatches(RenderBundle& render_bundle)
	{
		ProcessBatches(render_bundle._cmd_buffer);
	}
	//Todo: Split this up into 2 parts, everything before begin renderpass, and everything within the renderpass
	//the first part will take in the primary command buffer
	//the second will take in the secondary command buffer.
	void RenderTask::ProcessBatches(vk::CommandBuffer cmd_buffer)
	{
		if (!used)
			return;
		auto rp = curr_rp;
		auto fb = curr_frame_buffer;
		unsigned char block[4096];
		ArenaAllocator<unsigned char> alloc{ block };
		std::vector<vk::Viewport,ArenaAllocator<vk::Viewport>> viewports{alloc};
		std::vector<vk::Rect2D  ,ArenaAllocator<vk::Rect2D  >> scissors {alloc};
		vector<RscHandle<ShaderProgram>> condensed_shaders(std::size(batches.front().shaders.shaders));
		std::optional<RenderPassObj> prev_rp;
		
		const vector<vk::DescriptorSet>& uniform_sets = _descriptor_sets;

			VulkanPipeline::Options opt{};
			opt.dynamic_states.emplace_back(vk::DynamicState::eScissor);
			//ProcBatchLut()["PreBatch"]+= count_duration{ 1, timer.lap() };
			std::array<std::pair<vk::Buffer, size_t>, 32> vb_n_offset{};
			if (batches.size())
			{
				for (auto& batch : this->batches)
				{
					if (batch.label)
					{
						dbg::BeginLabel(cmd_buffer, batch.label->c_str());
					}
					if (batch.draw_calls.size() > 0)
					{
						condensed_shaders.clear();
						for (auto& oshader : batch.shaders.shaders)
						{
							if (oshader)
							{
								condensed_shaders.emplace_back(*oshader);
							}
						}
						auto& pipeline = (batch.pipeline_override) ? *batch.pipeline_override : ppm->GetPipeline(batch.pipeline, condensed_shaders, 0, rp, false, opt);
						//ProcBatchLut()["Pipeline Get"] += count_duration{ 1, timer.lap() };
						pipeline.Bind(cmd_buffer, View());
						viewports.resize(batch.viewport.size());
						scissors.resize(batch.scissor.size());
						auto bviewports = batch.viewport.to_span();
						auto bscissors = batch.scissor.to_span();

						std::transform(bviewports.begin(), bviewports.end(), viewports.begin(), [](rect r) {return vk::Viewport{ r.position.x,r.position.y, r.size.x,r.size.y,0,1 }; });
						std::transform(bscissors.begin(), bscissors.end(), scissors.begin(), [](rect r) {return hlp::ToRect2D(r); });
						cmd_buffer.setViewport(0, viewports);
						cmd_buffer.setScissor(0, scissors);
						//ProcBatchLut()["Set Viewport scissors"] += count_duration{ 1, timer.lap() };
						for (auto& p_ro : batch.draw_calls)
						{

							if (p_ro.label.first == LabelType::eInsert)
							{
								dbg::BeginLabel(cmd_buffer, batch.label->c_str());
							}
							auto set_bindings = p_ro.uniforms.to_span();
							auto dses = p_ro.uniforms.to_span(uniform_sets);
							for (size_t i = 0; i < set_bindings.size(); ++i)
							{
								auto ds = dses[i];
								auto [set, bindings] = set_bindings[i];
								cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), set, ds, {});
							}
							//ProcBatchLut()["Bind DS"] += count_duration{ 1, timer.lap() };

							for (auto& vb : p_ro.vertex_buffers)
							{
								//auto opt_binding = pipeline.GetBinding(vb.binding);
								//if (opt_binding)
								{
									if (!vb.buffer) //Don't bind null buffers.
										continue;
									auto& [prev_buffer, prev_offset] = vb_n_offset[vb.binding];
									if (prev_buffer != vb.buffer || prev_offset != vb.offset)
									{
										cmd_buffer.bindVertexBuffers(vb.binding, vb.buffer, vk::DeviceSize{ vb.offset }, vk::DispatchLoaderDefault{});
										prev_buffer = vb.buffer;
										prev_offset = vb.offset;
									}
								}
								//ProcBatchLut()["Bind Individual VB"] += count_duration{ 1, timer.lap() };
							}

							//std::visit(DrawFunc{}, p_ro.draw_info, std::variant<vk::CommandBuffer>{cmd_buffer}, std::variant<IndexBindingData>{ p_ro.index_buffer});
							switch (p_ro.draw_info.index())
							{
							case index_in_variant_v<indexed_draw_info, decltype(p_ro.draw_info)>:
							{
								auto& idx = p_ro.index_buffer;
								auto& di = std::get< indexed_draw_info>(p_ro.draw_info);
								cmd_buffer.bindIndexBuffer(idx.buffer, idx.offset, idx.type);
								//ProcBatchLut()["Bind Idx"] += count_duration{ 1, timer.lap() };
								cmd_buffer.drawIndexed(di.num_indices, di.num_instances, di.first_index, di.first_vertex, di.first_instance);
								//ProcBatchLut()["Draw Idx"] += count_duration{ 1, timer.lap() };
							}
							break;
							case index_in_variant_v<vertex_draw_info, decltype(p_ro.draw_info)>:
							{
								auto& di = std::get< vertex_draw_info>(p_ro.draw_info);
								cmd_buffer.draw(di.num_vertices, di.num_instances, di.first_vertex, di.first_instance);
								//ProcBatchLut()["Draw"] += count_duration{ 1, timer.lap() };
								break;
							}
							default:
								break;
							}

							if (p_ro.label.first == LabelType::eInsert)
							{
								dbg::EndLabel(cmd_buffer);
							}
							//ProcBatchLut()["inner extras"] += count_duration{ 1, timer.lap() };
						}

					}
					if (batch.label)
					{
						dbg::EndLabel(cmd_buffer);
					}
					//ProcBatchLut()["extras"] += count_duration{ 1, timer.lap() };
				}
			}
			//ProcBatchLut()["Batch"] += count_duration{ 1, timer.lap() };

		if (_label)
		{
			dbg::EndLabel(cmd_buffer);
		}
	}

	bool RenderTask::BeginSecondaryCmdBuffer(vk::CommandBuffer cmd_buffer)
	{
		if (!used || _skip_render_pass)
			return false;
		_start_new_batch = false;
		StartNewBatch();//flush the current batch
		auto rp = curr_rp;
		auto fb = curr_frame_buffer;
		uint32_t subpass = 0;
		vk::CommandBufferInheritanceInfo info
		{
			rp,
			subpass,
			fb,
		};
		cmd_buffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit|vk::CommandBufferUsageFlagBits::eRenderPassContinue,&info });
		return true;
	}
	bool RenderTask::BeginRenderPass(vk::CommandBuffer cmd_buffer)
	{
		if (ProcBatchLut().empty())
			test = 0.0f;
		dbg::stopwatch timer;
		timer.start();
		if (!used )
			return false;
		//AddToBatch(_current_batch);
		if (_label)
		{
			dbg::BeginLabel(cmd_buffer, _label->c_str());
			dbg::BeginLabel(cmd_buffer, "BeginCopyStage");
		}
		ProcessCopies(cmd_buffer);
		if (_label)
			dbg::EndLabel(cmd_buffer);

		//ProcBatchLut()["Proc Copies"] += count_duration{ 1, timer.lap() };

		if (_skip_render_pass)
			return false;


			vector<vk::ClearValue> clear_values;
			compute_clear_info(_num_output_attachments, clear_colors, _clear_depth_stencil, clear_values);

			auto rp = curr_rp;
			auto fb = curr_frame_buffer;
			auto ra_max = vec2{ std::numeric_limits<float>::min() };
			auto ra_min = vec2{ std::numeric_limits<float>::max() };
			for (auto& batch : batches)
			{
				for (auto& sc : batch.scissor.to_span())
				{
					auto vmin = sc.position;
					auto vmax = vmin + sc.size;
					ra_max = max(ra_max, vmax);
					ra_min = min(ra_min, vmin);
				}
			}
			render_area = { ra_min,ra_max - ra_min };

			vk::RenderPassBeginInfo rpbi
			{
				rp,fb,
				hlp::ToRect2D(render_area),
				static_cast<uint32_t>(clear_values.size()),
				std::data(clear_values),
			};
			cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eSecondaryCommandBuffers);

		return true;
	}

	template<typename Vec>
	void ClearSwap(Vec& dst, Vec& src) noexcept
	{
		src.clear();
		std::swap(dst, src);
	}
	void RenderTask::Reset()
	{
		if (!used)
			return;
		GetGfxTimeLog().start("reset uniform_manager & vtx_binding");
		//_uniform_manager.Reset();
		//_vtx_binding_tracker.Reset();
		//GetGfxTimeLog().end_then_start("tmp render task");
		//RenderTask tmp;
		//GetGfxTimeLog().end_then_start("swap");
		//std::swap(tmp, *this);
		GetGfxTimeLog().end_then_start("reset");

		
		this->attachment_offset = 0;
		this->batches.clear();
		this->clear_colors.clear();
		this->clear_depths.reset();
		this->clear_stencil.reset();
		this->curr_frame_buffer = nullptr;
		this->curr_rp.reset();
		this->ppm = nullptr;
		this->used = false;
		GetGfxTimeLog().end_then_start("reset 2");
		this->_clear_depth_stencil.reset();
		this->_copy_commands.clear();
		this->_current_batch = {};
		this->_dc_bindings._vertex_bindings.clear();
		this->_descriptor_sets.clear();
		this->_input_attachments = {};
		this->_label = {};
		this->_num_output_attachments = {};
		this->_rect_buffer->clear();
		this->_skip_render_pass = false;
		this->_start_new_batch = true;
		GetGfxTimeLog().end_then_start("UM reset");
		this->_uniform_manager.Reset();
		GetGfxTimeLog().end_then_start("reset 3");
		this->_uniform_sets.clear();
		GetGfxTimeLog().end_then_start("vtx binding");
		this->_vtx_binding_tracker.Reset();
		//*/
		//
		//
		//
		//
		//
		/*
		std::swap(_rect_buffer, tmp._rect_buffer);
		_rect_buffer->clear();
		//ClearSwap(_rect_buffer        ,tmp._rect_buffer);
		ClearSwap(batches             ,tmp.batches);
		ClearSwap(_copy_commands      ,tmp._copy_commands);
		std::swap(_uniform_manager    , tmp._uniform_manager);
		std::swap(this->_dc_bindings,tmp._dc_bindings);
		_dc_bindings._vertex_bindings.clear();
		_dc_bindings._dc_builder.clear();
		//std::swap(this->_dc_bindings._dc_builder   ,tmp._dc_bindings._dc_builder);
		std::swap(this->_rect_builder ,tmp._rect_builder);
		ClearSwap(_descriptor_sets    ,tmp._descriptor_sets);
		ClearSwap(_uniform_sets       ,tmp._uniform_sets);
		//ClearSwap(_dc_bindings._vertex_bindings    ,tmp._dc_bindings._vertex_bindings);
		std::swap(_vtx_binding_tracker,tmp._vtx_binding_tracker);
		*/
		GetGfxTimeLog().end();
	}
	void RenderTask::FlagUsed()
	{
		used = true;
	}
	void RenderTask::BindInputAttachmentToCurrent()
	{
		span<VknTextureView> input_attachments = _input_attachments;

		//TODO Bind Input attachments for the "current" batch
		auto shader = _current_batch.shaders.shaders[static_cast<size_t>(ShaderStage::Fragment)];
		auto itr = shader->as<ShaderModule>().InfoBegin();
		auto end = shader->as<ShaderModule>().InfoEnd();
		while (itr != end)
		{
			auto& info = itr->second;
			auto input_att_index = info.input_attachment_index + attachment_offset;
			if (info.type == uniform_layout_t::UniformType::eAttachment && input_att_index <input_attachments.size())
			{
				auto texture = input_attachments[input_att_index];
				if (texture.Image().operator VkImage() == (VkImage)0x3684)
					DoNothing();
				_uniform_manager.BindAttachment(UniformManager::UniInfo{ info.set,info.binding,info.size,info.layout }, 0, texture);
			}
			++itr;
		}
	}
	void RenderTask::AddToBatch(const DrawCall& draw_call)
	{
		_current_batch.draw_calls.emplace_back(draw_call);
		_start_new_batch = false;
	}
	void RenderTask::StartNewBatch(bool start)
	{
		//If we are starting a new batch
		if (!_start_new_batch && start)
		{
			//Retire the current batch
			_current_batch.pipeline.buffer_descriptions = _vtx_binding_tracker.GetDescriptors();
			batches.emplace_back(_current_batch);
			_current_batch.draw_calls.clear();
			_current_batch.label.reset();
		}
		_start_new_batch = start;
	}

	void RenderTask::ProcessCopies(RenderBundle& render_bundle)
	{
		ProcessCopies(render_bundle._cmd_buffer);

	}
	void RenderTask::ProcessCopies(vk::CommandBuffer cmd_buffer)
	{
		for (auto& copy_cmd : _copy_commands)
		{
			auto src_img = copy_cmd.src.Image();
			auto dst_img = copy_cmd.dst.Image();
			dbg_chk(src_img);
			dbg_chk(dst_img);
			//Transition from their original layouts to eGeneral
			auto target_src_format = vk::ImageLayout::eTransferSrcOptimal;
			auto target_dst_format = vk::ImageLayout::eTransferDstOptimal;
			if (copy_cmd.src_layout != target_src_format)
			{
				auto range = copy_cmd.src_range;
				if (!range)
					range = copy_cmd.src.FullRange();
				hlp::TransitionImageLayout(cmd_buffer, {}, src_img, copy_cmd.src.Format(), copy_cmd.src_layout, target_src_format, hlp::TransitionOptions{ {},{},range });
			}
			if (copy_cmd.dst_layout != target_dst_format)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.dst_range;
				if (!range)
					range = copy_cmd.dst.FullRange();
				hlp::TransitionImageLayout(cmd_buffer, {}, dst_img, copy_cmd.dst.Format(), vk::ImageLayout::eUndefined, target_dst_format, hlp::TransitionOptions{ {},{},range });
			}
			for (auto& region : copy_cmd.regions)
			{
				auto derp = uvec2{ region.dstOffset.x,region.dstOffset.y } +uvec2{ region.extent.width,region.extent.height };
				derp = min(derp, copy_cmd.dst.Size());
				region.extent.width = derp.x - region.dstOffset.x;
				region.extent.height = derp.y - region.dstOffset.y;
			}
			cmd_buffer.copyImage(copy_cmd.src.Image(), target_src_format, copy_cmd.dst.Image(), target_dst_format, copy_cmd.regions);
			//Transition from their eGeneral to original layouts 
			if (copy_cmd.src_layout != target_src_format && copy_cmd.src_layout != vk::ImageLayout::eUndefined)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.src_range;
				if (!range)
					range = copy_cmd.src.FullRange();
				dbg::BeginLabel(cmd_buffer, "DERP");
				hlp::TransitionImageLayout(cmd_buffer, {}, src_img, copy_cmd.src.Format(), target_src_format, copy_cmd.src_layout, hlp::TransitionOptions{ {},{},range });
				dbg::EndLabel(cmd_buffer);
			}
			if (copy_cmd.dst_layout != target_src_format)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.dst_range;
				if (!range)
					range = copy_cmd.dst.FullRange();
				dbg::BeginLabel(cmd_buffer, "HERP");
				hlp::TransitionImageLayout(cmd_buffer, {}, dst_img, copy_cmd.dst.Format(), vk::ImageLayout::eTransferDstOptimal, copy_cmd.dst_layout, hlp::TransitionOptions{ {},{},range });
				dbg::EndLabel(cmd_buffer);
			}
		}
	}
	RenderTask::DrawCallBuilder::DrawCallBuilder(vector<VertexBindingData>& vtx) : _vb_builder{vtx}
	{
	}
	void RenderTask::DrawCallBuilder::set_bindings(vector<VertexBindingData>& bindings)
	{
		_vb_builder.set(bindings);
	}
	void RenderTask::DrawCallBuilder::SetLabel(LabelType type, string str)
	{
		this->current_draw_call.label = { type,std::move(str) };
	}
	void RenderTask::DrawCallBuilder::AddVertexBuffer(VertexBindingData data)
	{
		_vertex_bindings[data.binding]= (std::move(data));
	}
	void RenderTask::DrawCallBuilder::SetIndexBuffer(IndexBindingData data)
	{
		this->current_draw_call.index_buffer = std::move(data);
	}
	RenderTask::DrawCall RenderTask::DrawCallBuilder::end(draw_info di, vector_span<UniformManager::set_binding_t> uniforms)
	{
		current_draw_call.draw_info = std::move(di);
		_vb_builder.grow_reserve(_vertex_bindings.size());
		_vb_builder.start();
		for (auto& binding : _vertex_bindings)
		{
			_vb_builder.emplace_back(binding);
		}
		current_draw_call.vertex_buffers = _vb_builder.end();
		current_draw_call.uniforms = uniforms;
		RenderTask::DrawCall result = current_draw_call;
		current_draw_call.label.first = {};
		current_draw_call.label.second.clear();
		return result;
	}
	void RenderTask::DrawFunc::operator()(indexed_draw_info di, vk::CommandBuffer cmd_buffer ,IndexBindingData idx)
	{
		cmd_buffer.bindIndexBuffer(idx.buffer, idx.offset, idx.type);
		cmd_buffer.drawIndexed(di.num_indices,di.num_instances,di.first_index,di.first_vertex,di.first_instance);
	}
	void RenderTask::DrawFunc::operator()(vertex_draw_info di, vk::CommandBuffer cmd_buffer, IndexBindingData)
	{
		cmd_buffer.draw(di.num_vertices, di.num_instances, di.first_vertex, di.first_instance);
	}

}

namespace idk::vkn::dbg
{
	struct SubDurations
	{
		hash_table<string_view, float> durations{};
		void clear()
		{
			for (auto& [name, duration] : durations)
			{
				name;
				duration = 0;
			}
		}
		void add(string_view name, float duration)
		{
			durations[name] += duration;
		}
	};
hash_table<string_view, float>& get_rendertask_durations()
{
	thread_local static hash_table<string_view, float> durations;
	return durations;
}

void add_rendertask_durations(string_view name, float duration)
{
	return;
	get_rendertask_durations()[name]+=duration;
}

}