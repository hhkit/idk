#pragma once
#include <core/GameObject.h>
#include <future>
#include "ColorPickResult.h"
#include <gfx/RenderObject.h> //Camera Data
namespace idk
{
	struct PickReqData
	{
		vec2 point{};
		CameraData camera;
		size_t inst_mesh_render_begin{}, inst_mesh_render_end{};
		vector<Handle<GameObject>> handles;
	};
	struct ColorPickRequest
	{
		using result_t = Handle<GameObject>;
		ColorPickResult promise(vec2 normalized_point, CameraData camera_data) 
		{ 
			data.point = normalized_point;
			data.camera = camera_data;
			return ColorPickResult{ result };
		}

		void select(uint32_t index) { set_result((index) ? data.handles[index - 1] : result_t{}); }
		void set_result(result_t value) { result.set_value(value); }

		
		PickReqData data;
	private:
		std::promise<result_t> result;
	};
}