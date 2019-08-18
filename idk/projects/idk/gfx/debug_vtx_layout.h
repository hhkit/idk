#pragma once
#include <idk.h>
namespace idk
{

	namespace dbg_vert_layout
	{
		/*
		 per vertex
		 >vertex, 0

		 per instance
		 >color,  1
		 >model transform 2-5
		*/
		constexpr uint32_t vertex_binding = 0,
			instance_binding = 1;
	}
	struct debug_vertex
	{
		vec3 vertex{};
		debug_vertex() = default;
		debug_vertex(vec3 const& v) :vertex{ v } {}
		~debug_vertex() {}
		//glm::vec4 color;
	};
	struct debug_instance
	{
		vec4 color{};
		mat4 model{};
		debug_instance() = default;
		debug_instance(const vec4& col, const mat4& mod) :color{ col }, model{ mod } {}
		~debug_instance() {}
		//glm::vec4 color;
	};


}