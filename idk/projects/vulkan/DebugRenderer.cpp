#include "pch.h"
#include "DebugRenderer.h"
#include "math2.h"
#include <map>
#include <vector>
#include "Vulkan.h"

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
};

struct DebugRenderer::Pipeline
{

};


void DebugRenderer::Init(GfxSystem& )
{
	impl = std::make_unique<pimpl>();
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

	


}

void DebugRenderer::DrawShape(DbgShape , vec3 , vec3 , quat )
{
}

void DebugRenderer::Render()
{
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
