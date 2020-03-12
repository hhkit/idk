#pragma once
#include <idk.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vkn/RenderPassObj.h>
#include <variant>
#include <gfx/pipeline_config.h>
#include <vkn/VknTextureView.h>
#include <math/rect.h>
namespace idk::vkn
{
	using VertexBuffer = vk::Buffer;
	using VknRenderPass = RenderPassObj;
	struct VulkanPipeline;

	//enum class IndexType { e16, e32 };
	using IndexType = vk::IndexType;
	using Framebuffer = vk::Framebuffer;
	using VertexBuffer = vk::Buffer;
	using IndexBuffer = vk::Buffer;
	class ShaderModule;

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



	class RenderInterface
	{
	public:
		virtual void BindShader  (ShaderStage stage, RscHandle<ShaderProgram> shader)=0;
		virtual void UnbindShader(ShaderStage shader_stage                          )=0;
		//virtual void SetRenderPass(RenderPassObj render_pass);
		//virtual void SetFrameBuffer(const Framebuffer& fb, uvec2 size);

#pragma region Uniforms
		virtual void BindDescriptorSet(uint32_t set,vk::DescriptorSet ds) = 0;
		virtual void BindUniform(string_view name, uint32_t index, string_view data, bool skip_if_bound = false                                                                 )=0;
		virtual void BindUniform(string_view name, uint32_t index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eGeneral)=0;
		//virtual void BindUniform(vk::DescriptorSet ds, std::optional<string_view> data                                                                                          )=0;
#pragma endregion

		virtual void BindVertexBufferByBinding(uint32_t binding, VertexBuffer vertex_buffer, size_t byte_offset) = 0;
		virtual void BindVertexBuffer(uint32_t location, VertexBuffer vertex_buffer, size_t byte_offset) = 0;
		virtual void BindIndexBuffer(IndexBuffer buffer, size_t offset, IndexType indexType) = 0;

#pragma region Draw
		virtual void Draw       (uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance                     )=0;
		virtual void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance)=0;
#pragma endregion

#pragma region PipelineConfigurations
		
		//If col is nullopt, we clear all the colors from attachment_index onwards.
		virtual bool SetPipeline          (const VulkanPipeline& pipeline                                    ) = 0;
		virtual void SetPipelineConfig    (const pipeline_config& config                                     ) = 0;
		virtual void SetClearColor        (uint32_t attachment_index, std::optional<color> col               ) = 0;//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
		virtual void SetBufferDescriptions(span<const buffer_desc>                                                 ) = 0;
		virtual void SetBlend             (uint32_t attachment_index, AttachmentBlendConfig blend_config = {}) = 0;
		virtual void SetClearDepthStencil (std::optional<float> depth, std::optional<uint8_t> stencil = {}   ) = 0;
		virtual void SetScissors          (rect r                                                            ) = 0;
		virtual void SetViewport          (rect r                                                            ) = 0;
		virtual void SetScissorsViewport  (rect r                                                            ) = 0;
		virtual void SetFillType          (FillType type                                                     ) = 0;
		virtual void SetCullFace          (CullFaceFlags cf                                                  ) = 0;
		virtual void SetPrimitiveTopology (PrimitiveTopology pt                                              ) = 0;
		virtual void SetDepthTest         (bool enabled                                                      ) = 0;
		virtual void SetDepthWrite        (bool enabled                                                      ) = 0;
		virtual void SetStencilTest       (bool enabled                                                      ) = 0;
		virtual void SetStencilWrite      (bool enabled                                                      ) = 0;
#pragma endregion
	};
}