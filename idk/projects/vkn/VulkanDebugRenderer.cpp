#include "pch.h"
#include <vulkan/vulkan.hpp>

#include <vkn/VulkanState.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>

#include "VulkanDebugRenderer.h"
namespace idk::vkn
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
			return hlp::make_array_proxy(hlp::buffer_size<uint32_t>(vertices),
				idk::r_cast<const unsigned char*>(std::data(vertices))
			);
		}
	};

	struct VulkanDebugRenderer::pimpl
	{
		VulkanView& detail;
		VulkanPipeline pipeline{};
		uniform_info uniforms{};
		hash_table<DbgShape, vbo> shape_buffers{};
		hash_table<DbgShape, vector<debug_instance>> instance_buffers{};

		pimpl(VulkanView& deets) :detail{ deets } {};
	};

	
	VulkanDebugRenderer::VulkanDebugRenderer(VulkanState& vulkan):vulkan_{&vulkan}
	{
	}

	void VulkanDebugRenderer::Init(const idk::pipeline_config& config, const idk::uniform_info& uniform_info)
	{
		auto& system = *vulkan_;
		impl = std::make_unique<pimpl>(system.View());
		impl->pipeline.Create(config, impl->detail);

		impl->shape_buffers = idk::hash_table<DbgShape, vbo>
		{
			{DbgShape::eCube   ,
				{
					vec3{-0.5f,-0.5f,1.0f},
					vec3{ 0.5f, 0.5f,1.0f},
					vec3{-0.5f, 0.5f,1.0f},
				}
			}
			,
			{DbgShape::eSquare,
				{
					vec3{-0.5f,-0.5f,0.0f},
					vec3{-0.5f, 0.5f,0.0f},
					vec3{ 0.5f, 0.5f,0.0f},

					vec3{ 0.5f, 0.5f,0.0f},
					vec3{ 0.5f,-0.5f,0.0f},
					vec3{-0.5f,-0.5f,0.0f},
				}
			}
		};
	}

	void VulkanDebugRenderer::Shutdown() 
	{
		this->info.reset();
		this->impl.reset();
	}

	void VulkanDebugRenderer::Render()
	{
		auto& cmd_buffer = *impl->detail.CurrCommandbuffer();
		auto& detail   = impl->detail  ;
		auto& pipeline = impl->pipeline;
		auto& uniforms = impl->uniforms;
		draw_call dc;
		dc.pipeline = &pipeline;
		dc.uniform_info = uniforms;

		for (auto& [shape, buffer] : info->render_info)
		{
			auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();

			//Bind vtx buffers
			auto instance_buffer = detail.AddToMasterBuffer(std::data(buffer), hlp::buffer_size<uint32_t>(buffer));
			auto vertex_buffer   = detail.AddToMasterBuffer(idk::s_cast<const void*>(std::data(shape_buffer_proxy)), hlp::buffer_size<uint32_t>(shape_buffer_proxy));
			dc.instance_count = hlp::arr_count(buffer);
			dc.vertex_count   = hlp::arr_count(shape_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::vertex_binding, vertex_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::instance_binding, instance_buffer);

			//Bind idx buffers
			//auto  index_buffer = detail.AddToMasterBuffer();
			//cmd_buffer.bindIndexBuffer(detail.CurrMasterVtxBuffer(),)

			//Bind uniform buffer

			//Draw
			//cmd_buffer.draw(ArrCount(shape_buffer), ArrCount(buffer), 0, 0, detail.Dispatcher());
			detail.CurrRenderState().AddDrawCall(dc);
		}	
		info->render_info.clear();
	}

	VulkanDebugRenderer::~VulkanDebugRenderer() = default;
}