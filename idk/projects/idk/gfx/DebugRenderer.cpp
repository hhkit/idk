#include "stdafx.h"
#include "DebugRenderer.h"
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include <math/matrix_transforms.h>

namespace idk
{
	//Temporary, need to use resource manager later
	std::string inline GetBinaryFile(const std::string& filepath)
	{
		std::ifstream file{ filepath,std::ios::binary };
		std::stringstream data;
		data << file.rdbuf();
		file.close();
		return data.str();
	}

	//struct DebugRenderer::pimpl
	//{
	//	std::map<DbgShape, vbo> shape_buffers;
	//};



	void DebugRenderer::Init()
	{
		info = std::make_shared<debug_info>();
		using buffer_desc  = idk::buffer_desc;
		using binding_info = idk::buffer_desc::binding_info;
		using attribute_info = idk::buffer_desc::attribute_info;
		//impl = std::make_unique<pimpl>();

		idk::pipeline_config config;
		auto vert_data = GetBinaryFile("shaders/dbgvertex.vert.spv");
		auto frag_data = GetBinaryFile("shaders/dbgfragment.frag.spv");
		config.frag_shader = frag_data;
		config.vert_shader = vert_data;
		config.fill_type = idk::FillType::eLine;
		config.prim_top = idk::PrimitiveTopology::eTriangleList;
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
					 attribute_info{ idk::AttribFormat::eSVec3,0, offsetof(idk::debug_instance,color) }
					,attribute_info{ idk::AttribFormat::eMat4 ,1, offsetof(idk::debug_instance,model) }
				}
			});

		idk::uniform_info uniform_info;
		Init(config, uniform_info);


	}


	void DebugRenderer::DrawShape(DbgShape shape, vec3 pos, vec3 scale, vec3 axis, idk::rad angle, vec4 color)
	{
		this->info->render_info[shape].emplace_back(debug_info::inst_data{ color, idk::translate(pos) * mat4(idk::rotate(axis,angle) * idk::scale(scale)) });
	}

	void DebugRenderer::Render()
	{
	}


	DebugRenderer::~DebugRenderer()
	{

	}
}