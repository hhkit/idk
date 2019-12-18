#pragma once
#include <cstdint>
#include <gfx/pipeline_config.h>
#include <vkn/utils/Flags.h>
namespace idk::vkn
{
	enum LoadOp {};
	enum StoreOp {};
	struct Framebuffer;
	struct VertexBuffer;
	struct ShaderModule;
	struct Texture;
	using TextureID  = Guid;
	using CullFaceFlags = Flags<CullFace>;

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

	struct Shaders {};

	struct index_span
	{
		size_t begin = 0, end = 0;
	};
	struct AttachmentDescription
	{
		ivec2 size;
		//Format format;
	};
	struct AttachmentInfo
	{
		TextureID target;
		std::optional<AttachmentBlendConfig> config;
	};

	/*
	****************
	Objective:
		- Provide an interface to deal with a sub renderpass
		- Be processed by a system to generate the appropriate vulkan render pass and pipeline information
	****************
	*/

	struct SubPass
	{
#pragma region FrameBuffers
		void BindInputAttachment(uint32_t index, const Texture& texture);

		void SetOutputAttachment(uint32_t index, const Texture& texture, LoadOp load_op, StoreOp store_op);
		void SetDepthtAttachment(const Texture& texture, LoadOp load_op, StoreOp store_op);
		void SetAttachmentBlending(uint32_t index, AttachmentBlendConfig blend);
#pragma endregion
		size_t subpass_index; //generated and assigned by a subpass system
	private:
		vector<AttachmentInfo> input_attachments;
		vector<AttachmentInfo> output_attachments;
	};

	struct RenderTask
	{
		void Associate(size_t subpass_index);

		void BindVertexBuffer(uint32_t binding, const VertexBuffer& vertex_buffer, size_t byte_offset);

#pragma region Uniforms
		void BindUniform(string_view name, string_view data);
		void BindUniform(string_view name, const Texture& texture);
#pragma endregion

		void BindShader(const ShaderModule& shader);

#pragma region Draw
		void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance);
		void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance);
#pragma endregion

#pragma region PipelineConfigurations
		void Inherit(const pipeline_config& config);
		//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
		void SetClearColor(uint32_t attachment_index, std::optional<color> col);
		void SetClearDepthStencil(std::optional<float> depth, std::optional<uint8_t> stencil);
		void SetScissors(rect);
		void SetViewport(rect);
		void SetFillType(FillType);
		void SetCullFace(CullFace);
		void SetPrimitiveTopology(PrimitiveTopology);
		void SetDepthTest(bool enabled);
		void SetDepthWrite(bool enabled);
		void SetStencilTest(bool enabled);
		void SetStencilWrite(bool enabled);
#pragma endregion 
		const pipeline_config GetCurrentConfig()const noexcept;
	private:


		struct UboData {};
		struct TexData {};
		struct VertexBindingData
		{
			//vk::Buffer buffer;
			uint32_t binding;
			uint32_t offset;
		};
		struct DrawCall
		{
			//Use spans so that this is flat and we don't pay for multiple allocations (versus vector)
			//Use indices so that even if we transform the buffers, as long as the order doesn't change, our indices remain correct.
			index_span ubos;
			index_span texs;
			index_span vertex_buffers;

			draw_info draw_info; //might need to split into two types/vectors of draw calls to prevent branching
		};

		struct RenderBatch
		{
			pipeline_config pipeline;
			Shaders shaders;
			vector<DrawCall> draw_calls;
		};



#pragma region Initial Data
		vector<UboData> ubos;
		vector<TexData> uniform_textures;

		vector<VertexBindingData> vertex_buffers;
#pragma endregion 
		std::optional<vec4> clear;

		pipeline_config curr_config;

		vector<RenderBatch> batches;
	};

	struct FrameGraphResource {};
	struct FrameGraphNode {};
	struct FrameGraphBuilder 
	{
		//Keep track of virtual resources
		AttachmentInfo CreateTexture(AttachmentDescription desc);
		AttachmentInfo CreateTexture(const Texture& texture);
		
		FrameGraphResource read(FrameGraphResource);
		FrameGraphResource write(FrameGraphResource);

		void BeginNode();
		FrameGraphNode EndNode();
	};
	struct FrameGraph
	{
		//Maybe do some tuple get thing based on the pre-registered context?
		using Context_t = int;
		template<typename T>
		using ExecFunc = std::function<void(T&, Context_t context)>;

		struct BaseEntry
		{
			virtual void Execute(Context_t context) = 0;
			//virtual void Execute(ContextTuple_t context_tuple)=0;
		};

		template<typename T>//,...Args
		struct Entry
		{
			T value;
			ExecFunc<T> execute;
			template<typename ... Args>
			Entry(ExecFunc<T>&& func, Args&& ... args) : value{ std::forward<Args>(args)... },execute { std::move(func) }
			{

			}
			void Execute(Context_t context)override
			{
				execute(value, context);
			}
			/*
			void Execute(ContextTuple_t context)override
			{
				execute(value, context);
			}
			*/
		};
		//Downside: Lambdas are probably out of the question unless we can deduce the appropriate std::function to store them
		template<typename T, typename Func>
		struct Entry2;
		template<typename T, typename ...Args>
		struct Entry2<T,std::function<void(T&,Args...)>>
		{
			
			using ExecFunc = std::function<void(T&,Args...)>;
			T value;
			ExecFunc execute;
			Entry(ExecFunc&& func) :execute{ std::move(func)} {}

			/*
			void Execute(ContextTuple_t context)override
			{
				execute(value, std::get<Args>(context)...);
			}
			*/
		};

		template<typename T,typename ... T_CtorArgs>
		T& addCallBackPass(string_view name,
			std::function<void(FrameGraphBuilder& builder, T&)> init,
			ExecFunc<T> execute,
			T_CtorArgs&& ... args
		)
		{
			auto ptr = std::make_unique<Entry<T>>(std::move(execute),std::forward<T_CtorArgs(args)...);
			auto& value = ptr->value;
			//Begin the node
			graph_builder.BeginNode();
			init(graph_builder, value);
			entries.emplace_back(std::move(ptr));
			//End the node
			nodes.emplace_back(graph_builder.EndNode());
			return value;
		}
		template<typename T,typename ...CtorArgs>
		T& addRenderPass(string_view name,CtorArgs&&...args)
		{
			return addCallBackPass<T>(name,
				[](FrameGraphBuilder&, T&) {},
				[](T& t, Context_t context) {t.execute(context); },
				graph_builder, std::forward<CtorArgs>(args)...
				);
		}
		FrameGraphBuilder graph_builder;
		vector<FrameGraphNode> nodes;
		vector<std::unique_ptr<BaseEntry>> entries;
	};


}