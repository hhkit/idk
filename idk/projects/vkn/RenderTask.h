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
#include <vkn/PipelineManager.h>

#include <vkn/RenderInterface.h>

#include <vkn/VertexBindingTracker.h>

#include <util/PoolContainer.h>

namespace idk::vkn
{
	

	struct UboManager;
	class PipelineManager;

	struct RenderBundle;// holds the stuff required to actually draw/submit

	using TextureID  = Guid;

	
	struct Shaders 
	{
		std::array< std::optional<RscHandle<ShaderProgram>>, static_cast<size_t>(ShaderStage::Size)> shaders;
	};

	struct AttachmentInfo
	{
		TextureID target;
		std::optional<AttachmentBlendConfig> config;
	};
	struct CopyCommand
	{
		VknTextureView src;
		vk::ImageLayout src_layout;
		std::optional<vk::ImageSubresourceRange> src_range;
		VknTextureView dst;
		vk::ImageLayout dst_layout;
		std::optional<vk::ImageSubresourceRange> dst_range;
		
		vector<vk::ImageCopy> regions;
	};

	struct RenderTask : RenderInterface
	{
		//hack
		int32_t attachment_offset = 0;
		enum class LabelType
		{
			eBegin,
			eInsert,
			eEnd,
		};
		enum class LabelLevel
		{
			eDrawCall,
			eBatch,
			eWhole,
		};

		RenderTask();//UniformManager& um);
		RenderTask(RenderTask&&) = default;//UniformManager& um);
		RenderTask& operator=(RenderTask&&)=default;//UniformManager& um);

		void DebugLabel(LabelLevel, string);

		//void Associate(size_t subpass_index);

		void SetUboManager(UboManager& ubo_manager);
		void SetPipelineManager(PipelineManager& pipeline_manager);


		void BindVertexBuffer(uint32_t location, VertexBuffer vertex_buffer, size_t byte_offset)override;
		void BindVertexBufferByBinding(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset)override;
		void BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType)override;

#pragma region Uniforms
		void BindDescriptorSet(uint32_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl) override;
		void BindUniform(string_view name, uint32_t index, string_view data, bool skip_if_bound = false)override;
		//void BindUniform(vk::DescriptorSet ds, std::optional<string_view> data = {})override;
		void BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound = false,vk::ImageLayout layout= vk::ImageLayout::eGeneral)override;
#pragma endregion

		void BindShader(ShaderStage stage,RscHandle<ShaderProgram> shader)override;
		void UnbindShader(ShaderStage shader_stage)override;
		void SetRenderPass(VknRenderPass render_pass);
		void SetFrameBuffer(const Framebuffer& fb,uvec2 size);

#pragma region Draw
		void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance)override;
		void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance)override;
#pragma endregion

		void Copy(const CopyCommand& copy);
		void Copy(CopyCommand&& copy);

#pragma region PipelineConfigurations
		//void Inherit(const pipeline_config& config);
		bool SetPipeline(const VulkanPipeline& pipeline) override;
		void SetPipelineConfig(const pipeline_config& config) override
		{
			StartNewBatch();
			_current_batch.pipeline = config;
		}
		void SetBufferDescriptions(span<const buffer_desc>)override;
		void SetBlend(uint32_t attachment_index, AttachmentBlendConfig blend_config = {})override;
		//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
		//If col is nullopt, we clear all the colors from attachment_index onwards.
		void SetClearColor(uint32_t attachment_index, std::optional<color> col)override;
		void SetClearDepthStencil(std::optional<float> depth, std::optional<uint8_t> stencil = {})override;
		void SetScissors(rect r)override;
		void SetViewport(rect r)override;
		void SetScissorsViewport(rect r)override;
		void SetFillType(FillType type)override;
		void SetCullFace(CullFaceFlags cf)override;
		void SetPrimitiveTopology(PrimitiveTopology pt)override;
		void SetDepthTest(bool enabled)override;
		void SetDepthWrite(bool enabled)override;
		void SetStencilTest(bool enabled)override;
		void SetStencilWrite(bool enabled)override;
