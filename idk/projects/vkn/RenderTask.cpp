#include "pch.h"
#include "RenderTask.h"

#include <vkn/ShaderModule.h>
#include <vkn/RenderBundle.h>
#include <vkn/utils/VknUtil.h>

#include <vkn/PipelineManager.h>
#include <res/ResourceHandle.inl>

#include <vkn/DebugUtil.h>

namespace idk::vkn
{
#pragma optimize("",off)
	RenderTask::RenderTask() :ppm{std::make_unique<PipelineManager>()}
	{
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
	void RenderTask::BindVertexBuffer(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset)
	{
		_dc_builder.AddVertexBuffer(VertexBindingData{vertex_buffer,binding,byte_offset});
	}
	void RenderTask::BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType)
	{
		_dc_builder.SetIndexBuffer(IndexBindingData{ buffer,offset,indexType});
	}
	void RenderTask::BindUniform(string_view name, uint32_t index, string_view data,bool skip_if_bound)
	{
		_uniform_manager.BindUniformBuffer(name, index, data,skip_if_bound);
	}
	void RenderTask::BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		_uniform_manager.BindSampler(name, index, texture, skip_if_bound,layout);
	}
	void RenderTask::BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader_handle)
	{
		auto& bound_shader = _current_batch.shaders.shaders[static_cast<size_t>(stage)];
		if (bound_shader == shader_handle)
			return;
		UnbindShader(stage);
		auto& shader = shader_handle.as<ShaderModule>();
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
		if(stage==ShaderStage::Fragment)
			BindInputAttachmentToCurrent();
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
	void RenderTask::SetRenderPass(RenderPassObj render_pass)
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
		AddToBatch(_dc_builder.end(di,uniforms));
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
		AddToBatch(_dc_builder.end(di, uniforms));
	}
	void RenderTask::SetBufferDescriptions(span<buffer_desc> descriptions)
	{
		StartNewBatch();
		_current_batch.pipeline.buffer_descriptions = {descriptions.begin(),descriptions.end()};
	}
	void RenderTask::SetBlend(uint32_t attachment_index, AttachmentBlendConfig blend_config)
	{
		auto& configs = _current_batch.pipeline.attachment_configs;
		uint32_t size = static_cast<uint32_t>(configs.size());
		configs.resize(std::max(size, attachment_index + 1));
		configs[attachment_index] = blend_config;
	}
	void RenderTask::SetInputAttachments(span<VknTextureView> input_attachments) noexcept
	{
		_input_attachments = input_attachments;
	}

	uint32_t compute_clear_info(span<const color> clear_colors, std::optional<float> clear_depth, std::optional<uint8_t> clear_stencil,
		vector<vk::ClearValue>& clear
		)
	{
		clear.clear();
		clear.reserve(clear_colors.size() + ((clear_depth|| clear_stencil) ? 1 : 0));
		for (auto color : clear_colors)
		{
			clear.emplace_back(std::array<float, 4> { color.r,color.g,color.b,color.a });
		}
		bool clear_other = false;
		float depth{};
		uint8_t stencil{};
		if (clear_depth)
		{
			clear_other = true;
			depth = *clear_depth;
		}
		if (clear_stencil)
		{
			clear_other = true;
			stencil = *clear_stencil;
		}
		if(clear_other)
			clear.emplace_back(vk::ClearDepthStencilValue{depth,stencil});
		return static_cast<uint32_t>(clear.size());
	}
	void RenderTask::ProcessBatches(RenderBundle& render_bundle)
	{
		//AddToBatch(_current_batch);
		StartNewBatch();//flush the current batch
		auto cmd_buffer = render_bundle._cmd_buffer;
		auto& d_manager = render_bundle._d_manager;
		vector<vk::DescriptorSet> uniform_sets(_uniform_sets.size());

		
		_uniform_manager.GenerateDescriptorSets(span{ _uniform_sets }, d_manager, uniform_sets);

		vector<vk::Viewport> viewports;
		std::optional<RenderPassObj> prev_rp;
		vector<vk::ClearValue> clear_values;
		compute_clear_info(clear_colors, clear_depths, clear_stencil,clear_values);
		vector<RscHandle<ShaderProgram>> condensed_shaders(std::size(batches.front().shaders.shaders));
		if (_label)
		{
			dbg::BeginLabel(cmd_buffer, _label->c_str());
		}
		if (batches.size())
		{

		auto rp = curr_rp;
		auto fb = curr_frame_buffer;
		auto ra_max = vec2{ std::numeric_limits<float>::min() };
		auto ra_min = vec2{ std::numeric_limits<float>::max() };
		for (auto& batch : batches)
		{
			for (auto& sc : batch.scissor.to_span())
			{
				auto vmin= sc.position;
				auto vmax= vmin + sc.size;
				ra_max = max(ra_max, vmax);
				ra_min = min(ra_min, vmin);
			}
		}
		render_area = {ra_min,ra_max-ra_min};

		vk::RenderPassBeginInfo rpbi
		{
			*rp,fb,
			hlp::ToRect2D(render_area),
			static_cast<uint32_t>(clear_values.size()),
			std::data(clear_values),
		};
		cmd_buffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);
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
				auto& pipeline =ppm->GetPipeline(batch.pipeline, condensed_shaders, 0, rp);
				pipeline.Bind(render_bundle._cmd_buffer, View());
				viewports.resize(batch.viewport.size());
				auto bviewports = batch.viewport.to_span();
				std::transform(bviewports.begin(), bviewports.end(), viewports.begin(), [](rect r) {return vk::Viewport{r.position.x,r.position.y, r.size.x,r.size.y,0,1 }; });
				cmd_buffer.setViewport(0,viewports);
				for (auto& p_ro : batch.draw_calls)
				{

					if (p_ro.label.first == LabelType::eInsert)
					{
						dbg::BeginLabel(cmd_buffer, batch.label->c_str());
					}
					auto set_bindings = p_ro.uniforms.to_span();
					auto dses = p_ro.uniforms.to_span(uniform_sets);
					for (size_t i=0;i<set_bindings.size();++i)
					{
						auto ds = dses[i];
						auto [set, bindings] = set_bindings[i];
						cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.PipelineLayout(), set, ds, {});
					}

					for (auto& vb : p_ro.vertex_buffers)
					{
						auto opt_binding = pipeline.GetBinding(vb.binding);
						if (opt_binding)
						{
							cmd_buffer.bindVertexBuffers(*opt_binding, vb.buffer, vk::DeviceSize{ vb.offset }, vk::DispatchLoaderDefault{});
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
				_uniform_manager.BindAttachment(UniformManager::UniInfo{ info.set,info.binding,info.size,info.layout }, 0, input_attachments[input_att_index]);
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
			batches.emplace_back(_current_batch);
			_current_batch.draw_calls.clear();
			_current_batch.label.reset();
		}
		_start_new_batch = start;
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
		_vb_builder.start();
		for (auto& binding : _vertex_bindings)
		{
			_vb_builder.emplace_back(binding);
		}
		current_draw_call.vertex_buffers = _vb_builder.end();
		current_draw_call.uniforms = uniforms;
		RenderTask::DrawCall result = current_draw_call;
		current_draw_call.label = {};
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