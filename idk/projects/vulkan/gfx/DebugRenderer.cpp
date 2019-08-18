#include "pch.h"
#include "DebugRenderer.h"
#include "math2.h"
#include <map>
#include <vector>

#include <utils/Utils.h>

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


void DebugRenderer::Init(GfxSystem& gfx_sys)
{
	using buffer_desc  = idk::buffer_desc;
	using binding_info = idk::buffer_desc::binding_info;
	using attribute_info = idk::buffer_desc::attribute_info;
	impl = std::make_unique<pimpl>();

	idk::pipeline_config config;
	auto vert_data = GetBinaryFile("shaders/dbgvertex.vert.spv");
	auto frag_data = GetBinaryFile("shaders/dbgfragment.frag.spv");
	config.frag_shader = frag_data;
	config.vert_shader = vert_data;
	config.fill_type = idk::FillType::eLine;
	config.buffer_descriptions.emplace_back(
		buffer_desc{
			binding_info{0,sizeof(idk::debug_vertex),idk::VertexRate::eVertex},
			{
				attribute_info{ idk::AttribFormat::eSVec3,0,0 }
			}
		});
	config.buffer_descriptions.emplace_back(
		buffer_desc{
			binding_info{1,sizeof(idk::debug_instance),idk::VertexRate::eInstance},
			{
				 attribute_info{ idk::AttribFormat::eSVec4,0, offsetof(idk::debug_instance,color) }
				,attribute_info{ idk::AttribFormat::eMat4,1, offsetof(idk::debug_instance,model) }
			}
		});

	idk::uniform_info uniform_info;
	Init(gfx_sys, config, uniform_info);


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
