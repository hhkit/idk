#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/UboManager.h>
#include <vkn/VulkanPipeline.h>
#include <gfx/pipeline_config.h>
namespace idk
{
	struct RenderObject;
}
namespace idk::vkn
{
	struct GraphicsState;
	struct ProcessedRO
	{
		struct BindingInfo
		{
			uint32_t binding;
			vk::Buffer ubuffer;
			uint32_t buffer_offset;
			uint32_t arr_index;
			size_t size;
			BindingInfo(
				uint32_t binding_,
				vk::Buffer& ubuffer_,
				uint32_t buffer_offset_,
				uint32_t arr_index_,
				size_t size_
			) :
				binding{ binding_ },
				ubuffer{ ubuffer_ },
				buffer_offset{ buffer_offset_ },
				arr_index{ arr_index_ },
				size{ size_ }
			{
			}
		};
		const RenderObject& Object()const
		{
			return *itr;
		}

		//set, update_instr
		const RenderObject* itr;
		hash_table<uint32_t, vector<BindingInfo>> bindings;
		shared_ptr<pipeline_config> config;
	};
	class PipelineManager;
	struct RenderStateV2
	{
		vk::CommandBuffer cmd_buffer;
		UboManager ubo_manager;//Should belong to each thread group.

		PresentationSignals signal;
		DescriptorsManager dpools;

		bool has_commands = false;
		void FlagRendered() { has_commands = true; }
		void Reset();
		RenderStateV2() = default;
		RenderStateV2(const RenderStateV2&) = delete;
		RenderStateV2(RenderStateV2&&) = default;
	};
	using PipelineHandle_t =uint32_t;
	struct RenderFrameObject
	{
	};
	class FrameRenderer
	{
	public:
		
		void Init(VulkanView* view, vk::CommandPool cmd_pool);

		void SetPipelineManager(PipelineManager& manager);
		void RenderGraphicsStates(const vector<GraphicsState>& state);
		PresentationSignals& GetMainSignal();
	private:
		using ProcessedRO=vkn::ProcessedRO;
		using DsBindingCount =hash_table<vk::DescriptorSetLayout, uint32_t>;
		class IRenderThread
		{
		public:
			IRenderThread() = default;
			IRenderThread(const IRenderThread&) = delete;
			IRenderThread(IRenderThread&&) = default;
			IRenderThread&operator=(const IRenderThread&) = delete;
			IRenderThread&operator=(IRenderThread&&) = default;
			virtual void Render(const GraphicsState& state, RenderStateV2& rs) = 0;
			virtual void Join() = 0;
			virtual ~IRenderThread()=default;
		};
		class NonThreadedRender : public IRenderThread
		{
		public:
			void Init(FrameRenderer* renderer);
			void Render(const GraphicsState& state, RenderStateV2& rs)override;
			void Join() override;
		private:
			FrameRenderer* _renderer;
		};

		void GrowStates(size_t new_min_size);

		std::pair<vector<ProcessedRO>, DsBindingCount> ProcessRoUniforms(const GraphicsState& draw_calls, UboManager& ubo_manager);
		void RenderGraphicsState(const GraphicsState& state,RenderStateV2& rs);
		VulkanView& View()const { return *_view; }
		
		RscHandle<ShaderProgram> GetMeshRendererShaderModule();
		PipelineManager& GetPipelineManager();
		VulkanPipeline&  GetPipeline(const pipeline_config& config,const vector<RscHandle<ShaderProgram>>& prog);
		//PipelineHandle_t GetPipelineHandle();//Add arguments when we know what determines which pipeline to get.


		VulkanView*                            _view                       {};
		RscHandle<ShaderProgram>               _mesh_renderer_shader_module{};
		PipelineManager*                       _pipeline_manager           {};
		vector<RenderStateV2>                  _states                     {};
		const vector<GraphicsState>*           _gfx_states                 {};
		vector<vk::UniqueCommandBuffer>        _state_cmd_buffers          {};
		vector<std::unique_ptr<IRenderThread>> _render_threads             {};
		vk::CommandPool         _cmd_pool{};
		vk::UniqueCommandBuffer _pri_buffer{};
		vk::UniqueCommandBuffer _transition_buffer{};
	};
}
