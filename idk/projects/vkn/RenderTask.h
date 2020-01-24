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
namespace idk::vkn
{
	using VknRenderPass = RenderPassObj;


	struct UboManager;

	struct RenderBundle;// holds the stuff required to actually draw/submit

	enum LoadOp {};
	enum StoreOp {};
	enum IndexType {};
	using Framebuffer = vk::Framebuffer;
	struct VertexBuffer;
	struct IndexBuffer;
	class ShaderModule;
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


	struct RenderTask
	{
		void Associate(size_t subpass_index);

		void BindVertexBuffer(uint32_t binding, const VertexBuffer& vertex_buffer, size_t byte_offset);
		void BindIndexBuffer(const IndexBuffer& buffer, size_t offset, IndexType indexType);

#pragma region Uniforms
		void BindUniform(string_view name,uint32_t index, string_view data);
		void BindUniform(string_view name,uint32_t index, const Texture& texture);
#pragma endregion

		void BindShader(const ShaderModule& shader);
		void SetRenderPass(RenderPassObj render_pass);
		void SetFrameBuffer(const Framebuffer& fb);
		void BindInputAttachments();

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

		
		void SetInputAttachments(span<VknTextureView> input_attachments);

		void ProcessBatches(RenderBundle& render_bundle);

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
			RenderPassObj render_pass;
			vk::Framebuffer frame_buffer;
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

		RenderPassObj curr_rp;
		vk::Framebuffer curr_frame_buffer;

		vector<RenderBatch> batches;
	};








}