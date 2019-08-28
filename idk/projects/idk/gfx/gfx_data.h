#pragma once
#include <idk.h>

namespace idk
{
	using std::optional;
	struct gfx_data
	{
		vector<unsigned char> vertex_buffer_data;
		optional<vector<unsigned char>> index_buffer_data;
		vector<unsigned char> uniform_buffer_data;
		struct uniform_prop {};
		struct vertex_prop {};
		struct index_prop { };
		std::vector<uniform_prop> uniforms;
		std::vector<vertex_prop> vertexs;
		std::vector<index_prop> indexs;
	};
}