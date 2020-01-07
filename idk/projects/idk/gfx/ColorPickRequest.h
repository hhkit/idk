#pragma once
#include <core/GameObject.h>
#include <future>
#include "ColorPickResult.h"
namespace idk
{
	struct PickReqData
	{
		ivec2 point{};
		CameraData camera;
		size_t inst_mesh_render_begin{}, inst_mesh_render_end{};
		vector<Handle<GameObject>> handles;
	};
	struct ColorPickRequest
	{
		using result_t = Handle<GameObject>;
		ColorPickResult promise(ivec2 picking_point, CameraData camera_data) 
		{ 
			data.point = picking_point;
			data.camera = camera_data;
			return ColorPickResult{ result };
		}

		void select(uint32_t index) { set_result(data.handles[index]); }
		void set_result(result_t value) { result.set_value(value); }

		
		PickReqData data;
	private:
		std::promise<result_t> result;
	};
}