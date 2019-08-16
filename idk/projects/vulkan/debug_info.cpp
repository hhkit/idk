#include "pch.h"
#include "debug_info.h"

void debug_info::AddObject(dbg_render_obj const& obj)
{
	list.emplace_back(obj);
}

void debug_info::ClearObjects()
{
	list.clear();
}

const debug_info::obj_list& debug_info::GetList() const
{
	// TODO: insert return statement here
	return list;
}
