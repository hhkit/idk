#include "pch.h"
#pragma once
#include<vector>
#include "color.h"
#include "math2.h"
struct dbg_render_obj
{
	uint32_t mesh;
	color color;
	mat4 transform;
};

class debug_info
{
public:
	using obj_list = std::vector<dbg_render_obj>;
	void AddObject   (dbg_render_obj const& obj);
	void ClearObjects();
	const obj_list& GetList()const;
private:
	obj_list list;
};