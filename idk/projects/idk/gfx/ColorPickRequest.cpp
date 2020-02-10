#include <stdafx.h>
#include "gfx\ColorPickRequest.h"
namespace idk
{

ColorPickResult ColorPickRequest::promise(vec2 normalized_point, CameraData camera_data)
{
	data.point = normalized_point;
	data.camera = camera_data;
	return ColorPickResult{ result };
}

void ColorPickRequest::select(uint32_t index)
{
	set_result((index) ? GetHandle(index) : result_t{});
}

void ColorPickRequest::set_result(result_t value) 
{ 
	try
	{
	result.set_value(value);

	}
	catch (std::future_error& err)
	{
		LOG_TO(LogPool::GFX, "Error [Color picking]: Failed to set result, future is missing?\n - Message: \"%s\"",err.what());
	}
}

// // 
ColorPickResult::result_t ColorPickRequest::GetHandle(uint32_t id) const
{
	auto index = id - 1;
	ColorPickResult::result_t res;

	if (index < data.handles.size())
		res = data.handles[index];
	else {
		index = index - static_cast<uint32_t>(data.handles.size());
		if (index < (*data.ani_handles).size())
		{
			res = (*data.ani_handles)[index];
		}
	}

	return res;
}

}