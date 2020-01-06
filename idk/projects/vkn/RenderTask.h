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
namespace idk::vkn
{
	using VknRenderPass = int;
	using VknFrameBuffer = int;



	enum LoadOp {};
	enum StoreOp {};
	enum IndexType {};
	struct Framebuffer;
	struct VertexBuffer;
	struct IndexBuffer;
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
		void BindIndexBuffer(const IndexBuffer& buffer, size_t offset, IndexType indexType);

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








}