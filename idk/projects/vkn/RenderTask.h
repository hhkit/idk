#pragma once
#include <cstdint>
#include <math/rect.h>
#include <gfx/pipeline_config.h>
#include <vkn/utils/Flags.h>
#include <ds/ranged_for.inl>
#include <ds/index_span.h>
#include <stack>
#include <queue>
#include <vkn/topological_sort.h>
#include <gfx/Texture.h>
#include <vkn/RenderPassObj.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknTextureView.h>
#include <ds/lazy_vector.h>
#include <vkn/vector_span_builder.h>

#include <vkn/UniformManager.h>

namespace idk::vkn
{
	using VknRenderPass = RenderPassObj;


	struct UboManager;

	struct RenderBundle;// holds the stuff required to actually draw/submit

	enum LoadOp {};
	enum StoreOp {};
	enum class IndexType { e16, e32 };
	using Framebuffer = vk::Framebuffer;
	using VertexBuffer = vk::Buffer;
	using IndexBuffer = vk::Buffer;
	class ShaderModule;
	using TextureID  = Guid;

	struct indexed_draw_info
	{
		uint32_t num_indices;
		uint32_t num_instances;
		uint32_t first_vertex;
		uint32_t first_index;
		uint32_t first_instance;
	};
	struct vertex_draw_info
	{
		uint32_t num_vertices;
		uint32_t num_instances;
		uint32_t first_vertex;
		uint32_t first_instance;
	};

	using draw_info = std::variant<indexed_draw_info, vertex_draw_info>;


	struct Shaders 
	{
		std::array< std::optional<RscHandle<ShaderProgram>>, static_cast<size_t>(ShaderStage::Size)> shaders;
	};

	struct AttachmentInfo
	{
		TextureID target;
		std::optional<AttachmentBlendConfig> config;
	};


	struct RenderTask
	{
		//void Associate(size_t subpass_index);

		void SetUboManager(UboManager& ubo_manager);


		void BindVertexBuffer(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset);
		void BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType);

#pragma region Uniforms
		void BindUniform(string_view name, uint32_t index, string_view data,bool skip_if_bound=false);
		void BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound = false,vk::ImageLayout layout= vk::ImageLayout::eGeneral);
#pragma endregion

		void BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader);
		void UnbindShader(ShaderStage shader_stage);
		void SetRenderPass(RenderPassObj render_pass);
		void SetFrameBuffer(const Framebuffer& fb);

#pragma region Draw
		void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance);
		void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance);
#pragma endregion

#pragma region PipelineConfigurations
		//void Inherit(const pipeline_config& config);
		void SetPipelineConfig(const pipeline_config& config)
		{
			_current_batch.pipeline = config;
		}
		void SetBufferDescriptions();
		//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
		//If col is nullopt, we clear all the colors from attachment_index onwards.
		void SetClearColor(uint32_t attachment_index, std::optional<color> col)
		{
			if (col)
				clear_colors[attachment_index] = *col;
			else
				clear_colors.resize(attachment_index);
		}
		void SetClearDepthStencil(std::optional<float> depth, std::optional<uint8_t> stencil = {})
		{
			clear_depths = depth;
			clear_stencil = stencil;
		}
		void SetScissors(rect r)
		{
			StartNewBatch();
			_rect_builder.start();
			_rect_builder.emplace_back(r);
			_current_batch.scissor =_rect_builder.end();
		}
		void SetViewport(rect r)
		{
			StartNewBatch();
			_rect_builder.start();
			_rect_builder.emplace_back(r);
			_current_batch.viewport = _rect_builder.end();
		}
		void SetFillType(FillType type)
		{
			StartNewBatch();
			_current_batch.pipeline.fill_type = type;
		}
		void SetCullFace(CullFaceFlags cf)
		{
			StartNewBatch();
			_current_batch.pipeline.cull_face = cf;
		}
		void SetPrimitiveTopology(PrimitiveTopology pt)
		{
			StartNewBatch();
			_current_batch.pipeline.prim_top= pt;
		}
		void SetDepthTest(bool enabled)
		{
			StartNewBatch();
			_current_batch.pipeline.depth_test= enabled;
		}
		void SetDepthWrite(bool enabled){
			StartNewBatch();
			_current_batch.pipeline.depth_write = enabled;
		}
		void SetStencilTest(bool enabled)
		{
			StartNewBatch();
			_current_batch.pipeline.stencil_test= enabled;
		}
		void SetStencilWrite(bool enabled)
		{
			StartNewBatch();
			_current_batch.pipeline.stencil_write= enabled;
		}
#pragma endregion 
		const pipeline_config GetCurrentConfig()const noexcept
		{
			return _current_batch.pipeline;
		}

		
		void SetInputAttachments(span<VknTextureView> input_attachments) noexcept;

		void ProcessBatches(RenderBundle& render_bundle);

	private:
		struct DrawCall;

		void BindInputAttachmentToCurrent();


		void AddToBatch(const DrawCall& draw_call);
		void StartNewBatch(bool start = true);

		struct VertexBindingData
		{
			//vk::Buffer buffer;
			VertexBuffer buffer;
			uint32_t binding;
			size_t offset;
		};
		struct IndexBindingData
		{
			//vk::Buffer buffer;
			IndexBuffer buffer;
			size_t offset;
			IndexType type;
		};
		struct DrawCall
		{
			//Use spans so that this is flat and we don't pay for multiple allocations (versus vector)
			//Use indices so that even if we transform the buffers, as long as the order doesn't change, our indices remain correct.
			vector_span<UniformManager::set_binding_t> uniforms;
			vector_span <VertexBindingData>            vertex_buffers;
			IndexBindingData                           index_buffer;

			draw_info draw_info; //might need to split into two types/vectors of draw calls to prevent branching
		};

		struct DrawFunc
		{
			void operator()(indexed_draw_info di,vk::CommandBuffer, IndexBindingData);
			void operator()(vertex_draw_info di, vk::CommandBuffer, IndexBindingData);
		};
		struct DrawCallBuilder
		{
			using uniform_t = UniformManager::set_binding_t;
			DrawCallBuilder( vector<VertexBindingData>&);
			void AddVertexBuffer(VertexBindingData);
			void SetIndexBuffer(IndexBindingData);
			DrawCall end(draw_info, vector_span<UniformManager::set_binding_t> uniforms);
		private:
			DrawCall current_draw_call;
			
			lazy_vector<VertexBindingData> _vertex_bindings;

			vector_span_builder<VertexBindingData> _vb_builder;
		};

		struct RenderBatch
		{
			pipeline_config pipeline;
			vector_span<rect> scissor, viewport;
			rect render_area;
			RenderPassObj render_pass;
			vk::Framebuffer frame_buffer;
			Shaders shaders;
			vector<DrawCall> draw_calls;
		};


#pragma region Clear Info
		lazy_vector<color> clear_colors;
		std::optional<float> clear_depths;
		std::optional<uint8_t> clear_stencil;
#pragma endregion Clear Info

#pragma region Pipeline State
		pipeline_config curr_config;
		vector<rect> _rect_buffer;
		vector_span_builder<rect> _rect_builder{ _rect_buffer };
		span<VknTextureView> _input_attachments;
#pragma endregion

		RenderPassObj curr_rp;
		vk::Framebuffer curr_frame_buffer;

		bool _start_new_batch = true;

		RenderBatch _current_batch{};


		vector<UniformManager::set_binding_t> _uniform_sets;
		vector<VertexBindingData> _vertex_bindings;
		UniformManager _uniform_manager;
		DrawCallBuilder _dc_builder{_vertex_bindings};

		vector<RenderBatch> batches;
	};








}