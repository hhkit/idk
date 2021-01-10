#include "pch.h"
#include "StandardVertexBindings.h"

#include <vkn/GraphicsState.h>
#include <vkn/BufferHelpers.inl>
#include "ShadowBindings.h"

namespace idk::vkn::bindings
{

	void StandardVertexBindings::SetState(const GraphicsState& vstate) {
		auto& cam = vstate.camera;
		SetState(cam, vstate.GetSkeletonTransforms());
	}

	void StandardVertexBindings::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		state.SetState(cam, skel);
	}

	void StandardVertexBindings::Bind(RenderInterface& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*state.proj_trf;//map back into z: (0,1)
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(projection_trf));
	}
	constexpr size_t elem_sz = sizeof(mat4) * 2;

	void StandardVertexBindings::Bind(RenderInterface& the_interface, const RenderObject& dc)
	{
		if (cache_.empty())
		{
			mat4 obj_trf = state.view_trf * dc.transform;
			mat4 obj_ivt = obj_trf.inverse().transpose();
			vector<mat4> mat4_block{ obj_trf,obj_ivt };
			the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(mat4_block));
		}
		else
		{
			auto& [buffer,offset] = cache_.back();
			the_interface.BindUniform("ObjectMat4Block", 0, buffer,offset,elem_sz);
			cache_.pop_back();
		}
	}

	void StandardVertexBindings::PrepareBindRange(RenderInterface& the_interface, strided_span<const RenderObject> dc_span, span<const size_t> indices)
	{
		cache_.clear();
		auto& ubo_manager=the_interface.GetUboManager();
		auto buffer = ubo_manager.Acquire(elem_sz + indices.size());
		cache_.reserve(indices.size());
		uint32_t offset = 0;
		for (auto idx : indices)
		{
			cache_.emplace_back(UboCache{ buffer.buffer,buffer.buffer_offset+offset });
			offset += elem_sz;
		}
		offset = 0;
		for (auto itr = std::make_reverse_iterator(indices.end()),end = std::make_reverse_iterator(indices.begin()); itr!=end;++itr)
		{
			auto idx = *itr;
			auto& dc = dc_span[idx];
			mat4 trfs[2];
			mat4& obj_trf = trfs[0];
			mat4& obj_ivt = trfs[1];
			obj_trf = state.view_trf* dc.transform;
			obj_ivt = obj_trf.inverse().transpose();
			auto subspan =buffer.data_buffer.subspan(offset,elem_sz);
			memcpy(subspan.data(),&trfs, sizeof(elem_sz));
		}
	}

	void StandardVertexBindings::Bind(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardVertexBindings::BindAni(RenderInterface& the_interface, const AnimatedRenderObject& dc)
	{
		//auto& state = State();
		auto& skels = (*state.skeletons);
		the_interface.BindUniform("BoneMat4Block", 0, hlp::to_data(skels.at(dc.skeleton_index).bones_transforms));
	}
	void VertexShaderBinding::Bind(RenderInterface& the_interface)
	{
		the_interface.BindShader(ShaderStage::Vertex, vertex_shader);
	}
	void StandardVertexBindings::StateInfo::SetState(const GraphicsState& vstate)
	{
		SetState(vstate.camera, *vstate.skeleton_transforms);
	}
	void StandardVertexBindings::StateInfo::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
		skeletons = &skel;
	}
}