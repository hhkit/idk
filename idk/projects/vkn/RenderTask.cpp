#include "pch.h"
#include "RenderTask.h"

#include <vkn/ShaderModule.h>
#include <vkn/RenderBundle.h>
#include <vkn/utils/VknUtil.h>

#include <vkn/PipelineManager.h>
#include <res/ResourceHandle.inl>

#include <vkn/DebugUtil.h>

//#pragma optimize("",off)
static void DoNothing()
{

}

void dbg_chk(vk::Image img)
{
	return;
}
//#pragma optimize("",on)

namespace idk::vkn
{

	RenderTask::RenderTask() //:ppm{std::make_unique<PipelineManager>()}
	{
		batches.reserve(256);
	}
	void RenderTask::DebugLabel(LabelLevel type, string label)
	{
		switch (type)
		{
		case idk::vkn::RenderTask::LabelLevel::eDrawCall:
			this->_dc_builder.SetLabel(LabelType::eInsert, std::move(label));
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


//#pragma optimize("",off)

	void RenderTask::BindVertexBuffer(uint32_t location, VertexBuffer vertex_buffer, size_t byte_offset)
	{
		auto binding = _vtx_binding_tracker.GetBinding(location);
		if (binding)
		{
			if (!vertex_buffer)
				DoNothing();
			_dc_builder.AddVertexBuffer(VertexBindingData{ vertex_buffer,*binding,byte_offset });
		}
		else
		{

			DoNothing();
		}
	}
	void RenderTask::BindVertexBufferByBinding(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset)
	{
		_dc_builder.AddVertexBuffer(VertexBindingData{ vertex_buffer,binding,byte_offset });
	}
	void RenderTask::BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType)
	{
		_dc_builder.SetIndexBuffer(IndexBindingData{ buffer,offset,indexType});
	}
	void RenderTask::BindDescriptorSet(uint32_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl)
	{
		_uniform_manager.BindDescriptorSet(set,ds,dsl);
	}
	void RenderTask::BindUniform(string_view name, uint32_t index, string_view data,bool skip_if_bound)
	{
		_uniform_manager.BindUniformBuffer(name, index, data,skip_if_bound);
	}
	//void RenderTask::BindUniform(vk::DescriptorSet ds, std::optional<string_view> data)
	//{
	//	_uniform_manager.BindUniformBuffer(ds,data);
	//}
//#pragma optimize("",off)
	static void DoNothing() {}
	void RenderTask::BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		dbg_chk(texture.Image());
		_uniform_manager.BindSampler(name, index, texture, skip_if_bound,layout);
	}
	void RenderTask::BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader_handle)
	{
		auto& bound_shader = _current_batch.shaders.shaders[static_cast<size_t>(stage)];
		if (bound_shader == shader_handle || shader_handle.guid == Guid{})
			return;
		UnbindShader(stage);
		auto& shader = shader_handle.as<ShaderModule>();
		if (!shader.HasCurrent())
			return;
		//DebugBreak();
		for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
		{
			_uniform_manager.AddBinding(itr->first,*itr->second.layout,itr->second.entry_counts);
		}
		for (auto itr = shader.InfoBegin(), end = shader.InfoEnd(); itr != end; ++itr)
		{
			auto [name, info] = *itr;
			_uniform_manager.RegisterUniforms(name, info.set, info.binding, info.size);
		}
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

			for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
			{
				_uniform_manager.RemoveBinding(itr->first);
			}
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
			AddToBatch(_dc_builder.end(di, *uniforms));
	}
	void RenderTask::DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance)
	{
		indexed_draw_info di{};
		di.first_instance = first_instance;
		di.num_instances = num_instances;
		di.num_indices = num_indices;
		di.first_index = first_index;
		di.first_vertex = first_vertex;
		auto uniforms = _uniform_manager.FinalizeCurrent(_uniform_sets);
		if(uniforms)
			AddToBatch(_dc_builder.end(di, *uniforms));
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
		_rect_builder.start();
		_rect_builder.emplace_back(r);
		_current_batch.scissor = _rect_builder.end();
	}
	void RenderTask::SetViewport(rect r)
	{
		StartNewBatch();
		r = ViewportScissors(r, fb_size);
		_rect_builder.start();
		_rect_builder.emplace_back(r);
		_current_batch.viewport = _rect_builder.end();
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
	void RenderTask::ProcessBatches(RenderBundle& render_bundle)
	{
		//AddToBatch(_current_batch);
		_start_new_batch = false;
		StartNewBatch();//flush the current batch
		auto cmd_buffer = render_bundle._cmd_buffer;
		if (_label)
		{
			dbg::BeginLabel(cmd_buffer, _label->c_str());
			dbg::BeginLabel(cmd_buffer, "BeginCopyStage");
		}
		ProcessCopies(render_bundle);
		if (_label)
			dbg::EndLabel(cmd_buffer);
		
		if (!_skip_render_pass)
		{


			auto& d_manager = render_bundle._d_manager;
			vector<vk::DescriptorSet> uniform_sets(_uniform_sets.size());


			_uniform_manager.GenerateDescriptorSets(span{ _uniform_sets }, d_manager, uniform_sets);

			vector<vk::Viewport> viewports;
			vector<vk::Rect2D> scissors;
			std::optional<RenderPassObj> prev_rp;
			vector<vk::ClearValue> clear_values;
			compute_clear_info(_num_output_attachments, clear_colors, _clear_depth_stencil, clear_values);
			vector<RscHandle<ShaderProgram>> condensed_shaders(std::size(batches.front().shaders.shaders));

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
			cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);

			VulkanPipeline::Options opt{};
			opt.dynamic_states.emplace_back(vk::DynamicState::eScissor);
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
						auto& pipeline = (batch.pipeline_override) ? *batch.pipeline_override : ppm->GetPipeline(batch.pipeline, condensed_shaders, 0, rp,false, opt);
						pipeline.Bind(render_bundle._cmd_buffer, View());
						viewports.resize(batch.viewport.size());
						scissors.resize(batch.scissor.size());
						auto bviewports = batch.viewport.to_span();
						auto bscissors = batch.scissor.to_span();
						
						std::transform(bviewports.begin(), bviewports.end(), viewports.begin(), [](rect r) {return vk::Viewport{ r.position.x,r.position.y, r.size.x,r.size.y,0,1 }; });
						std::transform(bscissors.begin(), bscissors.end(), scissors.begin(), [](rect r) {return hlp::ToRect2D(r); });
						cmd_buffer.setViewport(0, viewports);
						cmd_buffer.setScissor(0, scissors);
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

							for (auto& vb : p_ro.vertex_buffers)
							{
								//auto opt_binding = pipeline.GetBinding(vb.binding);
								//if (opt_binding)
								{
									if (!vb.buffer) //Don't bind null buffers.
										continue;
									cmd_buffer.bindVertexBuffers(vb.binding, vb.buffer, vk::DeviceSize{ vb.offset }, vk::DispatchLoaderDefault{});
								}
							}

							std::visit(DrawFunc{}, p_ro.draw_info, std::variant<vk::CommandBuffer>{cmd_buffer}, std::variant<IndexBindingData>{ p_ro.index_buffer});

							if (p_ro.label.first == LabelType::eInsert)
							{
								dbg::EndLabel(cmd_buffer);
							}
						}

					}
					if (batch.label)
					{
						dbg::EndLabel(cmd_buffer);
					}
				}
			}

			cmd_buffer.endRenderPass();
		}
		if (_label)
		{
			dbg::EndLabel(cmd_buffer);
		}
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
		auto cmd_buffer = render_bundle._cmd_buffer;
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
				hlp::TransitionImageLayout(cmd_buffer, {}, src_img, copy_cmd.src.Format(), copy_cmd.src_layout, target_src_format, hlp::TransitionOptions{ {},{},range});
			}
			if (copy_cmd.dst_layout != target_dst_format)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.dst_range;
				if (!range)
					range = copy_cmd.dst.FullRange();
				hlp::TransitionImageLayout(cmd_buffer, {}, dst_img, copy_cmd.dst.Format(), vk::ImageLayout::eUndefined, target_dst_format , hlp::TransitionOptions{ {},{},range });
			}
			cmd_buffer.copyImage(copy_cmd.src.Image(), target_src_format, copy_cmd.dst.Image(), target_dst_format, copy_cmd.regions);
			//Transition from their eGeneral to original layouts 
			if (copy_cmd.src_layout != target_src_format && copy_cmd.src_layout != vk::ImageLayout::eUndefined)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.src_range;
				if (!range)
					range = copy_cmd.src.FullRange();
				hlp::TransitionImageLayout(cmd_buffer, {}, src_img, copy_cmd.src.Format(), target_src_format, copy_cmd.src_layout, hlp::TransitionOptions{ {},{},range });
			}
			if (copy_cmd.dst_layout != target_src_format)
			{
				//cmd_buffer.pipelineBarrier();
				auto range = copy_cmd.dst_range;
				if (!range)
					range = copy_cmd.dst.FullRange();
				hlp::TransitionImageLayout(cmd_buffer, {}, dst_img, copy_cmd.dst.Format(), vk::ImageLayout::eTransferDstOptimal, copy_cmd.dst_layout, hlp::TransitionOptions{ {},{},range});
			}
		}

	}
	RenderTask::DrawCallBuilder::DrawCallBuilder(vector<VertexBindingData>& vtx) : _vb_builder{vtx}
	{
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