#pragma endregion 
		const pipeline_config& GetCurrentConfig()const noexcept;

		
		void SkipRenderPass(bool skip);
		void SetInputAttachments(span<VknTextureView> input_attachments) noexcept;
		void SetOutputAttachmentSize(size_t size);
		void SetClearDepthStencil(std::optional<vk::ClearValue> clear_value = {});

		void PreprocessDescriptors(DescriptorUpdateData& dud, DescriptorsManager& dm);

		void ProcessBatches(RenderBundle& render_bundle);
		void ProcessBatches(vk::CommandBuffer cmd_buffer);

		bool BeginSecondaryCmdBuffer(vk::CommandBuffer cmd_buffer);
		bool BeginRenderPass(vk::CommandBuffer cmd_buffer);

		void Reset();

		void FlagUsed();

	public:
		struct DrawCall;

		void BindInputAttachmentToCurrent();


		void AddToBatch(const DrawCall& draw_call);
		void StartNewBatch(bool start = true);

		void ProcessCopies(RenderBundle& render_bundle);
		void ProcessCopies(vk::CommandBuffer render_bundle);

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
			std::pair<LabelType, string>               label;
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
			void set_bindings(vector<VertexBindingData>&);
			void SetLabel(LabelType, string);
			void AddVertexBuffer(VertexBindingData);
			void SetIndexBuffer(IndexBindingData);
			DrawCall end(draw_info, vector_span<UniformManager::set_binding_t> uniforms);
			void clear()
			{
				_vertex_bindings.clear();
				current_draw_call = {};
			}
		private:
			DrawCall current_draw_call;
			
			lazy_vector<VertexBindingData> _vertex_bindings;

			vector_span_builder<VertexBindingData> _vb_builder;
		};

		struct RenderBatch
		{
			pipeline_config pipeline;
			const VulkanPipeline* pipeline_override=nullptr;
			vector_span<rect> scissor{}, viewport{};
			//RenderPassObj render_pass;
			//vk::Framebuffer frame_buffer;
			Shaders shaders;
			PooledContainer<vector<DrawCall>> draw_calls;
			std::optional<string> label;
		};

		PipelineManager* ppm;

#pragma region Clear Info
		lazy_vector<color> clear_colors;
		std::optional<float> clear_depths;
		std::optional<uint8_t> clear_stencil;
		std::optional<vk::ClearValue> _clear_depth_stencil = {};
#pragma endregion Clear Info

#pragma region Pipeline State
		pipeline_config curr_config;

		std::unique_ptr<vector<rect>> _rect_buffer = std::make_unique<vector<rect>>();
		std::unique_ptr<vector_span_builder<rect>> _rect_builder{ std::make_unique< vector_span_builder<rect>>(vector_span_builder<rect>{*_rect_buffer}) };
		span<VknTextureView> _input_attachments;
		size_t _num_output_attachments;
		rect render_area;
#pragma endregion

		bool _skip_render_pass=false;
		VknRenderPass curr_rp;
		vk::Framebuffer curr_frame_buffer;
		uvec2 fb_size;

		bool _start_new_batch = true;

		RenderBatch _current_batch{};

		std::optional<string> _label;

		vector<UniformManager::set_binding_t> _uniform_sets;
		vector<vk::DescriptorSet> _descriptor_sets;
		UniformManager _uniform_manager;
		struct DCMoveWrapper
		{
			vector<VertexBindingData> _vertex_bindings;
			DrawCallBuilder _dc_builder{ _vertex_bindings };
			DCMoveWrapper() = default;
			DCMoveWrapper(DCMoveWrapper&& rhs);
			DCMoveWrapper&operator=(DCMoveWrapper&&rhs);
		};
		DCMoveWrapper _dc_bindings;
		//vector<VertexBindingData> _vertex_bindings;
		//DrawCallBuilder _dc_builder{_vertex_bindings};

		vector<RenderBatch> batches;

		vector<CopyCommand> _copy_commands;
		VertexBindingTracker _vtx_binding_tracker;
		bool used = false;
	};




	namespace dbg
	{
		hash_table<string_view, float>& get_rendertask_durations();
		void add_rendertask_durations(string_view name,float duration);

	}



}