#include "pch.h"
#include <vulkan/vulkan.hpp>

#include <vkn/VulkanState.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanDetail.h>
#include <vkn/VulkanHelpers.h>
#include <vkn/RenderState.h>

#include "VulkanDebugRenderer.h"
namespace idk
{
	struct vbo
	{
		uint32_t offset{};
		std::vector< debug_vertex> vertices;

		vbo(std::initializer_list<debug_vertex>&& il) :vertices{ il } {}
		vbo(std::vector<debug_vertex>const& verts) :vertices{ verts } {}
		void SetHandle(uint32_t handle) { offset = handle; }
		vk::ArrayProxy<const unsigned char> ToProxy()const
		{
			return vhlp::make_array_proxy(vhlp::buffer_size<uint32_t>(vertices),
				idk::r_cast<const unsigned char*>(std::data(vertices))
			);
		}
	};

	struct VulkanDebugRenderer::pimpl
	{
		vgfx::VulkanDetail& detail;
		idk::VulkGfxPipeline pipeline{};
		idk::VulkGfxPipeline::uniform_info uniforms{};
		idk::hash_table<DbgShape, vbo> shape_buffers{};
		idk::hash_table<DbgShape, vector<debug_instance>> instance_buffers{};

		pimpl(vgfx::VulkanDetail& deets) :detail{ deets } {};
	};

	
	VulkanDebugRenderer::VulkanDebugRenderer(VulkanState& vulkan):vulkan_{&vulkan}
	{
	}

	void VulkanDebugRenderer::Init(const idk::pipeline_config& config, const idk::uniform_info& uniform_info)
	{
		auto& system = *vulkan_;
		impl = std::make_unique<pimpl>(system.GetDetail());
		impl->pipeline.Create(config, impl->detail);

		impl->shape_buffers = idk::hash_table<DbgShape, vbo>
		{
			{DbgShape::eCube   ,
				{
					vec3{-0.5f,-0.5f,1.0f},
					vec3{ 0.5f, 0.5f,1.0f},
					vec3{-0.5f, 0.5f,1.0f},
					//vec3{ 0.5f, 0.5f,1.0f},
					//vec3{-0.5f,-0.5f,1.0f},
					//vec3{-0.5f, 0.5f,1.0f},
				}
			}
			,
			{DbgShape::eSquare,
				{
					vec3{-0.5f,-0.5f,0.0f},
					vec3{ 0.5f, 0.5f,0.0f},
					vec3{-0.5f, 0.5f,0.0f},
					vec3{ 0.0f,-0.5f,0.0f},
					vec3{-0.5f,-0.5f,0.0f},
					vec3{ 0.5f, 0.5f,0.0f},
				}
			}
		};
	}

	void idk::VulkanDebugRenderer::Render()
	{
		auto& cmd_buffer = *impl->detail.CurrCommandbuffer();
		auto& detail   = impl->detail  ;
		auto& pipeline = impl->pipeline;
		auto& uniforms = impl->uniforms;
		vgfx::DrawCall dc;
		dc.pipeline = &pipeline;
		dc.uniform_info = uniforms;
		//pipeline.Bind(cmd_buffer, detail);
		//pipeline.BindUniformDescriptions(cmd_buffer, detail, uniforms);
		for (auto& [shape, buffer] : info->render_info)
		{
			auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();
			//Bind vtx buffers
			auto instance_buffer = detail.AddToMasterBuffer(ArrData(buffer), vhlp::buffer_size<uint32_t>(buffer));
			auto vertex_buffer   = detail.AddToMasterBuffer(idk::s_cast<const void*>(ArrData(shape_buffer_proxy)), vhlp::buffer_size<uint32_t>(shape_buffer_proxy));
			dc.instance_count = ArrCount(buffer);
			dc.vertex_count   = ArrCount(shape_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::vertex_binding, vertex_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::instance_binding, instance_buffer);
			//cmd_buffer.bindVertexBuffers(dbg_vert_layout::instance_binding, detail.CurrMasterVtxBuffer(), instance_buffer, detail.Dispatcher());
			//cmd_buffer.bindVertexBuffers(dbg_vert_layout::vertex_binding  , detail.CurrMasterVtxBuffer(), vertex_buffer  , detail.Dispatcher());

			//Bind idx buffers
			//auto  index_buffer = detail.AddToMasterBuffer();
			//cmd_buffer.bindIndexBuffer(detail.CurrMasterVtxBuffer(),)

			//Bind uniform buffer

			//Draw
			//cmd_buffer.draw(ArrCount(shape_buffer), ArrCount(buffer), 0, 0, detail.Dispatcher());
			detail.CurrRenderState().AddDrawCall(dc);
		}
		
	}
	VulkanDebugRenderer::~VulkanDebugRenderer()
	{

	}
}