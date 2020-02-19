#include "pch.h"
#include "ShadowBindings.h"
namespace idk::vkn::bindings
{

	bool ShadowFilter::Skip(RenderInterface&, const RenderObject& dc)
	{
		return !(dc.layer_mask & filter);
	}

	void ShadowFilter::SetState(const CameraData& cam, const vector<SkeletonTransforms>&)
	{
		filter = cam.culling_flags;
	}
}