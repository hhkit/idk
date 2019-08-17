#include "pch.h"
#include "DebugRenderer.h"
#include "math2.h"
#include <map>
#include <vector>
#include "Vulkan.h"
#include <VulkanPipeline.h>

using Hvbo = uint32_t;

struct debug_vertex
{
	vec3 vertex;
	debug_vertex() = default;
	debug_vertex(vec3 const& v) :vertex{ v }{}
	~debug_vertex(){}
	//glm::vec4 color;
};

struct vbo
{
	Hvbo handle = {};
	std::vector< debug_vertex> vertices;

	vbo(std::initializer_list<debug_vertex>&& il) :vertices{ il } {}
	vbo(std::vector<debug_vertex>const& verts) :vertices{ verts } {}
	void SetHandle(Hvbo handle);
};

struct DebugRenderer::pimpl
{
	std::map<DbgShape, vbo> shape_buffers;
	idk::VulkGfxPipeline pipeline;
	vgfx::VulkanDetail detail;
	idk::VulkGfxPipeline::uniform_info uniforms;
};

struct DebugRenderer::Pipeline
{

};


void DebugRenderer::Init(GfxSystem& gfx_sys)
{
	impl = std::make_unique<pimpl>();
	impl->detail = gfx_sys.GetDetail();
	impl->shape_buffers = std::map<DbgShape, vbo>
	{
		{DbgShape::eCube   , 
			{
				vec3{-0.5f,-0.5f,0.0f},
				vec3{ 0.5f, 0.5f,0.0f},
				vec3{-0.5f, 0.5f,0.0f},
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
	idk::VulkGfxPipeline::config_t config;
	impl->pipeline.Create(config, impl->detail);
	


}

void DebugRenderer::DrawShape(DbgShape , vec3 , vec3 , quat )
{
}

void DebugRenderer::Render()
{
	auto& cmd_buffer = *impl->detail.CurrCommandbuffer();
	impl->pipeline.Bind(cmd_buffer, impl->detail);
	impl->pipeline.BindUniformDescriptions(cmd_buffer, impl->detail,impl->uniforms);
	//Bind vtx buffers
	//Bind idx buffers
	//Draw
}

void DebugRenderer::RegisterShaders(GfxSystem& )
{
}

void DebugRenderer::RegisterShapes(GfxSystem& )
{
}

std::unique_ptr<DebugRenderer::Pipeline> DebugRenderer::CreateRenderPipeline(GfxSystem& )
{
	return std::unique_ptr<Pipeline>();
}
