#include "pch.h"
#include "PipelineBinders.inl"
#include <vkn/PipelineThingy.h>
#include <gfx/GraphicsSystem.h>
#include <vkn/GraphicsState.h>

#include <vkn/VknTexture.h>
#include <vkn/VknRenderTarget.h>
#include <vkn/ShaderModule.h>
#include <gfx/RenderObject.h>
#include <gfx/MaterialInstance.h>


#include <vkn/utils/utils.inl>

namespace idk::vkn
{
	void BindCameraStuff(PipelineThingy& the_interface, const CameraData& cam)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*cam.projection_matrix;//map back into z: (0,1)
		the_interface.BindUniformBuffer("CameraBlock", 0, projection_trf);
	}

	string PrepareLightBlock(const CameraData& cam, const vector<LightData>& lights)
	{
		vector<BaseLightData> tmp_light(lights.size());
		for (size_t i = 0; i < tmp_light.size(); ++i)
		{
			auto& light = tmp_light[i] = (lights)[i];
			light.v_pos = cam.view_matrix * vec4{ light.v_pos,1 };
			light.v_dir = (cam.view_matrix * vec4{ light.v_dir,0 }).get_normalized();
		}

		string light_block;
		uint32_t len = s_cast<uint32_t>(tmp_light.size());
		light_block += string{ reinterpret_cast<const char*>(&len),sizeof(len) };
		light_block += string(16 - sizeof(len), '\0');
		light_block += string{ reinterpret_cast<const char*>(tmp_light.data()), hlp::buffer_size(tmp_light) };
		return light_block;
	}


	void StandardBindings::Bind(PipelineThingy& the_interface) {}

	void StandardBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc) {}

	void StandardBindings::Bind(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardBindings::BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc) {}

	//const GraphicsState& StandardVertexBindings::State() { return *_state; }

	void StandardVertexBindings::SetState(const GraphicsState& vstate) {
		auto& cam = vstate.camera;
		SetState(cam,vstate.GetSkeletonTransforms());
	}

	void StandardVertexBindings::SetState(const CameraData& cam, const vector<SkeletonTransforms>& skel)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
		skeletons = &skel;
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*proj_trf;//map back into z: (0,1)
		the_interface.BindUniformBuffer("CameraBlock", 0, projection_trf);
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		mat4 obj_trf = view_trf * dc.transform;
		mat4 obj_ivt = obj_trf.inverse().transpose();
		vector<mat4> mat4_block{ obj_trf,obj_ivt };
		the_interface.BindUniformBuffer("ObjectMat4Block", 0, mat4_block);
	}

	void StandardVertexBindings::Bind(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		Bind(the_interface, s_cast<const RenderObject&>(dc));
		BindAni(the_interface, dc);
	}

	void StandardVertexBindings::BindAni(PipelineThingy& the_interface, const AnimatedRenderObject& dc)
	{
		//auto& state = State();
		the_interface.BindUniformBuffer("BoneMat4Block", 0, (*skeletons)[dc.skeleton_index].bones_transforms);
	}

	const GraphicsState& PbrFwdBindings::State() { return *_state; }

	void PbrFwdBindings::SetState(const GraphicsState& vstate) {
		_state = &vstate;
		auto& state = State();
		cam = state.camera;
		light_block = PrepareLightBlock(cam, *state.lights);
		view_trf = cam.view_matrix;
		pbr_trf = view_trf.inverse();
	}

	void PbrFwdBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		auto& state = State();
		the_interface.BindUniformBuffer("LightBlock", 0, light_block, true);//skip if pbr is already bound(not per instance)
		the_interface.BindUniformBuffer("PBRBlock", 0, pbr_trf, true);//skip if pbr is already bound(not per instance)
		uint32_t i = 0;
		//Bind the shadow maps
		for (auto& shadow_map : state.shadow_maps_2d)
		{
			auto& sm_uni = shadow_map;
			{
				auto& depth_tex = sm_uni.as<VknTexture>();
				the_interface.BindSampler("shadow_maps", i++, depth_tex);
			}
		}
		//TODO change to cubemap stuff
		//Bind the shadow cube maps
		//for (auto& shadow_map : state.shadow_maps_cube)
		//{
		//	auto& sm_uni = shadow_map;
		//	{
		//		auto& depth_tex = sm_uni.as<VknTexture>();
		//		the_interface.BindSampler("shadow_cube_maps", i++, depth_tex);
		//	}
		//}
	}


	//Assumes that the material is valid.

	void StandardMaterialFragBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		auto& mat_inst = *dc.material_instance;
		auto& mat = *mat_inst.material;
		the_interface.BindShader(ShaderStage::Fragment, mat._shader_program);
	}

	void StandardMaterialBindings::SetState(const GraphicsState& vstate) {
		_state = &vstate;
		auto& state = State();
	}

	//Assumes that the material is valid.

	void StandardMaterialBindings::Bind(PipelineThingy& the_interface, const RenderObject& dc)
	{
		//Bind the material uniforms
		{
			auto& mat_inst = *dc.material_instance;
			auto& mat = *mat_inst.material;
			auto mat_cache = mat_inst.get_cache();
			for (auto itr = mat_cache.uniforms.begin(); itr != mat_cache.uniforms.end(); ++itr)
			{
				if (mat_cache.IsUniformBlock(itr))
				{
					the_interface.BindUniformBuffer(itr->first, 0, mat_cache.GetUniformBlock(itr));
				}
				else if (mat_cache.IsImageBlock(itr))
				{
					auto img_block = mat_cache.GetImageBlock(itr);
					uint32_t i = 0;
					for (auto& img : img_block)
					{
						the_interface.BindSampler(itr->first.substr(0, itr->first.find_first_of('[')), i++, img.as<VknTexture>());
					}
				}
			}
		}
	}

	void StandardMaterialBindings::BindAni(PipelineThingy& , const AnimatedRenderObject& )
	{
	}

}