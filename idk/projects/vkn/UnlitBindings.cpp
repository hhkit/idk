#include "pch.h"
#include "UnlitBindings.h"

namespace idk::vkn::bindings
{
	bool UnlitFilter::Skip(RenderInterface&, const RenderObject& dc)
	{
		return !dc.material_instance || !dc.material_instance->material || dc.material_instance->material->model != ShadingModel::Unlit;
	}
}